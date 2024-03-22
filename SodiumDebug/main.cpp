#include <iostream>
#include <sodium/crypto_box.h>
#include "Utility.h"

int main() {
    unsigned char publicKey[crypto_box_PUBLICKEYBYTES];
    unsigned char privateKey[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(publicKey, privateKey); //generate keys


    //Chiffrement
    const auto message = "Hello, World!";
    const auto publicKeyString = std::string(reinterpret_cast<char *>(publicKey), crypto_box_PUBLICKEYBYTES);
    const auto cypheredMessage = Utility::EncryptString(message, publicKeyString);
    const auto encodedMessage = Utility::EncodeBase64(cypheredMessage);


    //dechifffrement

    const auto decodedMessage = Utility::DecodeBase64(encodedMessage);
    const auto decryptedMessage = Utility::DecryptString(decodedMessage, publicKeyString, privateKey);

    std::cout << "Message dechiffré : " << decryptedMessage << std::endl;



}
