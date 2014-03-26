#include "pwm.h"
#include <stdio.h>
#include <getopt.h>

static const char *usage_str =
  "pwm note get [<opts>] <key>\n\n"
  "options:\n"
  "  -h              show this help";

static void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

static int
run(const char *key) {
  pwm_db_t *db = NULL;
  PWM_STR_INIT(buf);
  int err;

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    goto cleanup;
  }

  if (!pwm_db_has(db, "notes", key)) {
    fprintf(stderr, "couldn't find your %s note\n", key);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_get(db, "notes", key, &buf)) < 0) {
    goto cleanup;
  }
  fputs(buf.buf, stdout);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err;
}

int
main(int argc, char **argv) {
  char c, *key = NULL;
  int err;

  while ((c = getopt(argc, argv, "-:h")) > -1) {
    switch (c) {
    case 1:
      key = optarg;
      break;
    default:
      usage();
    }
  }

  if (key == NULL) {
    usage();
  }
  pwm_init();
  err = run(key);
  pwm_shutdown();
  exit(err < 0);
}
