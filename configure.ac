AC_PREREQ([2.69])
AC_INIT([pwm], [0.0.1])
AC_CONFIG_SRCDIR([pwm.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([git2], [git_threads_init])
AC_CHECK_LIB([gpgme], [gpgme_check_version])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 setenv mlockall])

# PACKAGE_EXEC_DIR
AH_VERBATIM([PACKAGE_EXEC_DIR], [#undef PACKAGE_EXEC_DIR])

AC_ARG_WITH([pwmexecdir],
  AS_HELP_STRING([--with-pwmexecdir=dir],
    [directory containing the pwm subcommand executables]),
  [],
  [with_pwmexecdir=builtin])

AC_DEFINE_UNQUOTED([PACKAGE_EXEC_DIR], ["${with_pwmexecdir}"])
AC_SUBST([pwmexecdir], [$with_pwmexecdir])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
