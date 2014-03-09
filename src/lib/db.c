#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

int
pwm_db_new(pwm_db_t **out, pwm_str_t *home, const char *email) {
  pwm_db_t *db;

  if ((db = (pwm_db_t *) calloc(1, sizeof(pwm_db_t))) == NULL) {
    perror("pwm_db_new: calloc");
    goto fail;
  }
  db->home = home;

  if (pwm_git_new(&db->git, home->buf) < 0) {
    goto fail;
  }

  if (pwm_gpg_new(&db->gpg, email) < 0) {
    goto fail;
  }
  *out = db;
  return 0;

fail:
  pwm_db_free(db);
  return -1;
}

void
pwm_db_free(pwm_db_t *db) {
  if (db != NULL) {
    pwm_git_free(db->git);
    pwm_gpg_free(db->gpg);
    free(db);
  }
}

int
pwm_db_clean(pwm_db_t *db) {
  return pwm_git_clean(db->git);
}

int
pwm_db_get(pwm_db_t *db, const char *key, pwm_str_t *dst) {
  if (pwm_git_get(db->git, key, dst) < 0) {
    return -1;
  }
  return pwm_gpg_decrypt(db->gpg, dst);
}

int
pwm_db_has(pwm_db_t *db, const char *key) {
  return pwm_git_has(db->git, key);
}

int
pwm_db_set(pwm_db_t *db, const char *key, pwm_str_t *src) {
  char msg[strlen(key)+3];

  if (pwm_gpg_encrypt(db->gpg, src) < 0) {
    return -1;
  }

  if (pwm_git_add(db->git, key, src) < 0) {
    return -1;
  }
  sprintf(msg, "+ %s", key);
  return pwm_git_commit(db->git, msg);
}

int
pwm_db_del(pwm_db_t *db, const char *key) {
  char msg[strlen(key)+3];

  if (pwm_git_rm(db->git, key) < 0) {
    return -1;
  }
  sprintf(msg, "- %s", key);

  if (pwm_git_commit(db->git, msg) < 0) {
    return -1;
  }
  return 0;
}

int
pwm_db_list(pwm_db_t *db, pwm_git_walk_entries_cb cb) {
  return pwm_git_walk_entries(db->git, cb);
}

int
pwm_db_log(pwm_db_t *db, pwm_git_walk_log_cb cb) {
  return pwm_git_walk_log(db->git, cb);
}
