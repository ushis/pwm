#include "pwm.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

static const char *usage_str =
  "pwm note set [<opts>] <key>\n\n"
  "options:\n"
  "  -f              override existing note\n"
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
} opts_t;

#define OPTS_DEFAULT {NULL,0}

static int
run(opts_t *opts, const char *key) {
  pwm_db_t *db = NULL;
  PWM_STR_INIT(buf);
  int err;

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    goto cleanup;
  }

  if (!pwm_db_has(db, "passwd", key)) {
    fprintf(stderr, "couldn't find your %s password\n", key);
    err = -1;
    goto cleanup;
  }

  if (!opts->force && pwm_db_has(db, "notes", key)) {
    fprintf(stderr, "note for %s already exists\n", key);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_str_read_all(&buf, STDIN_FILENO)) < 0) {
    goto cleanup;
  }
  err = pwm_db_set(db, "notes", key, opts->msg, &buf);

cleanup:
  pwm_db_clean(db);
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err;
}

int
main(int argc, char **argv) {
  opts_t opts = OPTS_DEFAULT;
  char c, *key = NULL;
  int err;

  while ((c = getopt(argc, argv, "-:fhm:")) > -1) {
    switch (c) {
    case 1:
      key = optarg;
      break;
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

  if (key == NULL) {
    usage();
  }
  pwm_init();
  err = run(&opts, key);
  pwm_shutdown();
  exit(err < 0);
}
