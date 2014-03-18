#include <check.h>
#include <stdlib.h>

TCase *pwm_gen_tests();
TCase *pwm_git_tests();
TCase *pwm_gpg_tests();
TCase *pwm_hex_tests();
TCase *pwm_str_tests();

int
main() {
  int failed;
  Suite *s = suite_create("pwm");
  SRunner *sr = srunner_create(s);

  suite_add_tcase(s, pwm_gen_tests());
  suite_add_tcase(s, pwm_git_tests());
  suite_add_tcase(s, pwm_gpg_tests());
  suite_add_tcase(s, pwm_hex_tests());
  suite_add_tcase(s, pwm_str_tests());

  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
