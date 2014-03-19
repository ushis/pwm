#ifndef READ_H
#define READ_H 1

#include "str.h"

/**
 * Reads the contents of a file.
 *
 * @param  s     String pointer to store the result in
 * @param  path  Path to the file
 * @return 0 on success or an error code.
 */
int pwm_read_file(pwm_str_t *s, const char *path);

/**
 * Reads a line from stdin.
 *
 * @param  s       String pointer to store the result in
 * @param  prompt  Prompt to show
 * @return 0 on success or an error code
 */
int pwm_read_line(pwm_str_t *s, const char *prompt);

/**
 * Reads a line from stdin without printing the entered characters.
 *
 * @param  s       String pointer to store the result in
 * @param  prompt  Prompt to show
 * @return 0 on success or an error code
 */
int pwm_read_line_hidden(pwm_str_t *s, const char *prompt);

#endif /* READ_H */
