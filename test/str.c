#include "str.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

static const char *sample = "Hello, my name is ...test\0 hidden message";

static size_t sample_len = 41;

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
  ck_assert_int_ge(buf.cap, 4<<10);
  ck_assert_int_eq(buf.len, 0);
  buf.len = 2<<10;
  ck_assert_int_eq(pwm_str_resize(&buf, 1<<10), 0);
  ck_assert_int_ge(buf.cap, 1<<10);
  ck_assert_int_eq(buf.len, buf.cap);
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
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_append(&buf, sample, sample_len), 0);
  ck_assert_int_eq(buf.len, sample_len);
  ck_assert_int_eq(pwm_str_append(&buf, sample, sample_len), 0);
  ck_assert_int_eq(buf.len, 2*sample_len);
  ck_assert_int_eq(memcmp(buf.buf, sample, sample_len), 0);
  ck_assert_int_eq(memcmp(&buf.buf[sample_len], sample, sample_len), 0);
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_str_append_path_component) {
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_append_path_component(&buf, "home", 4), 0);
  ck_assert_int_eq(pwm_str_append_path_component(&buf, "john/", 5), 0);
  ck_assert_int_eq(pwm_str_append_path_component(&buf, "doe.txt", 7), 0);
  ck_assert_int_eq(buf.len, 17);
  ck_assert_int_eq(memcmp(buf.buf, "home/john/doe.txt", buf.len), 0);
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_str_shrink) {
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_set(&buf, sample, sample_len), 0);
  pwm_str_shrink(&buf, 16);
  ck_assert_int_eq(buf.len, 16);
  ck_assert_int_eq(strlen(buf.buf), 16);
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_str_cmp) {
  PWM_STR_INIT(a);
  PWM_STR_INIT(b);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);
  ck_assert_int_eq(pwm_str_set(&a, "aabbcc", 6), 0);
  ck_assert_int_eq(pwm_str_set(&b, "aabbcc", 6), 0);
  ck_assert_int_eq(pwm_str_cmp(&a, &b), 0);
  ck_assert_int_eq(pwm_str_append(&b, "dd", 2), 0);
  ck_assert_int_lt(pwm_str_cmp(&a, &b), 0);
  ck_assert_int_gt(pwm_str_cmp(&b, &a), 0);
  ck_assert_int_eq(pwm_str_set(&b, "ccbbaa", 6), 0);
  ck_assert_int_lt(pwm_str_cmp(&a, &b), 0);
  ck_assert_int_gt(pwm_str_cmp(&b, &a), 0);
  pwm_str_free(&a);
  pwm_str_free(&b);
}
END_TEST

START_TEST(test_pwm_str_read_all) {
  pid_t pid;
  int fd[2], rc;
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pipe(fd), 0);
  ck_assert_int_ge((pid = fork()), 0);

  if (pid == 0) {
    close(fd[0]);
    rc = write(fd[1], "Hello World", 11);
    close(fd[1]);
    exit(rc < 0);
  }
  ck_assert_int_eq(close(fd[1]), 0);
  ck_assert_int_eq(pwm_str_read_all(&buf, fd[0]), 0);
  ck_assert_int_eq(close(fd[0]), 0);
  ck_assert_int_ge(waitpid(pid, &rc, 0), 0);
  ck_assert(WIFEXITED(rc) && WEXITSTATUS(rc) == 0);
  ck_assert_str_eq(buf.buf, "Hello World");
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_str_read_line) {
  pid_t pid;
  int fd[2], rc;
  FILE *stream;
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pipe(fd), 0);
  ck_assert_int_ge((pid = fork()), 0);

  if (pid == 0) {
    close(fd[0]);
    rc = write(fd[1], "Hello World\nthis is data from outer space\n", 42);
    close(fd[1]);
    exit(rc < 0);
  }
  ck_assert_int_eq(close(fd[1]), 0);
  ck_assert((stream = fdopen(fd[0], "r")) != NULL);
  ck_assert_int_eq(pwm_str_read_line(&buf, stream), 0);
  ck_assert_int_eq(fclose(stream), 0);
  ck_assert_int_ge(waitpid(pid, &rc, 0), 0);
  ck_assert(WIFEXITED(rc) && WEXITSTATUS(rc) == 0);
  ck_assert_str_eq(buf.buf, "Hello World");
  pwm_str_free(&buf);
}
END_TEST

static const char raw[] = {0xde,0xad,0xbe,0xef,0x00,0xac,0x31,0x22,0x66,0xab};

static const char *hex = "deadbeef00ac312266ab";

START_TEST(test_pwm_str_hex_encode) {
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_hex_encode(&buf, raw, 10), 0);
  ck_assert_str_eq(buf.buf, hex);
  pwm_str_free(&buf);
}
END_TEST

START_TEST(test_pwm_str_hex_decode) {
  PWM_STR_INIT(buf);
  ck_assert_int_eq(pwm_str_hex_decode(&buf, hex, 20), 0);
  ck_assert_int_eq(buf.len, 10);
  ck_assert_int_eq(memcmp(buf.buf, raw, 10), 0);
  ck_assert_int_lt(pwm_str_hex_decode(&buf, hex, 19), 0);
  ck_assert_int_lt(pwm_str_hex_decode(&buf, "ac67dez8a8", 10), 0);
  pwm_str_free(&buf);
}
END_TEST

TCase *
pwm_str_tests() {
  TCase *tc = tcase_create("str");
  tcase_add_test(tc, test_pwm_str_init);
  tcase_add_test(tc, test_pwm_str_resize);
  tcase_add_test(tc, test_pwm_str_cpy);
  tcase_add_test(tc, test_pwm_str_set);
  tcase_add_test(tc, test_pwm_str_append);
  tcase_add_test(tc, test_pwm_str_append_path_component);
  tcase_add_test(tc, test_pwm_str_shrink);
  tcase_add_test(tc, test_pwm_str_cmp);
  tcase_add_test(tc, test_pwm_str_read_all);
  tcase_add_test(tc, test_pwm_str_read_line);
  tcase_add_test(tc, test_pwm_str_hex_encode);
  tcase_add_test(tc, test_pwm_str_hex_decode);
  return tc;
}
