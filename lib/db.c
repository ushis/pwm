#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
pwm_db_new(pwm_db_t **out, const char *home, const char *email) {
  pwm_db_t *db;

  if ((db = (pwm_db_t *) calloc(1, sizeof(pwm_db_t))) == NULL) {
    perror("pwm_db_new: calloc");
    goto fail;
  }

  if (pwm_git_new(&db->git, home) < 0) {
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
  PWM_STR_INIT(src);
  int err;

  if ((err = pwm_git_get(db->git, key, &src)) < 0) {
    goto cleanup;
  }
  err = pwm_gpg_decrypt(db->gpg, dst, &src);

cleanup:
  pwm_str_free(&src);
  return err;
}

int
pwm_db_has(pwm_db_t *db, const char *key) {
  return pwm_git_has(db->git, key);
}

int
pwm_db_set(pwm_db_t *db, const char *key, const char *msg, const pwm_str_t *src) {
  PWM_STR_INIT(dst);
  char buf[strlen(key)+3];
  int err;

  if ((err = pwm_gpg_encrypt(db->gpg, &dst, src)) < 0) {
    goto cleanup;
  }

  if ((err = pwm_git_add(db->git, key, &dst)) < 0) {
    goto cleanup;
  }

  if (msg == NULL) {
    sprintf(buf, "+ %s", key);
    msg = buf;
  }
  err = pwm_git_commit(db->git, msg);

cleanup:
  pwm_str_free(&dst);
  return err;
}

int
pwm_db_del(pwm_db_t *db, const char *key, const char *msg) {
  char buf[strlen(key)+3];

  if (pwm_git_rm(db->git, key) < 0) {
    return -1;
  }

  if (msg == NULL) {
    sprintf(buf, "- %s", key);
    msg = buf;
  }

  if (pwm_git_commit(db->git, msg) < 0) {
    return -1;
  }
  return 0;
}

int
pwm_db_note_get(pwm_db_t *db, const char *key, pwm_str_t *dst) {
  PWM_STR_INIT(src);
  int err;

  if ((err = pwm_git_note_get(db->git, key, &src)) < 0) {
    goto cleanup;
  }
  err = pwm_gpg_decrypt(db->gpg, dst, &src);

cleanup:
  pwm_str_free(&src);
  return err;
}

int
pwm_db_note_set(pwm_db_t *db, const char *key, const pwm_str_t *src) {
  PWM_STR_INIT(dst);
  int err;

  if ((err = pwm_gpg_encrypt_armor(db->gpg, &dst, src)) < 0) {
    goto cleanup;
  }
  err = pwm_git_note_set(db->git, key, &dst);

cleanup:
  pwm_str_free(&dst);
  return err;
}

int
pwm_db_note_del(pwm_db_t *db, const char *key) {
  return pwm_git_note_rm(db->git, key);
}

int
pwm_db_list(pwm_db_t *db, pwm_git_walk_entries_cb cb) {
  return pwm_git_walk_entries(db->git, cb);
}

int
pwm_db_log(pwm_db_t *db, pwm_git_walk_log_cb cb) {
  return pwm_git_walk_log(db->git, cb);
}
