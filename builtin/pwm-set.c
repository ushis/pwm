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

  if (!opts->force && pwm_db_has(db, "passwd", key)) {
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

  if (buf.len == 0) {
    fputs("i am not going to save an empty password\n", stderr);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_set(db, "passwd", key, opts->msg, &buf)) < 0) {
    goto cleanup;
  }

  if (opts->clip && (err = pwm_clip_set(&buf)) >= 0) {
    fprintf(stderr, "\rstored your %s password in the clipboard\n", key);
  } else {
    fprintf(stderr, "\rsaved your %s password\n", key);
  }

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

  while ((c = getopt(argc, argv, "-:cfhm:p")) > -1) {
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
  pwm_init();
  err = run(&opts, key);
  pwm_shutdown();
  exit(err < 0);
}
