#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#ifndef PATH_MAX
#define PATH_MAX (4<<10)
#endif

#define arr_size(arr) (sizeof(arr)/sizeof(arr[0]))

const char *pwm_path[] = {"./builtin", "/usr/local/lib/pwm", "/usr/lib/pwm", NULL};

typedef struct {
  char *cmd;
  char *bin;
  char *help;
  char path[PATH_MAX];
} cmd_t;

cmd_t pwm_cmds[] = {
  {"del", "pwm-del", "delete a password"},
  {"gen", "pwm-gen", "generate a password"},
  {"get", "pwm-get", "retrieve a password"},
  {"list", "pwm-list", "list all passwords"},
  {"log", "pwm-log", "print the log"},
  {"set", "pwm-set", "set a password"}
};

cmd_t *
cmd_get(const char *cmd) {
  int i;

  for (i = 0; i < arr_size(pwm_cmds); i++) {
    if (strcmp(cmd, pwm_cmds[i].cmd) == 0) {
      return &pwm_cmds[i];
    }
  }
  return NULL;
}

int
cmd_find(cmd_t *cmd) {
  struct stat info;
  int i;

  for (i = 0; pwm_path[i] != NULL; i++) {
    snprintf(cmd->path, PATH_MAX, "%s/%s", pwm_path[i], cmd->bin);

    if (stat(cmd->path, &info) == 0 &&
        S_ISREG(info.st_mode) &&
        access(cmd->path, X_OK) == 0) {
      return 0;
    }
  }
  return -1;
}

int
cmd_exec(cmd_t *cmd, char **argv) {
  int pid, rc;

  if ((pid = fork()) < 0) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    argv[0] = cmd->bin;
    execv(cmd->path, argv);
    perror("execv");
    return 1;
  }
  waitpid(pid, &rc, 0);
  return WEXITSTATUS(rc);
}

void
usage(const char *argv0) {
  int i;

  fprintf(stderr, "usage: %s <cmd> [<args>]\n\ncommands:\n", argv0);

  for (i = 0; i < arr_size(pwm_cmds); i++) {
    fprintf(stderr, "  %-4s  %s\n", pwm_cmds[i].cmd, pwm_cmds[i].help);
  }
}

int
main(int argc, char **argv) {
  cmd_t *cmd;

  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }

  if ((cmd = cmd_get(argv[1])) == NULL) {
    usage(argv[0]);
    return 1;
  }

  if (cmd_find(cmd) < 0) {
    fprintf(stderr, "could not locate the executable: %s\n", cmd->bin);
    return 1;
  }
  return cmd_exec(cmd, &argv[1]);
}
