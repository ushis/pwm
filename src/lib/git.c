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

    if (git_repository_init(&git->repo, path, 0) < 0) {
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

pwm_git_t *
pwm_git_new(const char *path) {
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

  if (git_repository_index(&git->index, git->repo) < 0) {
    fprintf(stderr, "pwm_git_new: %s\n", giterr_last()->message);
    goto fail;
  }

  if (git_index_read(git->index, 0) < 0) {
    fprintf(stderr, "pwm_git_new: %s\n", giterr_last()->message);
    goto fail;
  }
  return git;

fail:
  pwm_git_free(git);
  return NULL;
}

void
pwm_git_free(pwm_git_t *git) {
  if (git != NULL) {
    git_repository_free(git->repo);
    git_signature_free(git->sig);
    git_index_free(git->index);
    free(git);
  }
}

int
pwm_git_last_commit(pwm_git_t *git, git_commit **commit) {
  git_oid oid;

  if (git_reference_name_to_id(&oid, git->repo, "HEAD") < 0) {
    return -1;
  }
  return git_commit_lookup(commit, git->repo, &oid);
}

int
pwm_git_tree(pwm_git_t *git, git_tree **tree) {
  git_oid oid;

  if (git_index_write_tree(&oid, git->index) < 0) {
    return -1;
  }
  return git_tree_lookup(tree, git->repo, &oid);
}

int
pwm_git_blob(pwm_git_t *git, git_blob **blob, const char *path) {
  git_oid oid;

  if (git_blob_create_fromworkdir(&oid, git->repo, path) < 0) {
    return -1;
  }
  return git_blob_lookup(blob, git->repo, &oid);
}

int
pwm_git_add(pwm_git_t *git, const char *path) {
  if (git_index_add_bypath(git->index, path) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    return -1;
  }

  if (git_index_write(git->index) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    return -1;
  }
  return 0;
}

int
pwm_git_rm(pwm_git_t *git, const char *path) {
  if (git_index_remove_bypath(git->index, path) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    return -1;
  }

  if (git_index_write(git->index) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    return -1;
  }
  return 0;
}

int
pwm_git_commit(pwm_git_t *git, const char *msg) {
  int err;
  size_t n = 0;
  git_oid oid;
  git_tree *tree = NULL;
  git_commit *last = NULL;

  if ((err = pwm_git_tree(git, &tree)) < 0) {
    fprintf(stderr, "pwm_git_commit: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if (!git_repository_is_empty(git->repo)) {
    if ((err = pwm_git_last_commit(git, &last)) < 0) {
      fprintf(stderr, "pwm_git_commit: %s\n", giterr_last()->message);
      goto cleanup;
    }
    n = 1;
  }
  err = git_commit_create_v(&oid, git->repo, "HEAD", git->sig, git->sig,
                            NULL, msg, tree, n, last);

cleanup:
  git_commit_free(last);
  git_tree_free(tree);
  return err;
}

int
pwm_git_clean(pwm_git_t *git) {
  int err;
  git_oid oid;
  git_object *obj = NULL;
  git_checkout_opts opts = GIT_CHECKOUT_OPTS_INIT;

  if ((err = git_reference_name_to_id(&oid, git->repo, "HEAD")) < 0) {
    goto cleanup;
  }

  if ((err = git_object_lookup(&obj, git->repo, &oid, GIT_OBJ_COMMIT)) < 0) {
    goto cleanup;
  }

  if ((err = git_reset(git->repo, obj, GIT_RESET_HARD))  < 0) {
    goto cleanup;
  }
  opts.checkout_strategy = GIT_CHECKOUT_REMOVE_UNTRACKED;
  err = git_checkout_head(git->repo, &opts);

cleanup:
  git_object_free(obj);
  return err;
}

int
pwm_git_entry_content(pwm_git_t *git, pwm_str_t *dst, const char *path) {
  int err;
  git_blob *blob;

  if (pwm_git_blob(git, &blob, path) < 0) {
    fprintf(stderr, "pwm_git_blob_content: %s\n", giterr_last()->message);
    return -1;
  }
  err = pwm_str_set(dst, git_blob_rawcontent(blob), git_blob_rawsize(blob));
  git_blob_free(blob);
  return err;
}

int
pwm_git_entry_exists(pwm_git_t *git, const char *path) {
  git_oid oid;
  return git_blob_create_fromworkdir(&oid, git->repo, path) == 0;
}

int
pwm_git_walk_cb(const char *root, const git_tree_entry *entry, void *cb) {
  pwm_git_walk_entries_cb callback = cb;
  return callback(git_tree_entry_name(entry));
}

int
pwm_git_walk_entries(pwm_git_t *git, pwm_git_walk_entries_cb cb) {
  git_tree *tree = NULL;
  int err;

  if ((err = pwm_git_tree(git, &tree)) >= 0) {
    err = git_tree_walk(tree, GIT_TREEWALK_PRE, pwm_git_walk_cb, cb);
  }
  git_tree_free(tree);
  return err;
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

  if (pwm_git_last_commit(git, &commit) < 0) {
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
