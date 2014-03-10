#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <termios.h>
#include <string.h>
#include "pwm.h"

void
pwm_init() {
  struct rlimit zero = {0, 0};

  /* lock all memory */
  if (mlockall(MCL_FUTURE) < 0) {
    perror("mlockall");
    exit(EXIT_FAILURE);
  }

  /* disable coredumps */
  if (prlimit(0, RLIMIT_CORE, &zero, NULL) < 0) {
    perror("prlimit");
    exit(EXIT_FAILURE);
  }
}

int
pwm_find_home(pwm_str_t *s) {
  const char *buf;

  if ((buf = getenv(PWM_HOME_ENV_VAR)) != NULL) {
    return pwm_str_set(s, buf, strlen(buf));
  }

  if ((buf = getenv("HOME")) == NULL) {
    return -1;
  }

  if (pwm_str_set(s, buf, strlen(buf)) < 0) {
    return -1;
  }

  if (pwm_str_append_path_component(s, PWM_HOME_NAME, strlen(PWM_HOME_NAME)) < 0) {
    return -1;
  }
  return 0;
}

int
pwm_read_line_hidden(pwm_str_t *s, const char *prompt) {
  struct termios old_attr, new_attr;
  int err;

  if (tcgetattr(0, &old_attr) < 0) {
    perror("pwm_read_line: tcgetattr");
    return -1;
  }
  new_attr = old_attr;
  new_attr.c_lflag &= ~ECHO;

  if (tcsetattr(0, TCSAFLUSH, &new_attr) < 0) {
    perror("pwm_read_line: tcgetattr");
    return -1;
  }
  fprintf(stdout, "%s", prompt);
  err = pwm_str_read_line(s, stdin);
  tcsetattr(0, TCSAFLUSH, &old_attr);
  return err;
}
