#ifndef CLIP_H
#define CLIP_H 1

#include "str.h"

/**
 * Writes a string to the clipboard.
 *
 * @param  s  The string to write
 * @return 0 on success or an error code
 */
int pwm_clip_set(const pwm_str_t *s);

#endif /* CLIP_H */
