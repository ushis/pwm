#ifndef GPG_H
#define GPG_H 1

#include <gpgme.h>
#include "str.h"

/** GPG handler */
typedef struct {
  gpgme_ctx_t ctx;
  gpgme_key_t keys[2];
} pwm_gpg_t;

/** Initializes GPG. Do not call this directly - use pwm_init(). */
int pwm_gpg_init();

/**
 * Allocates a new GPG handler.
 *
 * @param  out    GPG pointer to store the allocated handler in
 * @param  email  Email used to determine the GPG recipient
 * @return 0 on success or an error code
 */
int pwm_gpg_new(pwm_gpg_t **out, const char *email);

/**
 * Frees a GPG handler.
 *
 * @param  gpg  The GPG handler
 */
void pwm_gpg_free(pwm_gpg_t *gpg);

/**
 * Decrypts a string.
 *
 * @param  gpg  The GPG handler
 * @param  dst  The string pointer to store the result in
 * @param  src  The string to decrypt
 * @return 0 on success or an error code
 */
int pwm_gpg_decrypt(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src);

/**
 * Encrypts a string.
 *
 * @param  gpg  The GPG handler
 * @param  dst  The string pointer to store the result in
 * @param  src  The string to encrypt
 * @return 0 on success or an error code
 */
int pwm_gpg_encrypt(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src);

/**
 * Encrypts a string ASCII armored.
 *
 * @param  gpg  The GPG handler
 * @param  dst  The string pointer to store the result in
 * @param  src  The string to encrypt
 * @return 0 on success or an error code
 */
int pwm_gpg_encrypt_armor(pwm_gpg_t *gpg, pwm_str_t *dst, const pwm_str_t *src);

#endif /* GPG_H */
