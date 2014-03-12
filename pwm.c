#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "config.h"

const char *usage_str = "pwm <cmd> [<args>]";

typedef struct {
  char *cmd;
  char *bin;
  char *help;
} cmd_t;

const cmd_t cmds[] = {
  {"del",  PWM_EXEC_DIR"/pwm-del",  "delete a password"},
  {"gen",  PWM_EXEC_DIR"/pwm-gen",  "generate a password"},
  {"get",  PWM_EXEC_DIR"/pwm-get",  "retrieve a password"},
  {"list", PWM_EXEC_DIR"/pwm-list", "list all passwords"},
  {"log",  PWM_EXEC_DIR"/pwm-log",  "print the log"},
  {"set",  PWM_EXEC_DIR"/pwm-set",  "set a password"}
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
cmd_exec(const cmd_t *cmd, char **argv) {
  int pid, rc;

  if ((pid = fork()) < 0) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    argv[0] = basename(cmd->bin);
    execv(cmd->bin, argv);
    perror("execv");
    exit(EXIT_FAILURE);
  }
  waitpid(pid, &rc, 0);
  return WIFEXITED(rc) ? WEXITSTATUS(rc) : 1;
}

int
main(int argc, char **argv) {
  const cmd_t *cmd;

  if (argc < 2) {
    usage();
  }

  if ((cmd = cmd_find(argv[1])) == NULL) {
    usage();
  }
  exit(cmd_exec(cmd, &argv[1]));
}
