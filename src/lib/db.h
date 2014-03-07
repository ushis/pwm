#ifndef DB_H
#define DB_H 1

#include "str.h"
#include "git.h"
#include "gpg.h"

/* db struct */
typedef struct {
  pwm_str_t *home;
  pwm_git_t *git;
  pwm_gpg_t *gpg;
} pwm_db_t;

/* inits a database */
int pwm_db_init(pwm_db_t *db, pwm_str_t *home, const char *email);

/* allocates a new database */
pwm_db_t *pwm_db_new(pwm_str_t *home, const char *email);

/* frees a database */
void pwm_db_free(pwm_db_t *db);

/* cleans up the database */
int pwm_db_clean(pwm_db_t *db);

/* retrieves a database entry and stores it in s */
int pwm_db_get(pwm_db_t *db, const char *key, pwm_str_t *dst);

/* checks the existence of a database entry */
int pwm_db_has(pwm_db_t *db, const char *key);

/* set a database entry s */
int pwm_db_set(pwm_db_t *db, const char *key, pwm_str_t *src);

/* deletes a database entry */
int pwm_db_del(pwm_db_t *db, const char *key);

/* lists all keys in the database */
int pwm_db_list(pwm_db_t *db, pwm_git_walk_entries_cb cb);

#endif /* DB_H */
