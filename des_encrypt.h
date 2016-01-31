#ifndef DES_ENCRYPT_H
#define DES_ENCRYPT_H
int DES_Encrypt(const char *pSrcData,int &nLen,const char *keyStr,char *pDesData);
int DES_Decrypt(const char *pSrcData, int &nLen, const char *keyStr,char *pDesData);
#endif // DES_ENCRYPT_H
