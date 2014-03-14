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
  local curcontext="$curcontext" state line ret=1

  _arguments \
    '1: :->cmd' \
    '*:: :->args' \
    && ret=0

  case "${state}" in
    cmd)
      __pwm_commands && ret=0
      ;;
    args)
      case "${line[1]}" in
        del|gen|get|note|set)
          __pwm_${line[1]} && ret=0
          ;;
        *)
          ret=1
      esac
  esac

  return ret
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
    'help[show help]' \
    'list[list all passwords]' \
    'log[print the log]' \
    'note[get, set or delete a password note]' \
    'set[set a password]' \
    'version[show pwm version]'
}

__pwm_del() {
  _arguments \
    '*:key:__pwm_keys' \
    '-f[ignore nonexistent passwords]' \
    '-h[show help]'
}

__pwm_gen() {
  local ret=1 keys=''

  __pwm_force && keys='*:key:__pwm_keys'

  _arguments \
    '-c[store password in the clipboard]' \
    '-f[override existing password]' \
    '-g+[generator to use (default: alnum)]:generator:->gen' \
    '-h[show help]' \
    '-l+[password length (default: 32)]:len' \
    '-p[print generated password]' \
    $keys \
    && ret=0

  [[ "${state}" != 'gen' ]] && return ret

  _values 'pwm generator' \
    'alnum[random alphanumeric characters]' \
    'ascii[random printable ascii characters]' \
    'hex[random hex]' \
    'num[random numeric characters]'
}

__pwm_get() {
  _arguments \
    '*:key:__pwm_keys' \
    '-c[store password in the clipboard]' \
    '-h[show help]'
}

__pwm_note() {
  local ret=1

  _arguments \
    '1: :->cmd' \
    '*:: :->args' \
    && ret=0

  case "${state}" in
    cmd)
      _values 'pwm note command' \
        'del[delete a note]' \
        'get[retrieve a note]' \
        'set[set a note]' \
        && ret=0
      ;;
    args)
      _arguments \
        '*:key:__pwm_keys' \
        '-h[show help]' \
        ret=0
  esac

  return ret
}

__pwm_set() {
  local keys=''

  __pwm_force && keys='*:key:__pwm_keys'

  _arguments \
    '-c[store password in the clipboard]' \
    '-f[override existing password]' \
    '-h[show help]' \
    '-p[print password]' \
    $keys
}
