#include "def.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

const char *usage_str = "pwm [<opts>] <cmd> [<args>]\n\n"
  "options:\n"
  "  -d <path>       database directory\n"
  "  -k <key>        gpg key id";

void usage();
void version();

typedef struct {
  char *cmd;
  char *bin;
  void (*func)();
  char *help;
} cmd_t;

const cmd_t cmds[] = {
  {"del",     PACKAGE_EXEC_DIR"/pwm-del",  NULL,    "delete a password"},
  {"gen",     PACKAGE_EXEC_DIR"/pwm-gen",  NULL,    "generate a password"},
  {"get",     PACKAGE_EXEC_DIR"/pwm-get",  NULL,    "retrieve a password"},
  {"help",    NULL,                        usage,   "show this help"},
  {"list",    PACKAGE_EXEC_DIR"/pwm-list", NULL,    "list all passwords"},
  {"log",     PACKAGE_EXEC_DIR"/pwm-log",  NULL,    "print the log"},
  {"note",    PACKAGE_EXEC_DIR"/pwm-note", NULL,    "get, set or del a password note"},
  {"set",     PACKAGE_EXEC_DIR"/pwm-set",  NULL,    "set a password"},
  {"version", NULL,                        version, "show version"}
};

typedef struct {
  char *home;
  char *key_id;
} opts_t;

#define OPTS_DEFAULT {NULL,NULL}

void
usage() {
  size_t i;

  fprintf(stderr, "usage: %s\n\ncommands:\n", usage_str);

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    fprintf(stderr, "  %-14s  %s\n", cmds[i].cmd, cmds[i].help);
  }
}

void
version() {
  fprintf(stderr, "pwm %s\n", PACKAGE_VERSION);
}

const cmd_t *
cmd_find(const char *cmd) {
  size_t i;

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    if (strcmp(cmd, cmds[i].cmd) == 0) {
      return &cmds[i];
    }
  }
  return NULL;
}

int
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
  int i;
  char c;
  size_t len;
  const cmd_t *cmd;
  opts_t opts = OPTS_DEFAULT;

  /* find position of the subcommand */
  for (i = 1; i < argc; i++) {
    if ((len = strlen(argv[i])) < 2 || argv[i][0] != '-') {
      break;
    }

    if (len == 2) {
      i++;
    }
  }

  if (i >= argc) {
    usage();
    exit(EXIT_FAILURE);
  }

  while ((c = getopt(i, argv, "d:k:")) >= 0) {
    switch (c) {
    case 'd':
      opts.home = optarg;
      break;
    case 'k':
      opts.key_id = optarg;
      break;
    default:
      usage();
      exit(EXIT_FAILURE);
    }
  }

  if (opts.home != NULL && setenv(PWM_HOME_ENV_VAR, opts.home, 1) < 0) {
    perror("setenv");
    exit(EXIT_FAILURE);
  }

  if (opts.key_id != NULL && setenv(PWM_KEY_ID_ENV_VAR, opts.key_id, 1) < 0) {
    perror("setenv");
    exit(EXIT_FAILURE);
  }

  if ((cmd = cmd_find(argv[i])) == NULL) {
    usage();
    exit(EXIT_FAILURE);
  }

  if (cmd->bin != NULL) {
    exit(cmd_exec(cmd, &argv[i]));
  }
  cmd->func();
  exit(EXIT_SUCCESS);
}
