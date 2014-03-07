#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

pwm_db_t *
pwm_db_new(pwm_str_t *home, const char *email) {
  pwm_db_t *db;

  if ((db = (pwm_db_t *) calloc(1, sizeof(pwm_db_t))) == NULL) {
    perror("pwm_db_new: calloc");
    goto fail;
  }
  db->home = home;

  if ((db->git = pwm_git_new(home->buf)) == NULL) {
    goto fail;
  }

  if ((db->gpg = pwm_gpg_new(email)) == NULL) {
    goto fail;
  }
  return db;

fail:
  pwm_db_free(db);
  return NULL;
}

void
pwm_db_free(pwm_db_t *db) {
  if (db != NULL) {
    pwm_str_free(db->home);
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
  if (pwm_git_entry_content(db->git, dst, key) < 0) {
    return -1;
  }
  return pwm_gpg_decrypt(db->gpg, dst);
}

int
pwm_db_has(pwm_db_t *db, const char *key) {
  return pwm_git_entry_exists(db->git, key);
}

int
pwm_db_set(pwm_db_t *db, const char *key, pwm_str_t *src) {
  int err;
  pwm_str_t *filename;
  char msg[strlen(key)+3];

  if ((filename = pwm_str_cpy(db->home)) == NULL) {
    return -1;
  }

  if ((err = pwm_str_append_path_component(filename, key, strlen(key))) < 0) {
    goto cleanup;
  }

  if ((err = pwm_gpg_encrypt_to_file(db->gpg, filename->buf, src)) < 0) {
    goto cleanup;
  }

  if ((err = pwm_git_add(db->git, key)) < 0) {
    goto cleanup;
  }
  sprintf(msg, "+ %s", key);
  err = pwm_git_commit(db->git, msg);

cleanup:
  pwm_str_free(filename);
  return err;
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
