#include "str.h"
#include "hex.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int
pwm_str_resize(pwm_str_t *s, size_t cap) {
  char *buf;

  if ((buf = (char *) realloc(s->buf, (cap+1)*sizeof(char))) == NULL) {
    perror("pwm_str_resize: realloc");
    return -1;
  }
  s->buf = buf;
  s->cap = cap;
  pwm_str_shrink(s, cap);

  if (mlock(s->buf, (cap+1)*sizeof(char)) < 0) {
    perror("pwm_str_resize: mlock");
    return -1;
  }
  return 0;
}

void
pwm_str_free(pwm_str_t *s) {
  free(s->buf);
  s->buf = NULL;
  s->len = 0;
  s->cap = 0;
}

int
pwm_str_cpy(pwm_str_t *dst, pwm_str_t *src) {
  return pwm_str_set(dst, src->buf, src->len);
}

int
pwm_str_set(pwm_str_t *s, const char *buf, size_t n) {
  if (s->cap < n && pwm_str_resize(s, n) < 0) {
    return -1;
  }
  memcpy(s->buf, buf, n);
  s->buf[n] = '\0';
  s->len = n;
  return 0;
}

int
pwm_str_append(pwm_str_t *s, const char *buf, size_t n) {
  size_t len = s->len + n;

  if (s->cap < len && pwm_str_resize(s, len) < 0) {
    return -1;
  }
  memcpy(&s->buf[s->len], buf, n);
  s->buf[len] = '\0';
  s->len = len;
  return 0;
}

int
pwm_str_append_path_component(pwm_str_t *s, const char *buf, size_t n) {
  size_t len = s->len + n + 1;

  if (s->cap < len && pwm_str_resize(s, len) < 0) {
    return -1;
  }

  if (s->len > 0 && s->buf[s->len-1] != '/') {
    s->buf[s->len] = '/';
    s->len++;
  }
  return pwm_str_append(s, buf, n);
}

void
pwm_str_shrink(pwm_str_t *s, size_t n) {
  if (s->len > n) {
    s->len = n;
    s->buf[s->len] = '\0';
  }
}

size_t
pwm_str_min_len(pwm_str_t *a, pwm_str_t *b) {
  return (a->len < b->len) ? a->len : b->len;
}

int
pwm_str_cmp(pwm_str_t *a, pwm_str_t *b) {
  int rc = memcmp(a->buf, b->buf, pwm_str_min_len(a, b));
  return (rc == 0) ? (a->len - b->len) : rc;
}

int
pwm_str_read_all(pwm_str_t *s, int fd) {
  ssize_t n;
  size_t i = 0;

  for (;;) {
    if (s->cap <= i && pwm_str_resize(s, (2*s->cap)+1) < 0) {
      return -1;
    }

    if ((n = read(fd, &s->buf[i], s->cap-i)) < 0) {
      perror("read");
      return -1;
    }

    if (n == 0) {
      break;
    }
    i += n;
  }
  s->buf[i] = '\0';
  s->len = i;
  return 0;
}

int
pwm_str_read_line(pwm_str_t *s, FILE *stream) {
  char c;
  size_t i = 0;

  while ((c = fgetc(stream)) != EOF && c != '\n') {
    if (c == '\r') {
      continue;
    }

    if (i >= s->cap && pwm_str_resize(s, (2*s->cap)+1) < 0) {
      return -1;
    }
    s->buf[i++] = c;
  }
  s->buf[i] = '\0';
  s->len = i;
  return 0;
}

int
pwm_str_hex_encode(pwm_str_t *dst, const char *buf, size_t n) {
  size_t len = pwm_hex_encode_len(n);

  if (dst->cap < len && pwm_str_resize(dst, len) < 0) {
    return -1;
  }
  pwm_hex_encode(dst->buf, buf, n);
  dst->buf[len] = '\0';
  dst->len = len;
  return 0;
}

int
pwm_str_hex_decode(pwm_str_t *dst, const char *buf, size_t n) {
  size_t len = pwm_hex_decode_len(n);

  if (dst->cap < len && pwm_str_resize(dst, len) < 0) {
    return -1;
  }

  if (pwm_hex_decode(dst->buf, buf, n) < 0) {
    return -1;
  }
  dst->buf[len] = '\0';
  dst->len = len;
  return 0;
}
