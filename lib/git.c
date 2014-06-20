#include "git.h"
#include "def.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int
pwm_git_init() {
  return git_threads_init();
}

void
pwm_git_shutdown() {
  git_threads_shutdown();
}

static int
pwm_git_find_home(pwm_str_t *s) {
  const char *buf;

  if ((buf = getenv(PWM_HOME_ENV_VAR)) != NULL) {
    return pwm_str_set(s, buf, strlen(buf));
  }

  if ((buf = getenv("HOME")) == NULL) {
    return -1;
  }

  if (pwm_str_set(s, buf, strlen(buf)) < 0) {
    return -1;
  }
  return pwm_str_append_path_component(s, PWM_HOME_NAME, strlen(PWM_HOME_NAME));
}

static int
pwm_git_open(pwm_git_t *git, const char *path) {
  int err;
  PWM_STR_INIT(buf);
  struct stat info;

  if (path == NULL) {
    if ((err = pwm_git_find_home(&buf)) < 0) {
      fputs("pwm_git_open: could not find pwm home dir\n", stderr);
      goto cleanup;
    }
    path = buf.buf;
  }

  if ((err = stat(path, &info)) < 0) {
    if (errno != ENOENT) {
      perror("pwm_git_new: stat");
      goto cleanup;
    }

    if ((err = git_repository_init(&git->repo, path, 1)) < 0) {
      fprintf(stderr, "pwm_git_open: %s\n", giterr_last()->message);
      goto cleanup;
    }

    if ((err = chmod(path, 0700)) < 0) {
      perror("pwm_git_open: chmod");
    }
    goto cleanup;
  }

  if ((info.st_mode&(S_IRWXG|S_IRWXO)) != 0) {
    fprintf(stderr, "pwm_git_open: insecure permissions on: %s\n", path);
    err = -1;
    goto cleanup;
  }

  if ((err = git_repository_open(&git->repo, path)) < 0) {
    fprintf(stderr, "pwm_git_open: %s\n", giterr_last()->message);
  }

cleanup:
  pwm_str_free(&buf);
  return err;
}

