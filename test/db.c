#include "db.h"
#include "read.h"
#include "str.h"
#include <check.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

static void
setup(pwm_db_t **db) {
  pwm_git_init();
  pwm_gpg_init();
  ck_assert_int_eq(system("rm -rf test/git"), 0);
  ck_assert_int_eq(setenv("GNUPGHOME", "test/gnupg", 1), 0);
  ck_assert_int_eq(pwm_db_new(db, "test/git", "A2D17C4B"), 0);
}

START_TEST(test_pwm_db_get) {
  pwm_db_t *db;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  setup(&db);

  /* non existent entry */
  ck_assert_int_lt(pwm_db_get(db, "sample", &b), 0);

  /* existing entry */
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_eq(pwm_db_get(db, "sample", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);

  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_db_free(db);
}
END_TEST

START_TEST(test_pwm_db_has) {
  pwm_db_t *db;
  PWM_STR_INIT(a);
  setup(&db);

  /* non existent entry */
  ck_assert_int_eq(pwm_db_has(db, "sample"), 0);

  /* existing entry */
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_eq(pwm_db_has(db, "sample"), 1);

  /* removed entry */
  ck_assert_int_eq(pwm_db_del(db, "sample", NULL), 0);
  ck_assert_int_eq(pwm_db_has(db, "sample"), 0);

  pwm_str_free(&a);
  pwm_db_free(db);
}
END_TEST

START_TEST(test_pwm_db_set) {
  pwm_db_t *db;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  PWM_STR_INIT(c);
  setup(&db);

  /* compare setted and getted */
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_eq(pwm_db_get(db, "sample", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);

  /* is it encrypted before stored? */
  ck_assert_int_eq(pwm_git_get(db->git, "sample", &b), 0);
  ck_assert_int_ne(pwm_str_cmp(&a, &b), 0);
  ck_assert_int_eq(pwm_gpg_decrypt(db->gpg, &c, &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);

  /* override entry */
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt.gpg"), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_eq(pwm_db_get(db, "sample", &c), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);

  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_str_free(&c);
  pwm_db_free(db);
}
END_TEST

START_TEST(test_pwm_db_del) {
  pwm_db_t *db;
  PWM_STR_INIT(a);
  setup(&db);

  /* nonexistent entry */
  ck_assert_int_lt(pwm_db_del(db, "sample", NULL), 0);

  /* existing entry */
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_eq(pwm_db_del(db, "sample", NULL), 0);
  ck_assert_int_eq(pwm_db_has(db, "sample"), 0);

  pwm_str_free(&a);
  pwm_db_free(db);
}
END_TEST

START_TEST(test_pwm_db_note_get) {
  pwm_db_t *db;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  setup(&db);

  /* nonexistent entry */
  ck_assert_int_lt(pwm_db_note_get(db, "sample", &a), 0);

  /* nonexistent note */
  ck_assert_int_eq(pwm_str_set(&a, "sample", 6), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_lt(pwm_db_note_get(db, "sample", &a), 0);

  /* existing note */
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_db_note_set(db, "sample", &a), 0);
  ck_assert_int_eq(pwm_db_note_get(db, "sample", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);

  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_db_free(db);
}
END_TEST

START_TEST(test_pwm_db_note_set) {
  pwm_db_t *db;
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  PWM_STR_INIT(c);
  setup(&db);

  /* compare setted and getted */
  ck_assert_int_eq(pwm_str_set(&a, "sample", 6), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_db_note_set(db, "sample", &a), 0);
  ck_assert_int_eq(pwm_db_note_get(db, "sample", &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);

  /* note encrypted before stored? */
  ck_assert_int_eq(pwm_git_note_get(db->git, "sample", &b), 0);
  ck_assert_int_ne(pwm_str_cmp(&a, &b), 0);
  ck_assert_int_eq(pwm_gpg_decrypt(db->gpg, &c, &b), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &c), 0);

  pwm_str_free(&a);
  pwm_str_free(&b);
  pwm_str_free(&c);
  pwm_db_free(db);
}
END_TEST

START_TEST(test_pwm_db_note_del) {
  pwm_db_t *db;
  PWM_STR_INIT(a);
  setup(&db);

  /* nonexistent entry */
  ck_assert_int_lt(pwm_db_note_del(db, "sample"), 0);

  /* nonexistent note */
  ck_assert_int_eq(pwm_str_set(&a, "sample", 6), 0);
  ck_assert_int_eq(pwm_db_set(db, "sample", NULL, &a), 0);
  ck_assert_int_lt(pwm_db_note_del(db, "sample"), 0);

  /* existing note */
  ck_assert_int_eq(pwm_read_file(&a, "test/data/slipsum.txt"), 0);
  ck_assert_int_eq(pwm_db_note_set(db, "sample", &a), 0);
  ck_assert_int_eq(pwm_db_note_del(db, "sample"), 0);

  /* removed note */
  ck_assert_int_lt(pwm_db_note_del(db, "sample"), 0);

  pwm_str_free(&a);
  pwm_db_free(db);
}
END_TEST

TCase *
pwm_db_tests() {
  TCase *tc = tcase_create("db");
  tcase_add_test(tc, test_pwm_db_get);
  tcase_add_test(tc, test_pwm_db_has);
  tcase_add_test(tc, test_pwm_db_set);
  tcase_add_test(tc, test_pwm_db_del);
  tcase_add_test(tc, test_pwm_db_note_get);
  tcase_add_test(tc, test_pwm_db_note_set);
  tcase_add_test(tc, test_pwm_db_note_del);
  return tc;
}
