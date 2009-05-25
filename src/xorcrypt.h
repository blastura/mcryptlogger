#ifndef XORCRYPT_H
#define XORCRYPT_H

/*
 * Name:        crypt
 * Purpose:     encrypt data extremly slow
 * Parameters:  src: buffer with data to encrypt
 *              srclen: # of bytes in buffer
 *              key: encryption key (\0-terminated ASCII string)
 * Returns:     pointer to encrypted data (same as src)
 * Comments:    The algorithm is based on XOR encryption, which is
 *              easy to break.
 *              Heaven help you if you pass a zero length key.
 *              The function is modifyed to be extremly slow.
 */ 

unsigned char* xorcrypt(unsigned char *src, size_t srclen, const unsigned char *key);

#endif
