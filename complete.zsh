#compdef pwm

# Zsh completion for the pwm command line tool.
#
# Copy this file to ~/.zsh/completions/_pwm
#
# Add it to your fpath and init completion.
#
#   fpath=(~/.zsh/completions fpath)
#   compinit
#
# Maybe you have to purge your compdump.
_pwm() {
  local curcontext="$curcontext" state line

  _arguments \
    '1: :__pwm_commands' \
    '*:: :__pwm_arguments' \
    '-d+[pwm database directory]:dir:_directories' \
    '-h[show help]' \
    '-k+[gpg key id]:key' \
    '-v[show version]'
}

__pwm_force() {
  local w

  for w in $words; do
    [[ "${w}" == '-f' ]] && return 0
  done

  return 1
}

__pwm_keys() {
  compadd $(type pwm &>/dev/null && pwm list 2>/dev/null)
}

__pwm_commands() {
  _values 'pwm command' \
    'del[delete a password]' \
    'gen[generate a password]' \
    'get[retrieve a password]' \
    'list[list all passwords]' \
    'log[print the log]' \
    'note[get, set or delete a password note]' \
    'set[set a password]'
}

__pwm_arguments() {
  case "${words[1]}" in
    del|gen|get|note|set)
      __pwm_${words[1]} && return 0
      ;;
    *)
      return 1
  esac
}

__pwm_del() {
  _arguments \
    '*:key:__pwm_keys' \
    '-f[ignore nonexistent passwords]' \
    '-h[show help]' \
    '-m+[custom log message]:msg'
}

__pwm_gen_generators() {
  _values 'pwm generator' \
    'alnum[random alphanumeric characters]' \
    'ascii[random printable ascii characters]' \
    'hex[random hex]' \
    'num[random numeric characters]'
}

__pwm_gen() {
  local ret=1 keys=''

  __pwm_force && keys='*:key:__pwm_keys'

  _arguments \
    '-c[store password in the clipboard]' \
    '-f[override existing password]' \
    '-g+[generator to use (default: alnum)]:generator:__pwm_gen_generators' \
    '-h[show help]' \
    '-l+[password length (default: 32)]:len' \
    '-m+[custom log message]:msg' \
    '-p[print generated password]' \
    $keys
}

__pwm_get() {
  _arguments \
    '*:key:__pwm_keys' \
    '-c[store password in the clipboard]' \
    '-h[show help]'
}

__pwm_note_commands() {
  _values 'pwm note command' \
    'del[delete a note]' \
    'get[retrieve a note]' \
    'set[set a note]'
}

__pwm_note_arguments() {
  _arguments \
    '*:key:__pwm_keys' \
    '-h[show help]'
}

__pwm_note() {
  _arguments \
    '1: :__pwm_note_commands' \
    '*:: :__pwm_note_arguments'
}

__pwm_set() {
  local keys=''

  __pwm_force && keys='*:key:__pwm_keys'

  _arguments \
    '-c[store password in the clipboard]' \
    '-f[override existing password]' \
    '-h[show help]' \
    '-m+[custom log message]:msg' \
    '-p[print password]' \
    $keys
}
