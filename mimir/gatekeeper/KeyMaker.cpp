#include "KeyMaker.h"
#include "../utils/mUtil.h"
#include <experimental/filesystem>
#include <string.h>

namespace fs = std::experimental::filesystem::v1;

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';

    }

    return ret;

}

std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

std::string KeyMaker::base64Encode(std::string s) {
    return base64_encode(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
}

std::string KeyMaker::GenUserAESKey(std::string hardwareSign, std::string userAuthKey)
{
    return MUtil::md5String(hardwareSign + "||" + userAuthKey);
}

bool KeyMaker::GenRSAKeyPair(std::string outDir)
{
    auto rawFile = outDir + "/cotRawKey.pem";
    auto publicKeyFile = outDir + "/publickey.pem";
    auto privateKeyFile = outDir + "/privatekey.pem";
    logger.log("GenRawKey", LHEADER);
    auto cmd = "openssl genrsa -out " + rawFile + " 2048";
    system(cmd.c_str());
    if (!fs::exists(rawFile))
    {
        return false;
    }
    logger.log("GenPrivateKey", LHEADER);
    cmd = "openssl pkcs8 -topk8 -inform PEM -outform PEM -in " + rawFile + " -out " + privateKeyFile + " -nocrypt";
    system(cmd.c_str());
    if (!fs::exists(privateKeyFile))
    {
        return false;
    }
    logger.log("GenPublicKey", LHEADER);
    cmd = "openssl rsa -in " + rawFile + " -pubout -outform PEM -out " + publicKeyFile;
    system(cmd.c_str());
    if (!fs::exists(publicKeyFile))
    {
        return false;
    }
    return true;
}

std::string KeyMaker::base64Decode(std::string s) {
    return base64_decode(s);
}

RSA* KeyMaker::loadRSAPublicKey(const std::string& filePath)
{
    FILE* hPubKeyFile = fopen(filePath.c_str(), "rb");
    if (hPubKeyFile == NULL)
    {
        return NULL;
    }
    RSA* pRSAPublicKey = PEM_read_RSA_PUBKEY(hPubKeyFile, NULL, NULL, NULL);
    fclose(hPubKeyFile);
    return pRSAPublicKey;
}

RSA* KeyMaker::loadRSAPrivateKey(const std::string& filePath)
{
    FILE* hPriKeyFile = fopen(filePath.c_str(), "rb");
    if (hPriKeyFile == NULL)
    {
        return NULL;
    }
    RSA* pRSAPriKey = PEM_read_RSAPrivateKey(hPriKeyFile, NULL, NULL, NULL);
    fclose(hPriKeyFile);
    return pRSAPriKey;

}

void KeyMaker::rsaEnviromentCleanup(RSA* pubKey, RSA* privateKey)
{
    try
    {
        RSA_free(pubKey);
        RSA_free(privateKey);
        CRYPTO_cleanup_all_ex_data();
    }
    catch (std::exception e)
    {

    }
}

std::string KeyMaker::encodeRSA(RSA* rsaPublicKey, const std::string& encodeData)
{
    if (rsaPublicKey == NULL || encodeData.empty())
    {
        return "";
    }

    std::string strRet = "";
    int nLen = RSA_size(rsaPublicKey);
    char* pEncode = new char[nLen + 1];
    int ret = RSA_public_encrypt(encodeData.length(), (const unsigned char*)encodeData.c_str(), (unsigned char*)pEncode, rsaPublicKey, RSA_PKCS1_PADDING);
    if (ret >= 0)
    {
        strRet = std::string(pEncode, ret);
    }
    delete[] pEncode;
    return strRet;
}

std::string KeyMaker::decodeRSA(RSA* rsaPrivateKey, const std::string& strData)
{
    if (rsaPrivateKey == NULL || strData.empty())
    {
        return "";
    }
    std::string strRet = "";
    int nLen = RSA_size(rsaPrivateKey);
    char* pDecode = new char[nLen + 1];
    int ret = RSA_private_decrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pDecode, rsaPrivateKey, RSA_PKCS1_PADDING);
    if (ret >= 0)
    {
        strRet = std::string((char*)pDecode, ret);
    }
    delete[] pDecode;
    return strRet;
}

bool KeyMaker::aesEncrypt(unsigned char* in, unsigned char* key, unsigned char* out,size_t len)
{
    if (!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];
    for (int i = 0; i < AES_BLOCK_SIZE; ++i)
        iv[i] = 0;
    AES_KEY aes;
    if (AES_set_encrypt_key(key, 256, &aes) < 0)
    {
        return false;
    }
    AES_cbc_encrypt(in, out, len, &aes, iv, AES_ENCRYPT);
    return true;
}
bool KeyMaker::aesDecrypt(unsigned char* in, unsigned char* key, unsigned char* out,size_t len)
{
    if (!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];
    for (int i = 0; i < AES_BLOCK_SIZE; ++i)
        iv[i] = 0;
    AES_KEY aes;
    //logger.debug("AES_set_decrypt_key");
    if (AES_set_decrypt_key(key, 256, &aes) < 0)
    {
        return false;
    }
    //logger.debug("AES_set_decrypt_key done");
    //logger.debug("AES_cbc_encrypt");
    AES_cbc_encrypt(in, out, len, &aes, iv, AES_DECRYPT);
   // logger.debug("AES_cbc_encrypt done");
    return true;
}
