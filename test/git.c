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

  /* empty tree */
  ck_assert_int_lt(pwm_git_get(git, "slipsum", &b), 0);

  /* existing entry */
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);

  /* nonexistent entry */
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

  /* empty tree */
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 0);

  /* uncommited entry */
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 1);

  /* commited entry */
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 1);

  /* nonexistent entry */
  ck_assert_int_eq(pwm_git_has(git, "nonexistent"), 0);

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

  /* new entries */
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_add(git, "slipsum2", &a), 0);

  /* override uncommited entry */
  ck_assert_int_eq(pwm_git_add(git, "slipsum2", &b), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum slipsum2"), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum2", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&b, &c), 0);

  /* override commited entry */
  ck_assert_int_eq(pwm_git_add(git, "slipsum2", &a), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum2", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);

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

  /* empty tree */
  ck_assert_int_lt(pwm_git_rm(git, "slipsum"), 0);

  /* remove uncommited entry */
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_rm(git, "slipsum"), 0);

  /* remove commited entry */
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_eq(pwm_git_rm(git, "slipsum"), 0);
  ck_assert_int_eq(pwm_git_commit(git, "- slipsum"), 0);
  ck_assert_int_eq(pwm_git_has(git, "slipsum"), 0);

  /* remove nonexistent entry */
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

  /* commit without any changes */
  ck_assert_int_lt(pwm_git_commit(git, "nothing"), 0);

  /* commit added entry */
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  pwm_git_free(git);
  ck_assert_int_eq(pwm_git_new(&git, "test/git"), 0);
  ck_assert_int_eq(pwm_git_get(git, "slipsum", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);

  /* commit multiple changes */
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

START_TEST(test_pwm_git_note_get) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  setup(&git);

  /* empty tree */
  ck_assert_int_lt(pwm_git_note_get(git, "sample", &b), 0);

  /* nonexistent note */
  ck_assert_int_eq(pwm_str_set(&a, "sample", 6), 0);
  ck_assert_int_eq(pwm_git_add(git, "sample", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ sample"), 0);
  read_file(&a, "test/data/slipsum.txt");
  ck_assert_int_lt(pwm_git_note_get(git, "sample", &b), 0);

  /* existing note */
  ck_assert_int_eq(pwm_git_note_set(git, "sample", &a), 0);
  ck_assert_int_eq(pwm_git_note_get(git, "sample", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);

  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_git_free(git);
}
END_TEST

START_TEST(test_pwm_git_note_set) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");

  /* empty tree */
  ck_assert_int_lt(pwm_git_note_set(git, "sample", &a), 0);

  /* existing entry */
  ck_assert_int_eq(pwm_str_set(&b, "sample", 6), 0);
  ck_assert_int_eq(pwm_git_add(git, "sample", &b), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ sample"), 0);
  ck_assert_int_eq(pwm_git_note_set(git, "sample", &a), 0);
  ck_assert_int_eq(pwm_git_note_get(git, "sample", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);

  /* override note */
  ck_assert_int_eq(pwm_str_set(&a, "sample", 6), 0);
  ck_assert_int_eq(pwm_git_note_set(git, "sample", &a), 0);
  ck_assert_int_eq(pwm_git_note_get(git, "sample", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);

  /* nonexistent entry */
  ck_assert_int_lt(pwm_git_note_set(git, "nonexistent", &a), 0);

  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_git_free(git);
}
END_TEST

START_TEST(test_pwm_git_note_rm) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");

  /* empty tree */
  ck_assert_int_lt(pwm_git_note_rm(git, "sample"), 0);

  /* nonexistent note */
  ck_assert_int_eq(pwm_git_add(git, "sample", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ sample"), 0);
  ck_assert_int_lt(pwm_git_note_rm(git, "sample"), 0);

  /* existing note */
  ck_assert_int_eq(pwm_git_note_set(git, "sample", &a), 0);
  ck_assert_int_eq(pwm_git_note_rm(git, "sample"), 0);

  pwm_str_free(&a);
  pwm_git_free(git);
}
END_TEST

static int entry_count = 0;

static int
entry_count_incr(const char *path) {
  return entry_count++;
}

START_TEST(test_pwm_git_walk_entries) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");

  /* empty tree */
  ck_assert_int_eq(pwm_git_walk_entries(git, entry_count_incr), 0);
  ck_assert_int_eq(entry_count, 0);

  /* tree with uncommited entries */
  ck_assert_int_eq(pwm_git_add(git, "sample1", &a), 0);
  ck_assert_int_eq(pwm_git_add(git, "sample2", &a), 0);
  ck_assert_int_eq(pwm_git_add(git, "sample3", &a), 0);
  ck_assert_int_eq(pwm_git_walk_entries(git, entry_count_incr), 0);
  ck_assert_int_eq(entry_count, 3);

  /* tree with commited entries */
  entry_count = 0;
  ck_assert_int_eq(pwm_git_commit(git, "+ sample1 sample2 sample3"), 0);
  ck_assert_int_eq(pwm_git_walk_entries(git, entry_count_incr), 0);
  ck_assert_int_eq(entry_count, 3);

  pwm_str_free(&a);
  pwm_git_free(git);
}
END_TEST

static int log_count = 0;

static int
log_count_incr(int64_t time, const char *name, const char *msg) {
  return log_count++;
}

START_TEST(test_pwm_git_walk_log) {
  pwm_git_t *git;
  PWM_STR_INIT(a);
  setup(&git);
  read_file(&a, "test/data/slipsum.txt");

  /* empty repo */
  ck_assert_int_ge(pwm_git_walk_log(git, log_count_incr), 0);
  ck_assert_int_eq(log_count, 0);

  /* repo with commits */
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_eq(pwm_git_rm(git, "slipsum"), 0);
  ck_assert_int_eq(pwm_git_commit(git, "- slipsum"), 0);
  ck_assert_int_eq(pwm_git_add(git, "slipsum", &a), 0);
  ck_assert_int_eq(pwm_git_commit(git, "+ slipsum"), 0);
  ck_assert_int_ge(pwm_git_walk_log(git, log_count_incr), 0);
  ck_assert_int_eq(log_count, 3);

  pwm_str_free(&a);
  pwm_git_free(git);
}
END_TEST

TCase *
pwm_git_tests() {
  TCase *tc = tcase_create("git");
  tcase_add_test(tc, test_pwm_git_get);
  tcase_add_test(tc, test_pwm_git_has);
  tcase_add_test(tc, test_pwm_git_add);
  tcase_add_test(tc, test_pwm_git_rm);
  tcase_add_test(tc, test_pwm_git_commit);
  tcase_add_test(tc, test_pwm_git_note_get);
  tcase_add_test(tc, test_pwm_git_note_set);
  tcase_add_test(tc, test_pwm_git_note_rm);
  tcase_add_test(tc, test_pwm_git_walk_entries);
  tcase_add_test(tc, test_pwm_git_walk_log);
  return tc;
}
