#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
pwm_db_new(pwm_db_t **out, const char *home, const char *key_id) {
  pwm_db_t *db;

  if ((db = (pwm_db_t *) calloc(1, sizeof(pwm_db_t))) == NULL) {
    perror("pwm_db_new: calloc");
    goto fail;
  }

  if (pwm_git_new(&db->git, home) < 0) {
    goto fail;
  }

  if (pwm_gpg_new(&db->gpg, key_id) < 0) {
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
  return (db == NULL) ? 0 : pwm_git_clean(db->git);
}

int
pwm_db_get(pwm_db_t *db, const char *dir, const char *key, pwm_str_t *dst) {
  PWM_STR_INIT(src);
  int err;

  if ((err = pwm_git_get(db->git, dir, key, &src)) < 0) {
    goto cleanup;
  }
  err = pwm_gpg_decrypt(db->gpg, dst, &src);

cleanup:
  pwm_str_free(&src);
  return err;
}

int
pwm_db_has(pwm_db_t *db, const char *dir, const char *key) {
  return pwm_git_has(db->git, dir, key);
}

int
pwm_db_set(pwm_db_t *db, const char *dir, const char *key, const char *msg, const pwm_str_t *src) {
  PWM_STR_INIT(dst);
  PWM_STR_INIT(buf);
  int err;

  if ((err = pwm_gpg_encrypt(db->gpg, &dst, src)) < 0) {
    goto cleanup;
  }

  if ((err = pwm_git_add(db->git, dir, key, &dst)) < 0) {
    goto cleanup;
  }

  if (msg == NULL) {
    if ((err = pwm_str_fmt(&buf, "+ %s:%s", dir, key)) < 0) {
      goto cleanup;
    }
    msg = buf.buf;
  }
  err = pwm_git_commit(db->git, msg);

cleanup:
  pwm_str_free(&buf);
  pwm_str_free(&dst);
  return err;
}

int
pwm_db_del(pwm_db_t *db, const char *dir, const char *key, const char *msg) {
  PWM_STR_INIT(buf);
  int err;

  if ((err = pwm_git_rm(db->git, dir, key)) < 0) {
    goto cleanup;
  }

  if (msg == NULL) {
    if ((err = pwm_str_fmt(&buf, "- %s:%s", dir, key)) < 0) {
      goto cleanup;
    }
    msg = buf.buf;
  }
  err = pwm_git_commit(db->git, msg);

cleanup:
  pwm_str_free(&buf);
  return err;
}

int
pwm_db_list(pwm_db_t *db, const char *dir, pwm_git_walk_entries_cb cb) {
  return pwm_git_walk_entries(db->git, dir, cb);
}

int
pwm_db_log(pwm_db_t *db, pwm_git_walk_log_cb cb) {
  return pwm_git_walk_log(db->git, cb);
}
