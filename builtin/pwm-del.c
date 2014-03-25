#include "pwm.h"
#include <stdio.h>
#include <getopt.h>

static const char *usage_str =
  "pwm del [<opts>] <key>\n\n"
  "options:\n"
  "  -f              ignore nonexistent passwords\n"
  "  -h              show this help\n"
  "  -m <msg>        use a custom log message";

static void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

typedef struct {
  char *msg;
  int force;
} opt_t;

#define OPTS_DEFAULT {NULL,0}

static int
run (opt_t *opts, const char *key) {
  pwm_db_t *db = NULL;
  int err;

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    goto cleanup;
  }

  if (!pwm_db_has(db, "passwd", key)) {
    if (opts->force) {
      fprintf(stderr, "deleted your %s password\n", key);
    } else {
      fprintf(stderr, "couldn't find your %s password\n", key);
      err = -1;
    }
    goto cleanup;
  }

  if ((err = pwm_db_del(db, "passwd", key, opts->msg)) < 0) {
    goto cleanup;
  }
  fprintf(stderr, "deleted your %s password\n", key);

  if (!pwm_db_has(db, "notes", key)) {
    goto cleanup;
  }

  if ((err = pwm_db_del(db, "notes", key, opts->msg)) < 0) {
    goto cleanup;
  }
  fprintf(stderr, "deleted your %s note\n", key);

cleanup:
  pwm_db_clean(db);
  pwm_db_free(db);
  return err;
}

int
main(int argc, char **argv) {
  char c;
  int err;
  opt_t opts = OPTS_DEFAULT;

  while ((c = getopt(argc, argv, "fhm:")) > -1) {
    switch (c) {
    case 'f':
      opts.force = 1;
      break;
    case 'm':
      opts.msg = optarg;
      break;
    default:
      usage();
    }
  }

  if (optind >= argc) {
    usage();
  }
  pwm_init();
  err = run(&opts, argv[optind]);
  pwm_shutdown();
  exit(err < 0);
}
