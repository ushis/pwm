#include "git.h"
#include <check.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

static void
setup(pwm_git_t **git) {
  pwm_git_init();
  ck_assert_int_eq(system("rm -rf test/git"), 0);
  ck_assert_int_eq(pwm_git_new(git, "test/git"), 0);
}

static void
read_file(pwm_str_t *s, const char *path) {
  int fd;
  ck_assert_int_gt((fd = open(path, O_RDONLY)), 0);
  ck_assert_int_eq(pwm_str_read_all(s, fd), 0);
  ck_assert_int_eq(close(fd), 0);
}

START_TEST(test_pwm_git_get) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);
  ck_assert_int_lt(pwm_git_get(git, "nonexistent", &b), 0);
  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_git_free(git);
}
END_TEST

START_TEST(test_pwm_git_has) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 0);
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 1);
  pwm_str_free(&a);
  pwm_git_free(git);
}
END_TEST

START_TEST(test_pwm_git_add) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  PWM_STR_INIT(c);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");
  read_file(&b, "test/data/slipsum.txt.gpg");
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_add(git, "slipsum2", &a), 0);
  ck_assert_int_eq(pwm_git_add(git, "slipsum2", &b), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum slipsum2"), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum2", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&b, &c), 0);
  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_str_free(&c);
  pwm_git_free(git);
}
END_TEST

START_TEST(test_pwm_git_rm) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_eq(pwm_git_rm(git, "slipsum"), 0);
  ck_assert_int_eq(pwm_git_commit(git, "- slipsum"), 0);
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 0);
  ck_assert_int_lt(pwm_git_rm(git, "slipsum"), 0);
  pwm_str_free(&a);
  pwm_git_free(git);
}
END_TEST;

START_TEST(test_pwm_git_commit) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  PWM_STR_INIT(c);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");
  read_file(&b, "test/data/slipsum.txt.gpg");
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  pwm_git_free(git);
  ck_assert_int_eq(pwm_git_new(&git, "test/git"), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);
  ck_assert_int_eq(pwm_git_add(git, "slipsum2", &b), 0);
  ck_assert_int_eq(pwm_git_rm(git, "slipsum"), 0);
  ck_assert_int_eq(pwm_git_commit(git, "- slipsum + slipsum2"), 0);
  pwm_git_free(git);
  ck_assert_int_eq(pwm_git_new(&git, "test/git"), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum2", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&b, &c), 0);
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 0);
  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_str_free(&c);
  pwm_git_free(git);
}
END_TEST

Suite *
pwm_git_suite() {
  Suite *s = suite_create("pwm_git");
  TCase *tc_core = tcase_create("core");
  tcase_add_test(tc_core, test_pwm_git_get);
  tcase_add_test(tc_core, test_pwm_git_has);
  tcase_add_test(tc_core, test_pwm_git_add);
  tcase_add_test(tc_core, test_pwm_git_rm);
  tcase_add_test(tc_core, test_pwm_git_commit);
  /*
  tcase_add_test(tc_core, test_pwm_git_note_get);
  tcase_add_test(tc_core, test_pwm_git_note_set);
  tcase_add_test(tc_core, test_pwm_git_note_rm);
  tcase_add_test(tc_core, test_pwm_git_walk_entries);
  tcase_add_test(tc_core, test_pwm_git_walk_log);
  */
  suite_add_tcase(s, tc_core);
  return s;
}
