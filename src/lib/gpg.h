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
pwm_gpg_t *pwm_gpg_new(const char *email);

/* frees the gpg handler */
void pwm_gpg_free(pwm_gpg_t *gpg);

/* decrypts the string */
int pwm_gpg_decrypt(pwm_gpg_t *gpg, pwm_str_t *s);

/* encrypts a string */
int pwm_gpg_encrypt(pwm_gpg_t *gpg, pwm_str_t *s);

#endif /* GPG_H */
