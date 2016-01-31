#include "messageblock.h"
#include "des_encrypt.h"

static char Des_VerifyData[8]={0x15,0x13,0x28,0x59,0x92,0xfe,0xa4,0xc7};

CMessageBlock::CMessageBlock()
{
}

void CMessageBlock::SetPassword(std::string strPassWord)
{
    strPass = strPassWord;
    strPass.append("********");
    int nLen = 8;
    DES_Encrypt(Des_VerifyData,nLen,strPass.c_str(),(char *)&EnCodeData);
}

bool CMessageBlock::CheckPassword(std::string strPassWord)
{
    int nLen = 8;
    char szDecode[8];
    strPassWord.append("********");
    DES_Decrypt((char *)&EnCodeData,nLen,strPassWord.c_str(),szDecode);
    if(memcmp(szDecode,Des_VerifyData,8)==0)
    {
        strPass = strPassWord;
        return true;
    }
    else
        return false;
}

void CMessageBlock::EncodeXml(std::string &strXml)
{
    int nLen = strXml.length();
    strXmlEncode.resize((nLen+7)&0xfffff8);
    DES_Encrypt(strXml.c_str(),nLen,strPass.c_str(),(char *)strXmlEncode.data());
}

void CMessageBlock::EncodeImage(std::string &filename,std::vector<char> &image)
{
    CImageFile jImage;
    jImage.strFileName = filename;
    int nLen = image.size();
    jImage.ImageData.resize((nLen+7)&0xffffff8);
    DES_Encrypt((char *)image.data(),nLen,strPass.c_str(),(char *)jImage.ImageData.data());
    ImageB.push_back(jImage);
}

void CMessageBlock::DecodeXml(std::string &strXml)
{
    int nLen = strXmlEncode.size();
    char *szText = new char[nLen+1];
    DES_Decrypt((char *)strXmlEncode.data(),nLen,strPass.c_str(),szText);
    szText[nLen] = '\0';
    strXml = szText;
    delete []szText;
}

void CMessageBlock::DecodeImage(int nIndex,std::vector<char> &image)
{
    if(nIndex>=ImageB.size())
        return;

    int nLen = ImageB[nIndex].ImageData.size();
    image.resize(nLen);
    DES_Decrypt((char *)ImageB[nIndex].ImageData.data(),nLen,strPass.c_str(),(char *)image.data());
}
