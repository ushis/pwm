#include "../config.h"
#include "pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <wait.h>

static const char *usage_str = "pwm note <cmd> [<args>]";

typedef struct {
  char *cmd;
  char *bin;
  char *help;
} cmd_t;

static const cmd_t cmds[] = {
  {"del",  PACKAGE_EXEC_DIR"/pwm-note-del",  "delete a note"},
  {"get",  PACKAGE_EXEC_DIR"/pwm-note-get",  "retrieve a note"},
  {"list", PACKAGE_EXEC_DIR"/pwm-note-list", "list all notes"},
  {"set",  PACKAGE_EXEC_DIR"/pwm-note-set",  "set a note"}
};

static void
usage() {
  size_t i;

  fprintf(stderr, "usage: %s\n\ncommands:\n", usage_str);

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    fprintf(stderr, "  %-14s  %s\n", cmds[i].cmd, cmds[i].help);
  }
  exit(EXIT_FAILURE);
}

static const cmd_t *
cmd_find(const char *cmd) {
  size_t i;

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    if (strcmp(cmd, cmds[i].cmd) == 0) {
      return &cmds[i];
    }
  }
  return NULL;
}

static int
cmd_exec(const cmd_t *cmd, char **argv) {
  pid_t pid;
  int rc;

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

  if (waitpid(pid, &rc, 0) < 0) {
    perror("waitpid");
    return 1;
  }
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
