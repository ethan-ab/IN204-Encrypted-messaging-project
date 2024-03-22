

#include <string>
#include <sodium/utils.h>
#include <sodium/crypto_box.h>
#include <sodium/randombytes.h>

class Utility {
public:

    static auto EncodeBase64(const std::string &in) {

        char base64PublicKey[crypto_box_PUBLICKEYBYTES * 4]; // Buffer to hold base64 string
        char *encodedResult =sodium_bin2base64(base64PublicKey, sizeof base64PublicKey, reinterpret_cast<const unsigned char *>(in.c_str()),
                          in.size(),
                          sodium_base64_VARIANT_ORIGINAL_NO_PADDING);

        if (encodedResult == NULL) {
            throw std::runtime_error("Base64 encoding failed.");
        }

        return std::string(encodedResult);
    }

    static std::vector<unsigned char> DecodeBase64(std::string &b64) {
        std::cout << "Decoding: " << b64 << std::endl;

        std::vector<unsigned char> data(b64.length());
        std::size_t outLen;

        if (sodium_base642bin(data.data(), data.size(), b64.c_str(), b64.length(),
                              nullptr, &outLen, nullptr, sodium_base64_VARIANT_ORIGINAL) != 0) {
            throw std::runtime_error("Failed to decode Base64 string. 1");
        }

        data.resize(outLen);
        return data;
    }

    static std::vector<unsigned char> DecodeBase64Message(std::string &b64) {
        std::cout << "Decoding: " << b64 << std::endl;

        std::vector<unsigned char> data(b64.length()); // Plus grand que nécessaire
        std::size_t outLen;

        if (sodium_base642bin(data.data(), data.size(), b64.c_str(), b64.length(),
                              nullptr, &outLen, nullptr, sodium_base64_VARIANT_ORIGINAL_NO_PADDING) != 0) {
            throw std::runtime_error("Failed to decode Base64 string. 1");
        }

        data.resize(outLen);
        return data;
    }





    static auto encodeBase64(const unsigned char *data, std::size_t len) {
        std::size_t b64Len = sodium_base64_encoded_len(len, sodium_base64_VARIANT_ORIGINAL);
        std::vector<char> b64(b64Len);

        sodium_bin2base64(b64.data(), b64Len, data, len, sodium_base64_VARIANT_ORIGINAL);

        return std::string(b64.begin(), b64.end() - 1);
    }


    //Encryption
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

    //Decryption
    static std::string DecryptString(std::string &encryptedMessageBase64,  std::string &pubKeyBase64,  std::string &privKeyBase64) {

        std::cout << "Decoding message before" << std::endl;
        std::vector<unsigned char> encryptedMessage = DecodeBase64Message(encryptedMessageBase64);
        std::cout << "Decoding message" << std::endl;

        std::vector<unsigned char> privateKey = DecodeBase64(privKeyBase64);
        std::cout << "Decoding private key" << std::endl;

        std::vector<unsigned char> publicKey = DecodeBase64(pubKeyBase64);

        if (encryptedMessage.size() < crypto_box_SEALBYTES) {
            throw std::runtime_error("Encrypted message is too short.");
        }

        std::vector<unsigned char> decryptedMessage(encryptedMessage.size() - crypto_box_SEALBYTES);

        // Tente de déchiffrer le message
        if (crypto_box_seal_open(decryptedMessage.data(), encryptedMessage.data(), encryptedMessage.size(),
                                 publicKey.data(), privateKey.data()) != 0) {
            throw std::runtime_error("Failed to decrypt the message.");
        }

        return std::string(reinterpret_cast<char *>(decryptedMessage.data()), decryptedMessage.size());
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
