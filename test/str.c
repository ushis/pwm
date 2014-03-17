#include "str.h"
#include <check.h>
#include <string.h>

const char *sample = "Hello, my name is ...test\0 hidden message";

size_t sample_len = 41;

START_TEST(test_pwm_str_init) {
  PWM_STR_INIT(buf);
  ck_assert(buf.buf == NULL);
  ck_assert_int_eq(buf.len, 0);
  ck_assert_int_eq(buf.cap, 0);
}
END_TEST

START_TEST(test_pwm_str_resize) {
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_resize(&buf, 4<<10), 0);
  ck_assert_int_eq(buf.cap, 4<<10);
  ck_assert_int_eq(buf.len, 0);
  buf.len = 2<<10;
  ck_assert_int_eq(pwm_str_resize(&buf, 1<<10), 0);
  ck_assert_int_eq(buf.cap, 1<<10);
  ck_assert_int_eq(buf.len, 1<<10);
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_str_cpy) {
  PWM_STR_INIT(src);
  PWM_STR_INIT(dst);
  ck_assert_int_eq(pwm_str_set(&src, sample, sample_len), 0);
  ck_assert_int_eq(pwm_str_cpy(&dst, &src), 0);
  ck_assert_int_eq(pwm_str_cmp(&src, &dst), 0);
  pwm_str_free(&src);
  pwm_str_free(&dst);
}
END_TEST

START_TEST(test_pwm_str_set) {
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_set(&buf, sample, sample_len), 0);
  ck_assert_int_eq(buf.len, sample_len);
  ck_assert_int_eq(memcmp(buf.buf, sample, buf.len), 0);
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_str_append) {
  size_t i;
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_append(&buf, sample, sample_len), 0);
  ck_assert_int_eq(buf.len, sample_len);
  ck_assert_int_eq(pwm_str_append(&buf, sample, sample_len), 0);
  ck_assert_int_eq(buf.len, 2*sample_len);
  i = buf.len/2;
  ck_assert_int_eq(memcmp(buf.buf, sample, i), 0);
  ck_assert_int_eq(memcmp(&buf.buf[i], sample, i), 0);
  pwm_str_free(&buf);
}
END_TEST

Suite *
pwm_str_suite() {
  Suite *s = suite_create("pwm_str");
  TCase *tc_core = tcase_create("core");
  tcase_add_test(tc_core, test_pwm_str_init);
  tcase_add_test(tc_core, test_pwm_str_resize);
  tcase_add_test(tc_core, test_pwm_str_cpy);
  tcase_add_test(tc_core, test_pwm_str_set);
  tcase_add_test(tc_core, test_pwm_str_append);
  /* TODO more tests please */
  suite_add_tcase(s, tc_core);
  return s;
}
