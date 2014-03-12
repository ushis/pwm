#ifndef HEX_H
#define HEX_H 1

/**
 * Calculates the minimal buffer size needed to encode data of a
 * specified size.
 *
 * @param  n  Size of the input buffer
 * @return The calculated buffer size
 */
size_t pwm_hex_encode_len(size_t n);

/**
 * Calculates the minimal buffer size needed to decode data of a
 * specified size.
 *
 * @param  n  SIze of the input buffer
 * @return The calculated buffer size
 */
size_t pwm_hex_decode_len(size_t n);

/**
 * Encodes a buffer to hex.
 *
 * @param  dst  Buffer to store the result in
 * @param  src  Buffer to encode
 * @param  n    Size of the input buffer
 * @return Number of written bytes
 */
size_t pwm_hex_encode(char *dst, const char *src, size_t n);

/**
 * Decodes a hex encoded buffer.
 *
 * @param  dst  Buffer to store the result in
 * @param  src  Buffer to decode
 * @param  n    Number
 * @return Number of written bytes or an error code
 */
ssize_t pwm_hex_decode(char *dst, const char *src, size_t n);

#endif /* HEX_H */
