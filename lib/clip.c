#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "clip.h"

int
pwm_clip_set(const pwm_str_t *s) {
  int pid, fd[2], status;
  char *argv[] = {"xclip", "-selection", "clipboard", NULL};

  if (pipe(fd) < 0) {
    perror("pipe");
    return -1;
  }

  if ((pid = fork()) < 0) {
    perror("fork");
    return -1;
  }

  if (pid == 0) {
    close(fd[1]);

    if (dup2(fd[0], STDIN_FILENO) < 0) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    execvp(argv[0], argv);
    perror("execvp: xclip");
    exit(EXIT_FAILURE);
  }
  close(fd[0]);

  /* NULL is a valid argument to clear the clipboard */
  if (s != NULL && write(fd[1], s->buf, s->len) < s->len) {
    fprintf(stderr, "pwm_clip_set: failed to write to clipboard");
    return -1;
  }
  close(fd[1]);

  if (waitpid(pid, &status, 0) < 0) {
    perror("waitpid");
    return -1;
  }
  return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : -1;
}

int
pwm_clip_clear() {
  return pwm_clip_set(NULL);
}
