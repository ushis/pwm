#include "pwm.h"
#include <stdio.h>
#include <getopt.h>

static const char *usage_str =
  "pwm get [<opts>] <key>\n\n"
  "options:\n"
  "  -c              store the password in the clipboard\n"
  "  -h              show this help";

static void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

typedef struct {
  int clip;
} opts_t;

#define OPTS_DEFAULT {0}

static int
run(opts_t *opts, const char *key) {
  int err;
  pwm_db_t *db = NULL;
  PWM_STR_INIT(buf);

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    goto cleanup;
  }

  if (!pwm_db_has(db, "passwd", key)) {
    fprintf(stderr, "couldn't find your %s password\n", key);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_get(db, "passwd", key, &buf)) < 0) {
    goto cleanup;
  }

  if (!opts->clip) {
    fprintf(stdout, "%s\n", buf.buf);
    goto cleanup;
  }

  if ((err = pwm_clip_set(&buf)) >= 0) {
    fprintf(stderr, "stored you %s password in the clipboard\n", key);
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

  while ((c = getopt(argc, argv, "-:ch")) > -1) {
    switch (c) {
    case 1:
      key = optarg;
      break;
    case 'c':
      opts.clip = 1;
      break;
    default:
      usage();
    }
  }

  if (key == NULL) {
    usage();
  }
  pwm_init();
  err = run(&opts, key);
  pwm_shutdown();
  exit(err < 0);
}
