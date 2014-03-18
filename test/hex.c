#include "hex.h"
#include <check.h>
#include <string.h>

static const char raw[] = {0xac,0xf0,0xdd,0x11,0x13,0x33,0xe3,0x00,0x0f,0xff};

static const char *hex = "acf0dd111333e3000fff";

START_TEST(test_pwm_hex_encode_len) {
  ck_assert_int_eq(pwm_hex_encode_len(10), 20);
}
END_TEST

START_TEST(test_pwm_hex_decode_len) {
  ck_assert_int_eq(pwm_hex_decode_len(20), 10);
}
END_TEST

START_TEST(test_pwm_hex_encode) {
  char buf[20];
  ck_assert_int_eq(pwm_hex_encode(buf, raw, 10), 20);
  ck_assert_int_eq(memcmp(buf, hex, 20), 0);
}
END_TEST

START_TEST(test_pwm_hex_decode) {
  char buf[10];
  ck_assert_int_eq(pwm_hex_decode(buf, hex, 20), 10);
  ck_assert_int_eq(memcmp(buf, raw, 10), 0);
}
END_TEST

TCase *
pwm_hex_tests() {
  TCase *tc = tcase_create("hex");
  tcase_add_test(tc, test_pwm_hex_encode_len);
  tcase_add_test(tc, test_pwm_hex_decode_len);
  tcase_add_test(tc, test_pwm_hex_encode);
  tcase_add_test(tc, test_pwm_hex_decode);
  return tc;
}
