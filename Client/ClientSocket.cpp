
#include "ClientSocket.h"
#include <iostream>
#include <sodium.h>

struct BaseRequest {
    std::string name;
    std::string token;
    MSGPACK_DEFINE(name, token);

    virtual ~BaseRequest() = default;
};

struct HandshakeRequest : public BaseRequest {
    std::string version;
    std::string publicKey;
    MSGPACK_DEFINE(MSGPACK_BASE(BaseRequest), version, publicKey);
};

struct ChatRequest : public BaseRequest {
    std::string message;
    std::string targetName;
    bool waitingKey;

    MSGPACK_DEFINE (MSGPACK_BASE(BaseRequest), message, targetName, waitingKey);
};



ClientSocket::ClientSocket(QObject *parent) : QThread(parent)
{

}

[[noreturn]] void ClientSocket::run()
{

    generateKeys();
    socket = new ip::tcp::socket(service);
    socket->connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8080));

    this->sendHandshake();
    bool isWaitingForHandshake = true;
    while(true)
    {
        boost::asio::streambuf buffer;
        boost::asio::read_until(*socket, buffer, "\n");
        std::string data = boost::asio::buffer_cast<const char*>(buffer.data());
        buffer.consume(data.size());

        if(isWaitingForHandshake)
        {
            HandshakeRequest handshakeRequest;
            std::stringstream ss(data);
            msgpack::unpack(data.data(), data.size()).get().convert(handshakeRequest);
            this->setName(handshakeRequest.name);
            this->setToken(handshakeRequest.token);
            std::cout<< "Logged in as " << handshakeRequest.name <<" with token: " <<handshakeRequest.token << std::endl;
            isWaitingForHandshake = false;
            sendHandshake();
            emit updateUiTitle(QString::fromStdString(handshakeRequest.name));
        }
        else
        {
            ChatRequest receivedData;
            std::stringstream ss(data);
            msgpack::unpack(data.data(), data.size()).get().convert(receivedData);
            if(receivedData.waitingKey){ //Response from server
                auto decodedString = Utility::DecodeBase64(receivedData.message);
                std::string decoded = std::string(decodedString.begin(), decodedString.end());
                publicKeys[receivedData.name] = decoded;
            }
            else
            {
                std::string decryptedMessage = DecryptWithRSA(  receivedData.message);
                emit messageReceived(QString::fromStdString(receivedData.name),QString::fromStdString(decryptedMessage));
            }

        }
    }
}

[[maybe_unused]] std::string ClientSocket::DecryptWithRSA(const std::string& encryptedMessage) {

    if (encryptedMessage.size() <= crypto_box_SEALBYTES) {
        std::cerr << "Encrypted message is too short to be valid." << std::endl;
        return "";
    }
    std::string messageToDecode = encryptedMessage;
    const auto decyptedMessage = Utility::DecryptString(messageToDecode,this->publicKey, this->privateKey);

    std::cout << "Decrypted message: " << decyptedMessage << std::endl;

    return std::string(decyptedMessage.begin(), decyptedMessage.end() );
}

void ClientSocket::sendHandshake() {
    QSettings settings("IN204","ChatApp");
    HandshakeRequest request;
    request.name = settings.value("username").toString().toStdString() ;
    request.version = "1.0"; //TODO
    request.token =this->getToken();
    std::string publicKey = this->getPublicKey();
    publicKey.erase(std::remove(publicKey.begin(), publicKey.end(), '\n'), publicKey.end());;
    request.publicKey =publicKey;


    std::stringstream buffer;
    msgpack::pack(buffer, request);
    boost::asio::write(*socket, boost::asio::buffer(buffer.str() + "\n"));

}



void ClientSocket::sendMessage(const std::string &targetName, const std::string &message)
{

    // Récupère la clé publique du destinataire
    std::string targetPublicKeyStr = publicKeys[targetName];
    if (targetPublicKeyStr.empty()) {
        std::cout << "No public key for " << targetName << std::endl;
        askPublicKey(targetName);
        return;
    }


    // EncryptMessage
    const auto cypheredMessage = Utility::EncryptString(message, targetPublicKeyStr);
    const auto encodedMessage = Utility::EncodeBase64(cypheredMessage);



    ChatRequest sendData;
    sendData.name = this->getName();
    sendData.token = this->getToken();
    sendData.targetName = targetName;
    sendData.message = encodedMessage; // On utilise le message chiffré
    sendData.waitingKey = false;

    size_t bin_len2 = cypheredMessage.size();

    // On emballe et on envoie le message comme avant
    std::stringstream buffer;
    msgpack::pack(buffer, sendData);

    boost::asio::write(*socket, boost::asio::buffer(buffer.str() + "\n"));

}


void ClientSocket::generateKeys(){
    unsigned char publicKey[crypto_box_PUBLICKEYBYTES];
    unsigned char privateKey[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(publicKey, privateKey); //generate keys

    // Encode To base64 without return line
    this->publicKey = Utility::encodeBase64(publicKey, crypto_box_PUBLICKEYBYTES);
    this->privateKey = Utility::encodeBase64(privateKey, crypto_box_SECRETKEYBYTES);

}


void ClientSocket::askPublicKey(const std::string &name) {
    ChatRequest sendData;
    sendData.name = this->getName();
    sendData.token = this->getToken();
    sendData.targetName = name;
    sendData.message = "Ask for public key";
    sendData.waitingKey = true;


    std::stringstream buffer;
    msgpack::pack(buffer, sendData);

    boost::asio::write(*socket, boost::asio::buffer(buffer.str() + "\n"));
    std::cout << "Asking public key \n";
}

