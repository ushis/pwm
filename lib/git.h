#ifndef GIT_H
#define GIT_H 1

#include <git2.h>
#include "str.h"

/* git struct */
typedef struct {
  git_repository *repo;
  git_signature *sig;
  git_tree *tree;
} pwm_git_t;

/* inits git */
int pwm_git_init();

/* shutdown git */
void pwm_git_shutdown();

/* allocates a new git handler */
int pwm_git_new(pwm_git_t **out, const char *path);

/* frees the git handler */
void pwm_git_free(pwm_git_t *git);

/* cleans up the git repo */
int pwm_git_clean(pwm_git_t *git);

/* retrieves blob from the git repo */
int pwm_git_get(pwm_git_t *git, const char *path, pwm_str_t *dst);

/* checks if blob in the git repo exists */
int pwm_git_has(pwm_git_t *git, const char *path);

/* adds the blob timo the git tree */
int pwm_git_add(pwm_git_t *git, const char *path, const pwm_str_t *s);

/* removes a blob from the git tree */
int pwm_git_rm(pwm_git_t *git, const char *path);

/* commits all changes in the tree */
int pwm_git_commit(pwm_git_t *git, const char *msg);

/* entry walk callback */
typedef int (*pwm_git_walk_entries_cb)(const char *name);

/* walks all entries in the git repo */
int pwm_git_walk_entries(pwm_git_t *git, pwm_git_walk_entries_cb cb);

/* log walk callback */
typedef int (*pwm_git_walk_log_cb)(int64_t time, const char *name, const char *msg);

/* walks all log entries */
int pwm_git_walk_log(pwm_git_t *git, pwm_git_walk_log_cb cd);

#endif /* GIT_H */
