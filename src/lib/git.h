#ifndef GIT_H
#define GIT_H 1

#include <git2.h>
#include "str.h"

/* */
typedef struct {
  git_repository *repo;
  git_signature *sig;
  git_index *index;
} pwm_git_t;

/* */
typedef int (*pwm_git_walk_entries_cb)(const char *);

/* */
pwm_git_t *pwm_git_new(const char *path);

/* */
void pwm_git_free(pwm_git_t *git);

/* */
int pwm_git_add(pwm_git_t *git, const char *path);

/* */
int pwm_git_rm(pwm_git_t *git, const char *path);

/* */
int pwm_git_commit(pwm_git_t *git, const char *msg);

/* */
int pwm_git_clean(pwm_git_t *git);

/* */
int pwm_git_entry_content(pwm_git_t *git, pwm_str_t *dst, const char *path);

/* */
int pwm_git_entry_exists(pwm_git_t *git, const char *path);

/* */
int pwm_git_walk_entries(pwm_git_t *git, pwm_git_walk_entries_cb cb);

#endif /* GIT_H */
