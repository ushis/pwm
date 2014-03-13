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

void usage();
void version();

typedef struct {
  char *cmd;
  char *bin;
  void (*func)();
  char *help;
} cmd_t;

const cmd_t cmds[] = {
  {"del",     PWM_EXEC_DIR"/pwm-del",  NULL,    "delete a password"},
  {"gen",     PWM_EXEC_DIR"/pwm-gen",  NULL,    "generate a password"},
  {"get",     PWM_EXEC_DIR"/pwm-get",  NULL,    "retrieve a password"},
  {"help",    NULL,                    usage,   "show this help"},
  {"list",    PWM_EXEC_DIR"/pwm-list", NULL,    "list all passwords"},
  {"log",     PWM_EXEC_DIR"/pwm-log",  NULL,    "print the log"},
  {"note",    PWM_EXEC_DIR"/pwm-note", NULL,    "get, set or del a password note"},
  {"set",     PWM_EXEC_DIR"/pwm-set",  NULL,    "set a password"},
  {"version", NULL,                    version, "show version"}
};

void
usage() {
  int i;

  fprintf(stderr, "usage: %s\n\ncommands:\n", usage_str);

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    fprintf(stderr, "  %-14s  %s\n", cmds[i].cmd, cmds[i].help);
  }
}

void
version() {
  fprintf(stderr, "pwm %s\n", PWM_VERSION);
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
    exit(EXIT_FAILURE);
  }

  if ((cmd = cmd_find(argv[1])) == NULL) {
    usage();
    exit(EXIT_FAILURE);
  }

  if (cmd->bin != NULL) {
    exit(cmd_exec(cmd, &argv[1]));
  }
  cmd->func();
  exit(EXIT_SUCCESS);
}
