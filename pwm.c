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

static const char *usage_str =
  "pwm [<opts>] <cmd> [<args>]\n\n"
  "options:\n"
  "  -d <path>       database directory\n"
  "  -h              show this help\n"
  "  -k <key>        gpg key id\n"
  "  -v              show version";

typedef struct {
  char *cmd;
  char *bin;
  char *help;
} cmd_t;

static const cmd_t cmds[] = {
  {"del",  PACKAGE_EXEC_DIR"/pwm-del",  "delete a password"},
  {"gen",  PACKAGE_EXEC_DIR"/pwm-gen",  "generate a password"},
  {"get",  PACKAGE_EXEC_DIR"/pwm-get",  "retrieve a password"},
  {"list", PACKAGE_EXEC_DIR"/pwm-list", "list all passwords"},
  {"log",  PACKAGE_EXEC_DIR"/pwm-log",  "print the log"},
  {"note", PACKAGE_EXEC_DIR"/pwm-note", "get, set or del a password note"},
  {"set",  PACKAGE_EXEC_DIR"/pwm-set",  "set a password"}
};

typedef struct {
  char *home;
  char *key_id;
} opts_t;

#define OPTS_DEFAULT {NULL,NULL}

static void
usage() {
  size_t i;

  fprintf(stderr, "usage: %s\n\ncommands:\n", usage_str);

  for (i = 0; i < (sizeof(cmds)/sizeof(cmd_t)); i++) {
    fprintf(stderr, "  %-14s  %s\n", cmds[i].cmd, cmds[i].help);
  }
  exit(EXIT_FAILURE);
}

static void
version() {
  fprintf(stderr, "pwm %s\n", PACKAGE_VERSION);
  exit(EXIT_SUCCESS);
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
  opts_t opts = OPTS_DEFAULT;
  const cmd_t *cmd;
  char c;

  while ((c = getopt(argc, argv, "+:d:hk:v")) > -1) {
    switch (c) {
    case 'd':
      opts.home = optarg;
      break;
    case 'k':
      opts.key_id = optarg;
      break;
    case 'v':
      version();
      break;
    default:
      usage();
    }
  }

  if (optind >= argc) {
    usage();
  }

  if (opts.home != NULL && setenv(PWM_HOME_ENV_VAR, opts.home, 1) < 0) {
    perror("setenv");
    exit(EXIT_FAILURE);
  }

  if (opts.key_id != NULL && setenv(PWM_KEY_ID_ENV_VAR, opts.key_id, 1) < 0) {
    perror("setenv");
    exit(EXIT_FAILURE);
  }

  if ((cmd = cmd_find(argv[optind])) == NULL) {
    usage();
  }
  exit(cmd_exec(cmd, &argv[optind]));
}
