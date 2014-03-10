#ifndef GPG_H
#define GPG_H 1

#include <gpgme.h>
#include "str.h"

/* gpg struct */
typedef struct {
  gpgme_ctx_t ctx;
  gpgme_key_t keys[2];
} pwm_gpg_t;

/* allocates a new gpg handler */
int pwm_gpg_new(pwm_gpg_t **out, const char *email);

/* frees the gpg handler */
void pwm_gpg_free(pwm_gpg_t *gpg);

/* decrypts the string */
int pwm_gpg_decrypt(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src);

/* encrypts a string */
int pwm_gpg_encrypt(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src);

#endif /* GPG_H */
