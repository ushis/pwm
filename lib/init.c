#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include "git.h"
#include "gpg.h"

void
pwm_init() {
  struct rlimit zero = {0, 0};

  /* lock all memory */
  if (mlockall(MCL_FUTURE) < 0) {
    perror("mlockall");
    exit(EXIT_FAILURE);
  }

  /* disable coredumps */
  if (setrlimit(RLIMIT_CORE, &zero) < 0) {
    perror("setrlimit");
    exit(EXIT_FAILURE);
  }

  if (pwm_git_init() < 0) {
    fprintf(stderr, "git initialization failed\n");
    exit(EXIT_FAILURE);
  }

  if (pwm_gpg_init() < 0) {
    fprintf(stderr, "gpg initialization failed\n");
    exit(EXIT_FAILURE);
  }
}

void
pwm_shutdown() {
  pwm_git_shutdown();
}
