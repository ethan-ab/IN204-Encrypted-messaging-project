

#include <string>
#include <sodium/utils.h>
#include <sodium/crypto_box.h>
#include <sodium/randombytes.h>
#include <iostream>
#include <vector>

class Utility {
public:

    static auto EncodeBase64(const std::string &in) {

        std::cout << "Encoding: " << in << std::endl;
        char base64PublicKey[crypto_box_PUBLICKEYBYTES * 4]; // Buffer to hold base64 string
        char *encodedResult =sodium_bin2base64(base64PublicKey, sizeof base64PublicKey, reinterpret_cast<const unsigned char *>(in.c_str()),
                          in.size(),
                          sodium_base64_VARIANT_ORIGINAL_NO_PADDING);

        if (encodedResult == NULL) {
            std::cerr << "Error encoding to Base64" << std::endl;
            throw std::runtime_error("Base64 encoding failed.");
        }

        std::cout << "Encoded: " << base64PublicKey << std::endl;
        return std::string(encodedResult);
    }

    static auto EncodeBase64(const unsigned char* in, size_t in_len) {
        char base64Hash[crypto_box_PUBLICKEYBYTES * 2]; // Buffer to hold base64 string
        sodium_bin2base64(base64Hash, sizeof base64Hash, in, in_len, sodium_base64_VARIANT_ORIGINAL_NO_PADDING);
        return std::string(base64Hash);
    }

    static auto DecodeBase64(const std::string &in) {
        std::vector<unsigned char> decoded(in.size());
        size_t bin_len;
        if (sodium_base642bin(decoded.data(), decoded.size(), in.c_str(), in.size(),
                              NULL, &bin_len, NULL, sodium_base64_VARIANT_ORIGINAL_NO_PADDING) != 0) {
            std::cerr << "Error decoding Base64" << std::endl;
            return std::string();
        }
        return std::string(reinterpret_cast<char *>(decoded.data()), bin_len);
    }


    static auto DecodePublicKeyFromBase64(const std::string &in) {
        std::vector<unsigned char> decoded(in.size());
        size_t bin_len;
        if (sodium_base642bin(decoded.data(), decoded.size(), in.c_str(), in.size(),
                              NULL, &bin_len, NULL, sodium_base64_VARIANT_ORIGINAL_NO_PADDING) != 0) {
            std::cerr << "Error decoding Base64" << std::endl;
            return std::string();
        }
        return std::string(reinterpret_cast<char *>(decoded.data()), bin_len);
    }


    static auto EncryptString(const std::string &message, const std::string &publicKey) {
        unsigned char nonce[crypto_box_NONCEBYTES];
        unsigned char encrypted[crypto_box_SEALBYTES + message.size()];
        randombytes_buf(nonce, sizeof nonce);
        if (crypto_box_seal(encrypted, reinterpret_cast<const unsigned char *>(message.c_str()), message.size(),
                            reinterpret_cast<const unsigned char *>(publicKey.c_str())) != 0) {
            return std::string();
        }
        return std::string(reinterpret_cast<char *>(encrypted), crypto_box_SEALBYTES + message.size());
    }

    static auto DecryptString(const std::string &encryptedMessage, const std::string &publicKey, const std::string &privateKey) {
        unsigned char decrypted[encryptedMessage.size() - crypto_box_SEALBYTES];
        if (crypto_box_seal_open(decrypted, reinterpret_cast<const unsigned char *>(encryptedMessage.c_str()),
                                 encryptedMessage.size(),
                                 reinterpret_cast<const unsigned char *>(publicKey.c_str()),
                                 reinterpret_cast<const unsigned char *>(privateKey.c_str())) != 0) {
            std::cerr << "Error decrypting message" << std::endl;
            return std::string();
        }
        return std::string(reinterpret_cast<char *>(decrypted));
    }

    static auto DecryptString(const std::string &encryptedMessage, const std::string &publicKey, const unsigned char *privateKey) {
        if (encryptedMessage.size() < crypto_box_SEALBYTES) {
            std::cerr << "Encrypted message is too short." << std::endl;
            return std::string();
        }

        std::vector<unsigned char> decrypted(encryptedMessage.size() - crypto_box_SEALBYTES);
        int error =crypto_box_seal_open(decrypted.data(), reinterpret_cast<const unsigned char *>(encryptedMessage.c_str()),
                                 encryptedMessage.size(), reinterpret_cast<const unsigned char *>(publicKey.c_str()),
                                 privateKey);
        if(error !=0){
            std::cerr << "Error decrypting message :" << error << std::endl;
            return std::string();
        }
        return std::string(reinterpret_cast<char *>(decrypted.data()), decrypted.size());
    }



};
