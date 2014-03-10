#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "pwm.h"

typedef struct {
  char *generator;
  ssize_t len;
  int force;
  int print;
  int clip;
} opts_t;

#define OPTS_DEFAULT {"alnum",32,0,0,0}

void
usage(const char *argv0) {
  size_t i = 0;
  const pwm_gen_t *gen;

  fprintf(stderr,
    "usage: %s [<opts>] <key>\n\n"
    "options:\n"
    "  -c              store password in the clipboard (not implemented)\n"
    "  -f              override existing password\n"
    "  -g <generator>  generator to use (default: alnum)\n"
    "  -h              show this help\n"
    "  -l <len>        password length (default: 32)\n"
    "  -p              print generated password\n\n"
    "generators:\n", argv0);

  while ((gen = pwm_gen_by_index(i++)) != NULL) {
    fprintf(stderr, "  %-14s  %s\n", gen->name, gen->desc);
  }
  exit(EXIT_FAILURE);
}

int
parse_opts(opts_t *opts, int argc, char **argv) {
  char c;

  while ((c = getopt(argc, argv, "cfg:l:p")) >= 0) {
    switch (c) {
      case 'c':
        opts->clip = 1;
        break;
      case 'f':
        opts->force = 1;
        break;
      case 'g':
        opts->generator = optarg;
        break;
      case 'l':
        opts->len = atol(optarg);
        break;
      case 'p':
        opts->print = 1;
        break;
      default:
        usage(argv[0]);
    }
  }
  return optind;
}

int
main(int argc, char **argv) {
  int i, err;
  char *key;
  opts_t opts = OPTS_DEFAULT;
  pwm_db_t *db = NULL;
  pwm_str_t buf = PWM_STR_INIT;
  const pwm_gen_t *gen;

  pwm_init();

  if ((i = parse_opts(&opts, argc, argv)) >= argc) {
    usage(argv[0]);
  }
  key = argv[i];

  if (opts.len < 1 || opts.len > 4<<10) {
    fprintf(stderr, "invalid password length: %ld\n", opts.len);
    exit(EXIT_FAILURE);
  }

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }

  if (!opts.force && pwm_db_has(db, key)) {
    fprintf(stderr, "password for %s already exists\n", key);
    err = -1;
    goto cleanup;
  }

  if ((gen = pwm_gen_by_name(opts.generator)) == NULL) {
    fprintf(stderr, "unknown generator: %s\n", opts.generator);
    err = -1;
    goto cleanup;
  }

  if ((err = gen->func(&buf, opts.len)) < 0) {
    goto cleanup;
  }
  err = pwm_db_set(db, key, &buf);
  pwm_db_clean(db);

  if (err < 0) {
    goto cleanup;
  }

  if (opts.print) {
    fprintf(stderr, "%s\n", buf.buf);
  } else {
    fprintf(stderr, "generated your %s password\n", key);
  }

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  exit(err < 0);
}
