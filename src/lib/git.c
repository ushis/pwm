#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "git.h"

int
pwm_git_open(pwm_git_t *git, const char *path) {
  struct stat info;

  if (stat(path, &info) < 0) {
    if (errno != ENOENT) {
      perror("pwm_git_new: stat");
      return -1;
    }

    if (git_repository_init(&git->repo, path, 1) < 0) {
      fprintf(stderr, "pwm_git_open: %s\n", giterr_last()->message);
      return -1;
    }

    if (chmod(path, 0700) < 0) {
      perror("pwm_git_open: chmod");
      return -1;
    }
    return 0;
  }

  if ((info.st_mode&(S_IRWXG|S_IRWXO)) != 0) {
    fprintf(stderr, "pwm_git_open: insecure permissions on: %s\n", path);
    return -1;
  }

  if (git_repository_open(&git->repo, path) < 0) {
    fprintf(stderr, "pwm_git_open: %s\n", giterr_last()->message);
    return -1;
  }
  return 0;
}

int
pwm_git_new(pwm_git_t **out, const char *path) {
  pwm_git_t *git;

  git_threads_init();

  if ((git = (pwm_git_t *) calloc(1, sizeof(pwm_git_t))) == NULL) {
    perror("calloc");
    goto fail;
  }

  if (pwm_git_open(git, path) < 0) {
    goto fail;
  }

  if (git_signature_default(&git->sig, git->repo) < 0) {
    fprintf(stderr, "pwm_git_new: %s\n", giterr_last()->message);
    goto fail;
  }

  if (git_repository_is_empty(git->repo)) {
    *out = git;
    return 0;
  }

  if (git_revparse_single((git_object **) &git->tree, git->repo, "HEAD^{tree}") < 0) {
    fprintf(stderr, "pwm_git_new: %s\n", giterr_last()->message);
    goto fail;
  }
  *out = git;
  return 0;

fail:
  pwm_git_free(git);
  return -1;
}

void
pwm_git_free(pwm_git_t *git) {
  if (git != NULL) {
    git_repository_free(git->repo);
    git_signature_free(git->sig);
    git_tree_free(git->tree);
    free(git);
  }
}

int
pwm_git_add(pwm_git_t *git, const char *path, const pwm_str_t *s) {
  git_treebuilder *bld = NULL;
  git_oid oid;
  int err;

  if ((err = git_blob_create_frombuffer(&oid, git->repo, s->buf, s->len)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_create(&bld, git->tree)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_insert(NULL, bld, path, &oid, 0100644)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_write(&oid, git->repo, bld)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_tree_lookup(&git->tree, git->repo, &oid)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
  }

cleanup:
  git_treebuilder_free(bld);
  return err;
}

int
pwm_git_rm(pwm_git_t *git, const char *path) {
  git_treebuilder *bld = NULL;
  git_oid oid;
  int err;

  if ((err = git_treebuilder_create(&bld, git->tree)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_remove(bld, path)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_write(&oid, git->repo, bld)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_tree_lookup(&git->tree, git->repo, &oid)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
  }

cleanup:
  git_treebuilder_free(bld);
  return err;
}

int
pwm_git_head(pwm_git_t *git, git_commit **commit) {
  return git_revparse_single((git_object **) commit, git->repo, "HEAD");
}

int
pwm_git_commit(pwm_git_t *git, const char *msg) {
  git_commit *head = NULL;
  git_oid oid;
  size_t n = 0;
  int err;

  if (!git_repository_is_empty(git->repo)) {
    if (pwm_git_head(git, &head) < 0) {
      fprintf(stderr, "pwm_git_commit: %s\n", giterr_last()->message);
      return -1;
    }
    n = 1;
  }
  err = git_commit_create_v(&oid, git->repo, "HEAD", git->sig, git->sig,
                            NULL, msg, git->tree, n, head);

  git_commit_free(head);
  return err;
}

int
pwm_git_clean(pwm_git_t *git) {
  int err;
  git_oid oid;
  git_object *obj = NULL;
  git_reset_t reset_type = GIT_RESET_HARD;
  git_checkout_opts opts = GIT_CHECKOUT_OPTS_INIT;

  if ((err = git_reference_name_to_id(&oid, git->repo, "HEAD")) < 0) {
    goto cleanup;
  }

  if ((err = git_object_lookup(&obj, git->repo, &oid, GIT_OBJ_COMMIT)) < 0) {
    goto cleanup;
  }

  if (git_repository_is_bare(git->repo)) {
    reset_type = GIT_RESET_SOFT;
  }

  if ((err = git_reset(git->repo, obj, reset_type))  < 0) {
    goto cleanup;
  }

  if (reset_type == GIT_RESET_HARD) {
    opts.checkout_strategy = GIT_CHECKOUT_REMOVE_UNTRACKED;
    err = git_checkout_head(git->repo, &opts);
  }

cleanup:
  git_object_free(obj);
  return err;
}

int
pwm_git_entry_content(pwm_git_t *git, const char *path, pwm_str_t *dst) {
  const git_tree_entry *entry;
  git_blob *blob = NULL;
  int err;

  if ((entry = git_tree_entry_byname(git->tree, path)) == NULL) {
    fprintf(stderr, "pwm_git_entry_content: %s\n", giterr_last()->message);
    return -1;
  }

  if ((err = git_tree_entry_to_object((git_object **) &blob, git->repo, entry)) < 0) {
    fprintf(stderr, "pwm_git_entry_content: %s\n", giterr_last()->message);
    goto cleanup;
  }
  err = pwm_str_set(dst, git_blob_rawcontent(blob), git_blob_rawsize(blob));

cleanup:
  git_blob_free(blob);
  return err;
}

int
pwm_git_entry_exists(pwm_git_t *git, const char *path) {
  if (git_repository_is_empty(git->repo)) {
    return 0;
  }
  return git_tree_entry_byname(git->tree, path) != NULL;
}

int
pwm_git_walk_cb(const char *root, const git_tree_entry *entry, void *cb) {
  return ((pwm_git_walk_entries_cb) cb)(git_tree_entry_name(entry));
}

int
pwm_git_walk_entries(pwm_git_t *git, pwm_git_walk_entries_cb cb) {
  if (git_repository_is_empty(git->repo)) {
    return 0;
  }
  return git_tree_walk(git->tree, GIT_TREEWALK_PRE, pwm_git_walk_cb, cb);
}

int
pwm_git_walk_log(pwm_git_t *git, pwm_git_walk_log_cb cb) {
  git_commit *commit, *parent;
  const char *msg, *name;
  int64_t time;
  int err;

  if (git_repository_is_empty(git->repo)) {
    return 0;
  }

  if (pwm_git_head(git, &commit) < 0) {
    fprintf(stderr, "pwm_git_walk_log: %s\n", giterr_last()->message);
    return -1;
  }

  for (;;) {
    time = git_commit_time(commit);
    name = git_commit_committer(commit)->name;
    msg = git_commit_message(commit);

    if ((err = cb(time, name, msg)) < 0) {
      break;
    }

    if (git_commit_parentcount(commit) == 0) {
      break;
    }

    if ((err = git_commit_parent(&parent, commit, 0)) < 0) {
      break;
    }
    git_commit_free(commit);
    commit = parent;
  }
  git_commit_free(commit);
  return err;
}
