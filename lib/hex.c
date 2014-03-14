#include "hex.h"
#include <stdio.h>

static const char *hex = "0123456789abcdef";

size_t
pwm_hex_encode_len(size_t len) {
  return len*2;
}

size_t
pwm_hex_decode_len(size_t len) {
  return len/2;
}

size_t
pwm_hex_encode(char *dst, const char *src, size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    *dst++ = hex[(*src>>4)&0xf];
    *dst++ = hex[(*src++)&0xf];
  }
  return 2 * i;
}

signed char
pwm_hex_decode_char(char c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  }

  if ('a' <= c && c <= 'f') {
    return c - 'a' + 10;
  }

  if ('A' <= c && c <= 'F') {
    return c - 'A' + 10;
  }
  return -1;
}

ssize_t
pwm_hex_decode(char *dst, const char *src, size_t len) {
  size_t i;
  signed char a, b;

  if (len % 2 != 0) {
    fprintf(stderr, "pwm_hex_decode: invalid len: %ld\n", len);
    return -1;
  }

  for (i = 0; i < len; i += 2) {
    if ((a = pwm_hex_decode_char(*src++)) < 0) {
      fprintf(stderr, "pwm_hex_decode: invalid byte at: %ld\n", i);
      return -1;
    }

    if ((b = pwm_hex_decode_char(*src++)) < 0) {
      fprintf(stderr, "pwm_hex_decode: invalid byte at: %ld\n", i+1);
      return -1;
    }
    *dst++ = (a<<4)|b;
  }
  return len/2;
}
