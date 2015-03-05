#include "gpg.h"
#include "str.h"
#include <check.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

static void
setup(pwm_gpg_t **gpg) {
  pwm_gpg_init();
  ck_assert_int_eq(setenv("GNUPGHOME", "test/gnupg", 1), 0);
  ck_assert_int_eq(pwm_gpg_new(gpg, "51C6592C"), 0);
}

START_TEST(test_pwm_gpg_decrypt) {
  int fd;
  pwm_gpg_t *gpg;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  setup(&gpg);
  ck_assert_int_ge((fd = open("test/data/slipsum.txt.gpg", O_RDONLY)), 0);
  ck_assert_int_eq(pwm_str_read_all(&a, fd), 0);
  ck_assert_int_eq(close(fd), 0);
  ck_assert_int_eq(pwm_gpg_decrypt(gpg, &b, &a), 0);
  ck_assert_int_ge((fd = open("test/data/slipsum.txt", O_RDONLY)), 0);
  ck_assert_int_eq(pwm_str_read_all(&a, fd), 0);
  ck_assert_int_eq(close(fd), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);
  pwm_str_free(&b);
  pwm_str_free(&a);
  pwm_gpg_free(gpg);
}
END_TEST

START_TEST(test_pwm_gpg_encrypt) {
  int fd;
  pwm_gpg_t *gpg;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  PWM_STR_INIT(c);
  setup(&gpg);
  ck_assert_int_ge((fd = open("test/data/slipsum.txt", O_RDONLY)), 0);
  ck_assert_int_eq(pwm_str_read_all(&a, fd), 0);
  ck_assert_int_eq(close(fd), 0);
  ck_assert_int_eq(pwm_gpg_encrypt(gpg, &b, &a), 0);
  ck_assert_int_eq(pwm_gpg_decrypt(gpg, &c, &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);
  pwm_str_free(&c);
  pwm_str_free(&b);
  pwm_str_free(&a);
  pwm_gpg_free(gpg);
}
END_TEST

TCase *
pwm_gpg_tests() {
  TCase *tc = tcase_create("gpg");
  tcase_add_test(tc, test_pwm_gpg_decrypt);
  tcase_add_test(tc, test_pwm_gpg_encrypt);
  return tc;
}
