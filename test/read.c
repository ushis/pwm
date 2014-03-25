#include "read.h"
#include <check.h>

static const char *sample =
"Now that we know who you are, I know who I am. I'm not a mistake! It all makes\n"
"sense! In a comic, you know how you can tell who the arch-villain's going to\n"
"be? He's the exact opposite of the hero. And most times they're friends, like\n"
"you and me! I should've known way back when... You know why, David? Because of\n"
"the kids. They called me Mr Glass.\n"
"\n"
"Well, the way they make shows is, they make one show. That show's called a\n"
"pilot. Then they show that show to the people who make shows, and on the\n"
"strength of that one show they decide if they're going to make more shows.\n"
"Some pilots get picked and become television programs. Some don't, become\n"
"nothing. She starred in one of the ones that became nothing.\n"
"\n"
"Now that there is the Tec-9, a crappy spray gun from South Miami. This gun is\n"
"advertised as the most popular gun in American crime. Do you believe that shit?\n"
"It actually says that in the little book that comes with it: the most popular\n"
"gun in American crime. Like they're actually proud of that shit.\n";

START_TEST(test_pwm_read_file) {
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  ck_assert_int_eq(pwm_str_set(&a, sample, strlen(sample)), 0);
  ck_assert_int_eq(pwm_read_file(&b, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);
  pwm_str_free(&b);
  pwm_str_free(&a);
}
END_TEST

TCase *
pwm_read_tests() {
  TCase *tc = tcase_create("read");
  tcase_add_test(tc, test_pwm_read_file);
  return tc;
}
