#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "hex.h"
#include "util.h"

pwm_str_t *
pwm_str_new(size_t cap) {
  pwm_str_t *s;

  if ((s = (pwm_str_t *) calloc(1, sizeof(pwm_str_t))) == NULL) {
    perror("pwm_str_new: calloc");
    return NULL;
  }

  if ((s->buf = (char *) calloc(cap+1, sizeof(char))) == NULL) {
    perror("pwm_str_new: calloc");
    free(s);
    return NULL;
  }
  s->len = 0;
  s->cap = cap;
  return s;
}

pwm_str_t *
pwm_str_new_from_buf(const char *buf, size_t n) {
  pwm_str_t *s;

  if ((s = pwm_str_new(n)) == NULL) {
    return NULL;
  }

  if (pwm_str_set(s, buf, n) < 0) {
    free(s);
    return NULL;
  }
  return s;
}

pwm_str_t *
pwm_str_cpy(pwm_str_t *s) {
  return pwm_str_new_from_buf(s->buf, s->len);
}

void
pwm_str_free(pwm_str_t *s) {
  if (s != NULL) {
    free(s->buf);
    free(s);
  }
}

int
pwm_str_resize(pwm_str_t *s, size_t cap) {
  char *buf;

  if ((buf = (char *) realloc(s->buf, cap+1)) == NULL) {
    perror("pwm_str_resize: realloc");
    return -1;
  }
  s->buf = buf;
  s->cap = cap;
  pwm_str_shrink(s, cap);
  return 0;
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

  if (s->buf[s->len-1] != '/') {
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

int
pwm_str_cmp(pwm_str_t *a, pwm_str_t *b) {
  int rc = memcmp(a->buf, b->buf, min(a->len, b->len));
  return (rc == 0) ? (a->len - b->len) : rc;
}

int
pwm_str_read_line(pwm_str_t *s, FILE *stream) {
  char c;
  size_t i = 0;

  while ((c = fgetc(stream)) != EOF) {
    if (c == '\r' || c == '\n') {
      break;
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
pwm_str_encode_hex(pwm_str_t *dst, pwm_str_t *src) {
  size_t len = pwm_hex_encode_len(src->len);

  if (dst->cap < len && pwm_str_resize(dst, len) < 0) {
    return -1;
  }
  pwm_hex_encode(dst->buf, src->buf, src->len);
  dst->buf[len] = '\0';
  dst->len = len;
  return 0;
}

int
pwm_str_decode_hex(pwm_str_t *dst, pwm_str_t *src) {
  size_t len = pwm_hex_decode_len(src->len);

  if (dst->cap < len && pwm_str_resize(dst, len) < 0) {
    return -1;
  }

  if (pwm_hex_decode(dst->buf, src->buf, src->len) < 0) {
    return -1;
  }
  dst->buf[len] = '\0';
  dst->len = len;
  return 0;
}
