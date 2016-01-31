#ifndef CMESSAGEBLOCK_H
#define CMESSAGEBLOCK_H
#include "version.h"
#include "serialize.h"


class CImageFile{
public:
    std::string strFileName;
    std::vector<char> ImageData;
    IMPLEMENT_SERIALIZE
    (
        READWRITE(strFileName);
        READWRITE(ImageData);
    )
};


class CMessageBlock
{
public:
    explicit CMessageBlock();
    unsigned int nType;

    int64_t  EnCodeData;

    std::string strXmlPublic;
    std::vector <char>strXmlEncode;
    std::vector <CImageFile> ImageA;
    std::vector <CImageFile> ImageB;

    std::string strPass;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nType);
        READWRITE(EnCodeData);
        READWRITE(strXmlPublic);
        READWRITE(strXmlEncode);
        READWRITE(ImageA);
        READWRITE(ImageB);
    )
    void SetPassword(std::string strPassWord);
    bool CheckPassword(std::string strPassWord);
    void EncodeXml(std::string &strXml);
    void EncodeImage(std::string &filename,std::vector<char> &image);
    void DecodeXml(std::string &strXml);
    void DecodeImage(int nIndex,std::vector<char> &image);

};

#endif // CMESSAGEBLOCK_H
