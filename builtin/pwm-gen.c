#include "pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

static const char *usage_str =
  "pwm gen [<opts>] <key>\n\n"
  "options:\n"
  "  -c              store password in the clipboard\n"
  "  -f              override existing password\n"
  "  -g <generator>  generator to use (default: alnum)\n"
  "  -h              show this help\n"
  "  -l <len>        password length (default: 32)\n"
  "  -m <msg>        use a custom log message\n"
  "  -p              print generated password";

static void
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
  char *msg;
  ssize_t len;
  int force;
  int print;
  int clip;
} opts_t;

#define OPTS_DEFAULT {"alnum",NULL,32,0,0,0}

static int
run(opts_t *opts, const char *key) {
  int err;
  pwm_db_t *db = NULL;
  PWM_STR_INIT(buf);
  const pwm_gen_t *gen;

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    goto cleanup;
  }

  if (!opts->force && pwm_db_has(db, "passwd", key)) {
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
  err = pwm_db_set(db, "passwd", key, opts->msg, &buf);
  pwm_db_clean(db);

  if (err < 0) {
    goto cleanup;
  }

  if (opts->clip && (err = pwm_clip_set(&buf)) >= 0) {
    fprintf(stderr, "stored your generated %s password in the clipboard\n", key);
  } else if (opts->print) {
    fprintf(stdout, "%s\n", buf.buf);
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
  opts_t opts = OPTS_DEFAULT;
  char c, *key = NULL;
  int err;

  while ((c = getopt(argc, argv, "-:cfg:hl:m:p")) > -1) {
    switch (c) {
    case 1:
      key = optarg;
      break;
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
    case 'm':
      opts.msg = optarg;
      break;
    case 'p':
      opts.print = 1;
      break;
    default:
      usage();
    }
  }

  if (key == NULL) {
    usage();
  }

  if (opts.len < 1 || opts.len > 4<<10) {
    fprintf(stderr, "invalid password length: %ld\n", opts.len);
    exit(EXIT_FAILURE);
  }
  pwm_init();
  err = run(&opts, key);
  pwm_shutdown();
  exit(err < 0);
}
