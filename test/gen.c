#include "gen.h"
#include "str.h"
#include <check.h>
#include <ctype.h>

START_TEST(test_pwm_gen_rand) {
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  ck_assert_int_eq(pwm_gen_rand(&a, 4<<10), 0);
  ck_assert_int_eq(pwm_gen_rand(&b, 4<<10), 0);
  ck_assert_int_eq(a.len, 4<<10);
  ck_assert_int_eq(b.len, 4<<10);
  ck_assert_int_ne(pwm_str_cmp(&a, &b), 0);
  pwm_str_free(&b);
  pwm_str_free(&a);
}
END_TEST

START_TEST(test_pwm_gen_alnum) {
  size_t i;
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_gen_alnum(&buf, 4<<10), 0);
  ck_assert_int_eq(buf.len, 4<<10);

  for (i = 0; i < 4<<10; i++) {
    ck_assert(isalnum(buf.buf[i]));
  }
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_gen_ascii) {
  size_t i;
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_gen_ascii(&buf, 4<<10), 0);
  ck_assert_int_eq(buf.len, 4<<10);

  for (i = 0; i < 4<<10; i++) {
    ck_assert(isprint(buf.buf[i]));
  }
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_gen_hex) {
  size_t i;
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_gen_hex(&buf, 4<<10), 0);
  ck_assert_int_eq(buf.len, 4<<10);

  for (i = 0; i < 4<<10; i++) {
    char c = buf.buf[i];
    ck_assert(('0' <= c && c <= '9') || ('a' <= c && c <= 'f'));
  }
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_gen_num) {
  size_t i;
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_gen_num(&buf, 4<<10), 0);
  ck_assert_int_eq(buf.len, 4<<10);

  for (i = 0; i < 4<<10; i++) {
    ck_assert_int_ge(buf.buf[i], '0');
    ck_assert_int_le(buf.buf[i], '9');
  }
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_gen_by_name) {
  ck_assert_ptr_eq(pwm_gen_by_name("alnum")->func, pwm_gen_alnum);
  ck_assert_ptr_eq(pwm_gen_by_name("ascii")->func, pwm_gen_ascii);
  ck_assert_ptr_eq(pwm_gen_by_name("hex")->func, pwm_gen_hex);
  ck_assert_ptr_eq(pwm_gen_by_name("num")->func, pwm_gen_num);
  ck_assert(pwm_gen_by_name("crap") == NULL);
}
END_TEST

TCase *
pwm_gen_tests() {
  TCase *tc = tcase_create("gen");
  tcase_add_test(tc, test_pwm_gen_rand);
  tcase_add_test(tc, test_pwm_gen_alnum);
  tcase_add_test(tc, test_pwm_gen_ascii);
  tcase_add_test(tc, test_pwm_gen_hex);
  tcase_add_test(tc, test_pwm_gen_num);
  tcase_add_test(tc, test_pwm_gen_by_name);
  return tc;
}
