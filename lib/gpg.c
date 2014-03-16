#include "gpg.h"
#include "def.h"
#include <stdio.h>
#include <stdlib.h>

int
pwm_gpg_init() {
  return (gpgme_check_version(NULL) == NULL) ? -1 : 0;
}

int
pwm_gpg_new(pwm_gpg_t **out, const char *key_id) {
  gpgme_error_t err;
  pwm_gpg_t *gpg;

  if ((gpg = (pwm_gpg_t *) calloc(1, sizeof(pwm_gpg_t))) == NULL) {
    perror("pwm_gpg_new: calloc");
    goto fail;
  }

  if ((err = gpgme_new(&gpg->ctx)) > 0) {
    fprintf(stderr, "pwm_db_init_gpg: %s\n", gpgme_strerror(err));
    goto fail;
  }

  if (key_id == NULL) {
    key_id = getenv(PWM_KEY_ID_ENV_VAR);
  }

  if ((err = gpgme_op_keylist_start(gpg->ctx, key_id, 1)) > 0) {
    fprintf(stderr, "pwm_db_init_gpg: %s\n", gpgme_strerror(err));
    goto fail;
  }

  if ((err = gpgme_op_keylist_next(gpg->ctx, &gpg->keys[0])) > 0) {
    fprintf(stderr, "pwm_db_init_gpg: %s\n", gpgme_strerror(err));
    goto fail;
  }
  gpg->keys[1] = NULL;
  *out = gpg;
  return 0;

fail:
  pwm_gpg_free(gpg);
  return -1;
}

void
pwm_gpg_free(pwm_gpg_t *gpg) {
  if (gpg != NULL) {
    gpgme_key_release(gpg->keys[0]);
    gpgme_release(gpg->ctx);
    free(gpg);
  }
}

int
pwm_gpg_decrypt(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src) {
  gpgme_data_t cipher = NULL, plain = NULL;
  gpgme_error_t err;
  size_t len;
  char *buf = NULL;
  int rc = -1;

  if ((err = gpgme_data_new_from_mem(&cipher, src->buf, src->len, 0)) > 0) {
    fprintf(stderr, "pwm_gpg_decrypt: %s\n", gpgme_strerror(err));
    goto cleanup;
  }

  if ((err = gpgme_data_new(&plain)) > 0) {
    fprintf(stderr, "pwm_gpg_decrypt: %s\n", gpgme_strerror(err));
    goto cleanup;
  }

  if ((err = gpgme_op_decrypt_verify(gpg->ctx, cipher, plain)) > 0) {
    fprintf(stderr, "pwm_gpg_decrypt: %s\n", gpgme_strerror(err));
    gpgme_data_release(plain);
    goto cleanup;
  }

  if ((buf = gpgme_data_release_and_get_mem(plain, &len)) == NULL) {
    perror("pwm_gpg_decrypt");
    goto cleanup;
  }
  rc = pwm_str_set(dst, buf, len);

cleanup:
  gpgme_data_release(cipher);
  gpgme_free(buf);
  return rc;
}

int
pwm_gpg_encrypt(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src) {
  gpgme_data_t cipher = NULL, plain = NULL;
  gpgme_error_t err;
  size_t len;
  char *buf = NULL;
  int rc = -1;

  if ((err = gpgme_data_new_from_mem(&plain, src->buf, src->len, 0)) > 0) {
    fprintf(stderr, "pwm_gpg_encrypt: %s\n", gpgme_strerror(err));
    goto cleanup;
  }

  if ((err = gpgme_data_new(&cipher)) > 0) {
    fprintf(stderr, "pwm_gpg_encrypt: %s\n", gpgme_strerror(err));
    goto cleanup;
  }

  if ((err = gpgme_op_encrypt_sign(gpg->ctx, gpg->keys, 0, plain, cipher)) > 0) {
    fprintf(stderr, "pwm_gpg_encrypt: %s\n", gpgme_strerror(err));
    gpgme_data_release(cipher);
    goto cleanup;
  }

  if ((buf = gpgme_data_release_and_get_mem(cipher, &len)) == NULL) {
    perror("pwm_gpg_encrypt");
    goto cleanup;
  }
  rc = pwm_str_set(dst, buf, len);

cleanup:
  gpgme_data_release(plain);
  gpgme_free(buf);
  return rc;
}

int
pwm_gpg_encrypt_armor(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src) {
  int err;
  gpgme_set_armor(gpg->ctx, 1);
  err = pwm_gpg_encrypt(gpg, dst, src);
  gpgme_set_armor(gpg->ctx, 0);
  return err;
}
