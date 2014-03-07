#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "pwm.h"

pwm_str_t *
pwm_find_home() {
  pwm_str_t *home;
  const char *buf;

  if ((buf = getenv(PWM_HOME_ENV_VAR)) != NULL) {
    return pwm_str_new_from_buf(buf, strlen(buf));
  }

  if ((buf = getenv("HOME")) == NULL) {
    return NULL;
  }

  if ((home = pwm_str_new_from_buf(buf, strlen(buf))) == NULL) {
    return NULL;
  }

  if (pwm_str_append_path_component(home, PWM_HOME_NAME, strlen(PWM_HOME_NAME)) < 0) {
    pwm_str_free(home);
    return NULL;
  }
  return home;
}

pwm_str_t *
pwm_read_line(const char *prompt) {
  pwm_str_t *s;

  if ((s = pwm_str_new(4<<10)) == NULL) {
    return NULL;
  }
  fprintf(stdout, "%s", prompt);

  if (pwm_str_read_line(s, stdin) < 0) {
    pwm_str_free(s);
    return NULL;
  }
  return s;
}

pwm_str_t *
pwm_read_line_hidden(const char *prompt) {
  struct termios old_attr, new_attr;
  pwm_str_t *s;

  if (tcgetattr(0, &old_attr) < 0) {
    perror("pwm_read_line: tcgetattr");
    return NULL;
  }
  new_attr = old_attr;
  new_attr.c_lflag &= ~ECHO;

  if (tcsetattr(0, TCSAFLUSH, &new_attr) < 0) {
    perror("pwm_read_line: tcgetattr");
    return NULL;
  }
  s = pwm_read_line(prompt);
  fprintf(stdout, "\n");
  tcsetattr(0, TCSAFLUSH, &old_attr);
  return s;
}
