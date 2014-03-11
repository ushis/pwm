#ifndef HEX_H
#define HEX_H 1

/* calculates min size of a buffer needed to encode a buffer of size len */
size_t pwm_hex_encode_len(size_t len);

/* calculates min size of a buffer needed to decode a buffer of size len */
size_t pwm_hex_decode_len(size_t len);

/* encodes a raw src to hex and writes to dst */
size_t pwm_hex_encode(char *dst, const char *src, size_t len);

/* decodes a hex src to raw bytes and writes to dst */
ssize_t pwm_hex_decode(char *dst, const char *src, size_t len);

#endif /* HEX_H */
