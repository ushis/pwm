#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gpg.h"

pwm_gpg_t *
pwm_gpg_new(const char *email) {
  gpgme_error_t err;
  pwm_gpg_t *gpg;

  gpgme_check_version(NULL);

  if ((gpg = (pwm_gpg_t *) calloc(1, sizeof(pwm_gpg_t))) == NULL) {
    perror("pwm_gpg_new: calloc");
    goto fail;
  }

  if ((err = gpgme_new(&gpg->ctx)) > 0) {
    fprintf(stderr, "pwm_db_init_gpg: %s\n", gpgme_strerror(err));
    goto fail;
  }

  if ((err = gpgme_op_keylist_start(gpg->ctx, email, 1)) > 0) {
    fprintf(stderr, "pwm_db_init_gpg: %s\n", gpgme_strerror(err));
    goto fail;
  }

  if ((err = gpgme_op_keylist_next(gpg->ctx, &gpg->keys[0])) > 0) {
    fprintf(stderr, "pwm_db_init_gpg: %s\n", gpgme_strerror(err));
    goto fail;
  }
  gpg->keys[1] = NULL;
  return gpg;

fail:
  pwm_gpg_free(gpg);
  return NULL;
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
pwm_gpg_decrypt(pwm_gpg_t *gpg, pwm_str_t *s) {
  gpgme_data_t cipher = NULL, plain = NULL;
  gpgme_error_t err;
  size_t len;
  char *buf = NULL;
  int rc = -1;

  if ((err = gpgme_data_new_from_mem(&cipher, s->buf, s->len, 1)) > 0) {
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
  rc = pwm_str_set(s, buf, len);

cleanup:
  gpgme_data_release(cipher);
  gpgme_free(buf);
  return rc;
}

int
pwm_gpg_encrypt_to_file(pwm_gpg_t *gpg, const char *filename, pwm_str_t *src) {
  gpgme_data_t cipher = NULL, plain = NULL;
  gpgme_error_t err;
  int fd, rc = -1;

  if ((fd = open(filename, O_WRONLY|O_TRUNC|O_CREAT, 0600)) < 0) {
    perror("pwm_gpg_encrypt_to_file: open");
    goto cleanup;
  }

  if ((err = gpgme_data_new_from_mem(&plain, src->buf, src->len, 0)) > 0) {
    fprintf(stderr, "pwm_gpg_encrypt_to_file: %s\n", gpgme_strerror(err));
    goto cleanup;
  }

  if ((err = gpgme_data_new_from_fd(&cipher, fd)) > 0) {
    fprintf(stderr, "pwm_gpg_encrypt_to_file: %s\n", gpgme_strerror(err));
    goto cleanup;
  }

  if ((err = gpgme_op_encrypt_sign(gpg->ctx, gpg->keys, 0, plain, cipher)) > 0) {
    fprintf(stderr, "pwm_gpg_encrypt_to_file: %s\n", gpgme_strerror(err));
    goto cleanup;
  }
  rc = 0;

cleanup:
  gpgme_data_release(cipher);
  gpgme_data_release(plain);
  close(fd);
  return rc;
}
