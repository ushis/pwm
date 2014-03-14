#ifndef GIT_H
#define GIT_H 1

#include "str.h"
#include <git2.h>

/** Git handler */
typedef struct {
  git_repository *repo;
  git_signature *sig;
  git_tree *tree;
} pwm_git_t;

/** Initializes git. Do not call this directly - use pwm_init(). */
int pwm_git_init();

/** Shuts down git */
void pwm_git_shutdown();

/**
 * Allocates a new git handler.
 *
 * @param  out   Pointer to store the allocated handler in
 * @param  path  Path to the git repo
 * @return 0 on success or an error code
 */
int pwm_git_new(pwm_git_t **out, const char *path);

/**
 * Frees a git handler.
 *
 * @param  git  The git handler
 */
void pwm_git_free(pwm_git_t *git);

/**
 * Cleans up a git repo. Call this after changing the git repo to cleanup
 * after failed operations.
 *
 * @param  git  The git handler
 */
int pwm_git_clean(pwm_git_t *git);

/**
 * Retrieves a blob from the git tree.
 *
 * @param  git   The git handler
 * @param  path  Path to blob
 * @param  s     String pointer to store blobs contents in
 * @return 0 on success or an error code
 */
int pwm_git_get(pwm_git_t *git, const char *path, pwm_str_t *s);

/**
 * Checks if blob exists in the git tree.
 *
 * @param  git   The git handler
 * @param  path  Path to the blob
 * @return 0 on success or an error code
 */
int pwm_git_has(pwm_git_t *git, const char *path);

/**
 * Adds a blob to the git tree.
 *
 * @param  git   The git handler
 * @param  path  The path to the blob
 * @param  s     String pointer with blobs contents
 * @return 0 on success or an error code
 */
int pwm_git_add(pwm_git_t *git, const char *path, const pwm_str_t *s);

/**
 * Removes a blob from the git tree.
 *
 * @param  git   The git handler
 * @param  path  Path to the blob
 * @return 0 on success or an error code
 */
int pwm_git_rm(pwm_git_t *git, const char *path);

/**
 * Commits all changes in the git tree.
 *
 * @param  git  The git handler
 * @param  msg  A commit message
 * @return 0 on success or an error code
 */
int pwm_git_commit(pwm_git_t *git, const char *msg);

/**
 * Retrieves a blob note.
 *
 * @param  git   The git handler
 * @param  path  Path to the blob
 * @param  s     String pointer to store the notes contents in
 * @return 0 on success or an error code
 */
int pwm_git_note_get(pwm_git_t *git, const char *path, pwm_str_t *s);

/**
 * Sets a blob note.
 *
 * @param  git   The git handler
 * @param  path  Path to the blob
 * @param  s     String pointer with the notes contents
 * @return 0 on success or an error code
 */
int pwm_git_note_set(pwm_git_t *git, const char *path, const pwm_str_t *s);

/**
 * Removes a blob note.
 *
 * @param  git   The git handler
 * @param  path  Path to the blob
 * @return 0 on success or an error code
 */
int pwm_git_note_rm(pwm_git_t *git, const char *path);

/** Callback function used by pwm_git_walk_entries() */
typedef int (*pwm_git_walk_entries_cb)(const char *name);

/**
 * Walks all entries in the git tree.
 *
 * @param  git  The git handler
 * @param  cb   Function to call on each tree entry
 * @return 0 on success or an error code
 */
int pwm_git_walk_entries(pwm_git_t *git, pwm_git_walk_entries_cb cb);

/** Callback function used by pwm_git_walk_log() */
typedef int (*pwm_git_walk_log_cb)(int64_t time, const char *name, const char *msg);

/**
 * Walks all log entries.
 *
 * @param  git  The git handler
 * @param  cb   Function to call on each log entry
 * @return 0 on success or an error code
 */
int pwm_git_walk_log(pwm_git_t *git, pwm_git_walk_log_cb cd);

#endif /* GIT_H */
