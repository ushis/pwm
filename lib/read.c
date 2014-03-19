#include "read.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int
pwm_read_file(pwm_str_t *s, const char *path) {
  int fd, err;

  if ((fd = open(path, O_RDONLY)) < 0) {
    perror("pwm_read_file: open");
    return -1;
  }
  err = pwm_str_read_all(s, fd);
  close(fd);
  return err;
}

int
pwm_read_line(pwm_str_t *s, const char *prompt) {
  fputs(prompt, stdout);
  return pwm_str_read_line(s, stdin);
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
  new_attr.c_lflag &= ~(ICANON|ECHO);

  if (tcsetattr(0, TCSAFLUSH, &new_attr) < 0) {
    perror("pwm_read_line: tcsetattr");
    return -1;
  }
  err = pwm_read_line(s, prompt);
  tcsetattr(0, TCSAFLUSH, &old_attr);
  return err;
}
