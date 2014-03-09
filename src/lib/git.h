#ifndef GIT_H
#define GIT_H 1

#include <git2.h>
#include "str.h"

/* */
typedef struct {
  git_repository *repo;
  git_signature *sig;
  git_tree *tree;
} pwm_git_t;

/* */
pwm_git_t *pwm_git_new(const char *path);

/* */
void pwm_git_free(pwm_git_t *git);

/* */
int pwm_git_add(pwm_git_t *git, const char *path, const pwm_str_t *s);

/* */
int pwm_git_rm(pwm_git_t *git, const char *path);

/* */
int pwm_git_commit(pwm_git_t *git, const char *msg);

/* */
int pwm_git_clean(pwm_git_t *git);

/* */
int pwm_git_entry_content(pwm_git_t *git, const char *path, pwm_str_t *dst);

/* */
int pwm_git_entry_exists(pwm_git_t *git, const char *path);

/* */
typedef int (*pwm_git_walk_entries_cb)(const char *name);

/* */
int pwm_git_walk_entries(pwm_git_t *git, pwm_git_walk_entries_cb cb);

/* */
typedef int (*pwm_git_walk_log_cb)(int64_t time, const char *name, const char *msg);

/* */
int pwm_git_walk_log(pwm_git_t *git, pwm_git_walk_log_cb cd);

#endif /* GIT_H */
