#pragma once



#include <QObject>
#include <QThread>
#include <boost/asio.hpp>
#include <iostream>
#include <msgpack.hpp>
#include <QSettings>
#include "utils/Utility.h"

using namespace boost::asio;

class ClientSocket : public QThread
{
Q_OBJECT

public:
    explicit ClientSocket(QObject *parent = nullptr);

    [[noreturn]] void run() override;

    void sendMessage(const std::string& name, const std::string& message);

    void sendHandshake();

    void generateKeys();

    void askPublicKey(const std::string &name);

    inline void setToken(const std::string &token){
        this->token = token;
    }
    inline void setName(const std::string &name) {
        this->name = name;
    }
    std::string getName() const {
        return name;
    }
    std::string getToken() const {
        return token;
    }

    std::string getPublicKey() const {
        return publicKey;
    }
    void setPublicKey(const std::string &publicKey) {
        this->publicKey = publicKey;
    }


signals:
    void messageReceived(const QString &expeditor,const QString & message);

    void updateUiTitle(const QString &title);

private:
    io_service service;
    ip::tcp::socket* socket;
    std::string name;
    std::string token;
    std::string publicKey;
    std::string privateKey;
    //map of public keys
    std::map<std::string, std::string> publicKeys;

    std::string DecryptWithRSA( const std::string &encryptedMessage);
};


