#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "gen.h"

int
pwm_gen_rand(char *buf, size_t n) {
  ssize_t m;
  int fd;

  if ((fd = open("/dev/urandom", O_RDONLY)) < 0) {
    perror("pwm_gen_rand: open");
    return -1;
  }
  m = read(fd, buf, n);
  close(fd);

  if (m < n) {
    fprintf(stderr, "pwm_gen_rand: urandom is broken\n");
    return -1;
  }
  return 0;
}

const char *alnum = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int
pwm_gen_alnum(pwm_str_t *s, size_t n) {
  size_t i;
  char buf[n];

  if (pwm_gen_rand(buf, n) < 0) {
    return -1;
  }

  for (i = 0; i < n; i++) {
    buf[i] = alnum[abs(buf[i])%62];
  }
  return pwm_str_set(s, buf, n);
}

int
pwm_gen_ascii(pwm_str_t *s, size_t n) {
  size_t i;
  char buf[n];

  if (pwm_gen_rand(buf, n) < 0) {
    return -1;
  }

  for (i = 0; i < n; i++) {
    buf[i] = (abs(buf[i])%('~'+1-'!'))+'!';
  }
  return pwm_str_set(s, buf, n);
}

int
pwm_gen_hex(pwm_str_t *s, size_t n) {
  size_t len = pwm_hex_decode_len(n+1);
  char buf[len];

  if (pwm_gen_rand(buf, len) < 0) {
    return -1;
  }

  if (pwm_str_hex_encode(s, buf, len) < 0) {
    return -1;
  }
  pwm_str_shrink(s, n);
  return 0;
}

const pwm_gen_t gens[] = {
  {"alnum", "random alphanumeric characters",    pwm_gen_alnum},
  {"ascii", "random printable ascii characters", pwm_gen_ascii},
  {"hex",   "random hex",                        pwm_gen_hex}
};

const pwm_gen_t *
pwm_gen_by_index(size_t i) {
  return (i < (sizeof(gens)/sizeof(pwm_gen_t))) ? &gens[i] : NULL;
}

const pwm_gen_t *
pwm_gen_by_name(const char *name) {
  const pwm_gen_t *gen;
  size_t i = 0;

  while ((gen = pwm_gen_by_index(i++)) != NULL) {
    if (strcmp(name, gen->name) == 0) {
      return gen;
    }
  }
  return NULL;
}
