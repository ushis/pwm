#include "test.h"
#include <stdlib.h>

int
main() {
  int failed;
  SRunner *sr = srunner_create(NULL);
  srunner_add_suite(sr, pwm_hex_suite());
  srunner_add_suite(sr, pwm_gen_suite());
  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
