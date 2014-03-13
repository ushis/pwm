#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "pwm.h"

const char *usage_str = "pwm note <cmd> [<args>]";

const char *usage_cmd_str =
  "[<opts>] <key>\n\n"
  "options:\n"
  "  -h              show this help";

int del(pwm_db_t *db, const char *key);
int get(pwm_db_t *db, const char *key);
int set(pwm_db_t *db, const char *key);

typedef struct {
  const char *cmd;
  int (*func)(pwm_db_t *db, const char *key);
  const char *help;
} cmd_t;

const cmd_t cmds[] = {
  {"del", del, "delete a note"},
  {"get", get, "retrieve a note"},
  {"set", set, "set a note"}
};

void
usage() {
  int i;

  fprintf(stderr, "usage: %s\n\ncommands:\n", usage_str);

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    fprintf(stderr, "  %-14s  %s\n", cmds[i].cmd, cmds[i].help);
  }
  exit(EXIT_FAILURE);
}

void
usage_cmd(const char *cmd) {
  fprintf(stderr, "usage: pwm note %s %s\n", cmd, usage_cmd_str);
  exit(EXIT_FAILURE);
}

int
del(pwm_db_t *db, const char *key) {
  int err;

  if ((err = pwm_db_note_del(db, key)) >= 0) {
    fprintf(stderr, "deleted your %s note\n", key);
  }
  return err;
}

int
get(pwm_db_t *db, const char *key) {
  int err;
  PWM_STR_INIT(buf);

  if ((err = pwm_db_note_get(db, key, &buf)) >= 0) {
    fputs(buf.buf, stdout);
  }
  pwm_str_free(&buf);
  return err;
}

int
set(pwm_db_t *db, const char *key) {
  int err;
  PWM_STR_INIT(buf);

  if ((err = pwm_str_read_all(&buf, STDIN_FILENO)) < 0) {
    goto cleanup;
  }
  err = pwm_db_note_set(db, key, &buf);

cleanup:
  pwm_str_free(&buf);
  return err;
}

int
cmd_exec(const cmd_t *cmd, const char *key) {
  int err;
  pwm_db_t *db = NULL;
  PWM_STR_INIT(buf);

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, pwm_find_email())) < 0) {
    goto cleanup;
  }

  if (!pwm_db_has(db, key)) {
    fprintf(stderr, "couldn't find your %s password\n", key);
    err = -1;
    goto cleanup;
  }
  err = cmd->func(db, key);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err;
}

const cmd_t *
cmd_find(const char *cmd) {
  int i;

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    if (strcmp(cmd, cmds[i].cmd) == 0) {
      return &cmds[i];
    }
  }
  return NULL;
}

int
main(int argc, char **argv) {
  int err;
  const cmd_t *cmd;

  if (argc < 2) {
    usage();
  }

  if ((cmd = cmd_find(argv[1])) == NULL) {
    usage();
  }

  while (getopt(argc-1, &argv[1], "h") >= 0) {
    usage_cmd(cmd->cmd); /* -h is the only valid option */
  }
  optind++;

  if (optind >= argc) {
    usage_cmd(cmd->cmd);
  }
  pwm_init();
  err = cmd_exec(cmd, argv[optind]);
  pwm_shutdown();
  exit(err < 0);
}
