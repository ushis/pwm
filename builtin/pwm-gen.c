#include "pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

const char *usage_str =
  "pwm gen [<opts>] <key>\n\n"
  "options:\n"
  "  -c              store password in the clipboard\n"
  "  -f              override existing password\n"
  "  -g <generator>  generator to use (default: alnum)\n"
  "  -h              show this help\n"
  "  -l <len>        password length (default: 32)\n"
  "  -p              print generated password";

void
usage() {
  size_t i = 0;
  const pwm_gen_t *gen;

  fprintf(stderr, "usage: %s\n\ngenerators:\n", usage_str);

  while ((gen = pwm_gen_by_index(i++)) != NULL) {
    fprintf(stderr, "  %-14s  %s\n", gen->name, gen->desc);
  }
  exit(EXIT_FAILURE);
}

typedef struct {
  char *generator;
  ssize_t len;
  int force;
  int print;
  int clip;
} opts_t;

#define OPTS_DEFAULT {"alnum",32,0,0,0}

int
run(opts_t *opts, const char *key) {
  int err;
  pwm_db_t *db = NULL;
  PWM_STR_INIT(buf);
  const pwm_gen_t *gen;

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, pwm_find_email())) < 0) {
    goto cleanup;
  }

  if (!opts->force && pwm_db_has(db, key)) {
    fprintf(stderr, "password for %s already exists\n", key);
    err = -1;
    goto cleanup;
  }

  if ((gen = pwm_gen_by_name(opts->generator)) == NULL) {
    fprintf(stderr, "unknown generator: %s\n", opts->generator);
    err = -1;
    goto cleanup;
  }

  if ((err = gen->func(&buf, opts->len)) < 0) {
    goto cleanup;
  }
  err = pwm_db_set(db, key, &buf);
  pwm_db_clean(db);

  if (err < 0) {
    goto cleanup;
  }

  if (!opts->clip) {
    if (opts->print) {
      fprintf(stderr, "%s\n", buf.buf);
    } else {
      fprintf(stderr, "generated your %s password\n", key);
    }
    goto cleanup;
  }

  if ((err = pwm_clip_set(&buf)) >= 0) {
    fprintf(stderr, "stored your generated %s password in the clipboard\n", key);
  } else {
    fprintf(stderr, "generated your %s password\n", key);
  }

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err;
}

int
main(int argc, char **argv) {
  char c;
  int err;
  opts_t opts = OPTS_DEFAULT;

  while ((c = getopt(argc, argv, "cfg:hl:p")) > -1) {
    switch (c) {
    case 'c':
      opts.clip = 1;
      break;
    case 'f':
      opts.force = 1;
      break;
    case 'g':
      opts.generator = optarg;
      break;
    case 'l':
      opts.len = atol(optarg);
      break;
    case 'p':
      opts.print = 1;
      break;
    default:
      usage();
    }
  }

  if (optind >= argc) {
    usage();
  }

  if (opts.len < 1 || opts.len > 4<<10) {
    fprintf(stderr, "invalid password length: %ld\n", opts.len);
    exit(EXIT_FAILURE);
  }
  pwm_init();
  err = run(&opts, argv[optind]);
  pwm_shutdown();
  exit(err < 0);
}
