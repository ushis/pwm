#include "gen.h"
#include "hex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int
pwm_gen_rand(pwm_str_t *s, size_t n) {
  ssize_t m;
  int fd;

  if (s->cap < n && pwm_str_resize(s, n) < 0) {
    return -1;
  }

  if ((fd = open("/dev/urandom", O_RDONLY)) < 0) {
    perror("pwm_gen_rand: open");
    return -1;
  }
  m = read(fd, s->buf, n);
  close(fd);

  if (m < n) {
    fprintf(stderr, "pwm_gen_rand: urandom is broken\n");
    return -1;
  }
  return pwm_str_set_len(s, n);
}

static const char *alnum =
  "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

int
pwm_gen_alnum(pwm_str_t *s, size_t n) {
  size_t i;

  if (pwm_gen_rand(s, n) < 0) {
    return -1;
  }

  for (i = 0; i < n; i++) {
    s->buf[i] = alnum[abs(s->buf[i])%62];
  }
  return 0;
}

int
pwm_gen_ascii(pwm_str_t *s, size_t n) {
  size_t i;

  if (pwm_gen_rand(s, n) < 0) {
    return -1;
  }

  for (i = 0; i < n; i++) {
    s->buf[i] = (abs(s->buf[i])%('~'-'!'+1))+'!';
  }
  return 0;
}

int
pwm_gen_hex(pwm_str_t *s, size_t n) {
  int err;
  PWM_STR_INIT(buf);
  size_t len = pwm_hex_decode_len(n+1);

  if ((err = pwm_gen_rand(&buf, len)) < 0) {
    goto cleanup;
  }

  if ((err = pwm_str_hex_encode(s, buf.buf, len)) < 0) {
    goto cleanup;
  }
  pwm_str_shrink(s, n);

cleanup:
  pwm_str_free(&buf);
  return err;
}

int
pwm_gen_num(pwm_str_t *s, size_t n) {
  size_t i;

  if (pwm_gen_rand(s, n) < 0) {
    return -1;
  }

  for (i = 0; i < n; i++) {
    s->buf[i] = (abs(s->buf[i])%10)+'0';
  }
  return 0;
}

static const pwm_gen_t gens[] = {
  {"alnum", "random alphanumeric characters",    pwm_gen_alnum},
  {"ascii", "random printable ascii characters", pwm_gen_ascii},
  {"hex",   "random hex",                        pwm_gen_hex},
  {"num",   "random numeric characters",         pwm_gen_num}
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
