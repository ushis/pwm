#ifndef DB_H
#define DB_H 1

#include "str.h"
#include "git.h"
#include "gpg.h"

/** Representation of a pwm database. */
typedef struct {
  pwm_git_t *git;
  pwm_gpg_t *gpg;
} pwm_db_t;

/**
 * Allocates a new database.
 *
 * @param  out     Pointer to store the allocated database in
 * @param  home    Optional path to a pwm database
 * @param  key_id  Optional key id used to determine the GPG recipient
 * @return 0 on success or an error code
 */
int pwm_db_new(pwm_db_t **out, const char *home, const char *key_id);

/**
 * Frees a database.
 *
 * @param  db  Database pointer
 */
void pwm_db_free(pwm_db_t *db);

/**
 * Cleans up a database. Call this after changing the database to cleanup
 * after failed operations.
 *
 * @param  db  Database pointer
 * @return 0 on success or an error code
 */
int pwm_db_clean(pwm_db_t *db);

/**
 * Retrieves a database entry.
 *
 * @param  db   Database pointer
 * @param  key  Key of the database entry
 * @param  dst  String pointer to store the result in
 * @return 0 on success or an error code
 */
int pwm_db_get(pwm_db_t *db, const char *key, pwm_str_t *dst);

/**
 * Checks the existence of a database entry.
 *
 * @param  db   Database pointer
 * @param  key  Key of the database entry
 * @return 1 if the entry exists else 0
 */
int pwm_db_has(pwm_db_t *db, const char *key);

/**
 * Sets a database entry.
 *
 * @param  db   Database pointer
 * @param  key  Key of the database entry
 * @param  msg  Optional log message
 * @param  src  String pointer containing the value to store
 * @return 0 on success or an error code
 */
int pwm_db_set(pwm_db_t *db, const char *key, const char *msg, const pwm_str_t *src);

/**
 * Deletes a database entry.
 *
 * @param  db   Database pointer
 * @param  key  Key of the database entry
 * @param  msg  Optional log message
 * @return 0 on success or an error code
 */
int pwm_db_del(pwm_db_t *db, const char *key, const char *msg);

/**
 * Retrieves an entry note.
 *
 * @param  db   Database pointer
 * @param  key  Key of the database entry
 * @param  dst  String pointer to store the result in
 * @return 0 on success or an error code
 */
int pwm_db_note_get(pwm_db_t *db, const char *key, pwm_str_t *dst);

/**
 * Sets an entry note.
 *
 * @param  db   Database pointer
 * @param  key  Key of the database entry
 * @param  src  String pointer containing the note
 * @return 0 on success or an error code
 */
int pwm_db_note_set(pwm_db_t *db, const char *key, const pwm_str_t *src);

/**
 * Deletes an entry note.
 *
 * @param  db   Database pointer
 * @param  key  Key of the database entry
 * @return 0 on success or an error code
 */
int pwm_db_note_del(pwm_db_t *db, const char *key);

/**
 * Lists all keys in the database.
 *
 * @param  db  Database pointer
 * @param  cb  Function to call on each database entry
 * @return 0 on success or an error code
 */
int pwm_db_list(pwm_db_t *db, pwm_git_walk_entries_cb cb);

/**
 * Lists all log entries.
 *
 * @param  db  Database pointer
 * @param  cb  Function to call on each log entry
 * @return 0 on success or an error code
 */
int pwm_db_log(pwm_db_t *db, pwm_git_walk_log_cb cb);

#endif /* DB_H */
