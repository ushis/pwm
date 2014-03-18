#include <check.h>
#include <stdlib.h>

Suite *pwm_gen_suite();
Suite *pwm_git_suite();
Suite *pwm_gpg_suite();
Suite *pwm_hex_suite();
Suite *pwm_str_suite();

int
main() {
  int failed;
  SRunner *sr = srunner_create(NULL);
  srunner_add_suite(sr, pwm_gen_suite());
  srunner_add_suite(sr, pwm_git_suite());
  srunner_add_suite(sr, pwm_gpg_suite());
  srunner_add_suite(sr, pwm_hex_suite());
  srunner_add_suite(sr, pwm_str_suite());
  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