int
pwm_git_new(pwm_git_t **out, const char *path) {
  pwm_git_t *git;
  int empty;

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

  if ((empty = git_repository_is_empty(git->repo)) < 0) {
    fprintf(stderr, "pwm_git_new: %s\n", giterr_last()->message);
    goto fail;
  }

  if (empty) {
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
pwm_git_clean(pwm_git_t *git) {
  int err;
  git_oid id;
  git_object *obj = NULL;
  git_reset_t reset_type = GIT_RESET_HARD;
  git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;

  if ((err = git_reference_name_to_id(&id, git->repo, "HEAD")) < 0) {
    goto cleanup;
  }

  if ((err = git_object_lookup(&obj, git->repo, &id, GIT_OBJ_COMMIT)) < 0) {
    goto cleanup;
  }

  if (git_repository_is_bare(git->repo)) {
    reset_type = GIT_RESET_SOFT;
  }

  if ((err = git_reset(git->repo, obj, reset_type, git->sig, NULL))  < 0) {
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

static int
pwm_git_head(pwm_git_t *git, git_commit **commit) {
  return git_revparse_single((git_object **) commit, git->repo, "HEAD");
}

static int
pwm_git_tree(pwm_git_t *git, const char *name, git_tree **tree) {
  const git_tree_entry *entry;

  if (git->tree == NULL) {
    *tree = NULL;
    return 0;
  }

  if ((entry = git_tree_entry_byname(git->tree, name)) == NULL) {
    *tree = NULL;
    return 0;
  }
  return git_tree_lookup(tree, git->repo, git_tree_entry_id(entry));
}

static int
pwm_git_tree_add(pwm_git_t *git, const char *name, git_oid *id) {
  git_treebuilder *bld = NULL;
  int err;

  if ((err = git_treebuilder_create(&bld, git->tree)) < 0) {
    goto cleanup;
  }

  if ((err = git_treebuilder_insert(NULL, bld, name, id, 0040000)) < 0) {
    goto cleanup;
  }

  if ((err = git_treebuilder_write(id, git->repo, bld)) < 0) {
    goto cleanup;
  }

  if ((err = git_tree_lookup(&git->tree, git->repo, id)) < 0) {
    goto cleanup;
  }
  git->dirty = 1;

cleanup:
  git_treebuilder_free(bld);
  return err;
}

int
pwm_git_get(pwm_git_t *git, const char *dir, const char *key, pwm_str_t *s) {
  const git_tree_entry *entry;
  git_tree *tree = NULL;
  git_blob *blob = NULL;
  int err;

  if ((err = pwm_git_tree(git, dir, &tree)) < 0) {
    fprintf(stderr, "pwm_git_get: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if (tree == NULL || (entry = git_tree_entry_byname(tree, key)) == NULL) {
    fprintf(stderr, "pwm_git_get: entry not found: %s:%s\n", dir, key);
    err = -1;
    goto cleanup;
  }

  if ((err = git_blob_lookup(&blob, git->repo, git_tree_entry_id(entry))) < 0) {
    fprintf(stderr, "pwm_git_get: %s\n", giterr_last()->message);
    goto cleanup;
  }
  err = pwm_str_set(s, git_blob_rawcontent(blob), git_blob_rawsize(blob));

cleanup:
  git_blob_free(blob);
  git_tree_free(tree);
  return err;
}

int
pwm_git_has(pwm_git_t *git, const char *dir, const char *key) {
  git_tree *tree = NULL;
  int rc;

  if ((rc = pwm_git_tree(git, dir, &tree)) < 0) {
    fprintf(stderr, "pwm_git_has: %s\n", giterr_last()->message);
    goto cleanup;
  }
  rc = (tree == NULL) ? 0 : (git_tree_entry_byname(tree, key) != NULL);

cleanup:
  git_tree_free(tree);
  return rc;
}

int
pwm_git_add(pwm_git_t *git, const char *dir, const char *key, const pwm_str_t *s) {
  git_treebuilder *bld = NULL;
  git_tree *tree = NULL;
  git_oid id;
  int err;

  if ((err = git_blob_create_frombuffer(&id, git->repo, s->buf, s->len)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = pwm_git_tree(git, dir, &tree)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_create(&bld, tree)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_insert(NULL, bld, key, &id, 0100644)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_write(&id, git->repo, bld)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = pwm_git_tree_add(git, dir, &id)) < 0) {
    fprintf(stderr, "pwm_git_add: %s\n", giterr_last()->message);
    goto cleanup;
  }

cleanup:
  git_treebuilder_free(bld);
  return err;
}

int
pwm_git_rm(pwm_git_t *git, const char *dir, const char *key) {
  git_treebuilder *bld = NULL;
  git_tree *tree = NULL;
  git_oid id;
  int err;

  if ((err = pwm_git_tree(git, dir, &tree)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_create(&bld, tree)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_remove(bld, key)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = git_treebuilder_write(&id, git->repo, bld)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if ((err = pwm_git_tree_add(git, dir, &id)) < 0) {
    fprintf(stderr, "pwm_git_rm: %s\n", giterr_last()->message);
    goto cleanup;
  }

cleanup:
  git_treebuilder_free(bld);
  return err;
}

int
pwm_git_commit(pwm_git_t *git, const char *msg) {
  git_commit *head = NULL;
  git_oid id;
  size_t n;
  int err;

  if (!git->dirty) {
    fprintf(stderr, "pwm_git_commit: nothing to commit\n");
    return -1;
  }

  if ((err = pwm_git_head(git, &head)) < 0 && err != GIT_ENOTFOUND) {
    fprintf(stderr, "pwm_git_commit: %s\n", giterr_last()->message);
    goto cleanup;
  }
  n = (err == GIT_ENOTFOUND) ? 0 : 1;

  err = git_commit_create_v(&id, git->repo, "HEAD", git->sig, git->sig,
                            NULL, msg, git->tree, n, head);

  if (err < 0) {
    fprintf(stderr, "pwm_git_commit: %s\n", giterr_last()->message);
  } else {
    git->dirty = 0;
  }

cleanup:
  git_commit_free(head);
  return err;
}

int
pwm_git_walk_entries(pwm_git_t *git, const char *dir, pwm_git_walk_entries_cb cb) {
  const git_tree_entry *entry;
  git_tree *tree = NULL;
  size_t i;
  int err;

  if ((err = pwm_git_tree(git, dir, &tree)) < 0) {
    fprintf(stderr, "pwm_git_walk_entries: %s\n", giterr_last()->message);
    goto cleanup;
  }

  if (tree == NULL) {
    goto cleanup;
  }

  for (i = 0; (entry = git_tree_entry_byindex(tree, i)) != NULL; i++) {
    if ((err = cb(git_tree_entry_name(entry))) < 0) {
      goto cleanup;
    }
  }

cleanup:
  git_tree_free(tree);
  return err;
}

int
pwm_git_walk_log(pwm_git_t *git, pwm_git_walk_log_cb cb) {
  git_commit *commit, *parent;
  const char *msg, *name;
  int64_t time;
  int err;

  if ((err = pwm_git_head(git, &commit)) < 0) {
    if (err == GIT_ENOTFOUND) {
      return 0;
    }
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
