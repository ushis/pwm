#include "pwm.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

static const char *usage_str =
  "pwm set [<opts>] <key>\n\n"
  "options:\n"
  "  -c              store password in the clipboard\n"
  "  -f              override existing password\n"
  "  -h              show this help\n"
  "  -m <msg>        use a custom log message\n"
  "  -p              print password";

static void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

typedef struct {
  char *msg;
  int clip;
  int force;
  int print;
} opts_t;

#define OPTS_DEFAULT {NULL,0,0,0}

static int
run(opts_t *opts, const char *key) {
  int err;
  pwm_db_t *db = NULL;
  PWM_STR_INIT(buf);

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    goto cleanup;
  }

  if (!opts->force && pwm_db_has(db, key)) {
    fprintf(stderr, "password for %s already exists\n", key);
    err = -1;
    goto cleanup;
  }

  if (!isatty(STDIN_FILENO)) {
    err = pwm_str_read_line(&buf, stdin);
  } else if (opts->print) {
    err = pwm_read_line(&buf, "password: ");
  } else {
    err = pwm_read_line_hidden(&buf, "password: ");
  }

  if (err < 0) {
    goto cleanup;
  }

  if ((err = pwm_db_set(db, key, opts->msg, &buf)) >= 0) {
    fprintf(stderr, "\rsaved your %s password\n", key);
  }
  pwm_db_clean(db);

  if (opts->clip && (err = pwm_clip_set(&buf)) >= 0) {
    fprintf(stderr, "stored your %s password in the clipboard\n", key);
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

  while ((c = getopt(argc, argv, "cfhm:p")) > -1) {
    switch (c) {
    case 'c':
      opts.clip = 1;
      break;
    case 'f':
      opts.force = 1;
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

  if (optind >= argc) {
    usage();
  }
  pwm_init();
  err = run(&opts, argv[optind]);
  pwm_shutdown();
  exit(err < 0);
}
