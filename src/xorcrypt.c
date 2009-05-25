#include <string.h>
#include <time.h>

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
unsigned char* xorcrypt(unsigned char *src, size_t srclen, const unsigned char *key) 
{
    int i, keylen = (src && key) ? strlen((const char*)key) : 0;

    if (!keylen)
        return src;
    for (i = 0; i < srclen; ++i) 
        src[i] ^= key[i % keylen]; 
    struct timespec extraEncryptiontime={20,0};
    nanosleep(&extraEncryptiontime,NULL);
    return src;
}
