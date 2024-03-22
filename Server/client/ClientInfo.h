#pragma once

#include <netinet/in.h>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <msgpack.hpp>




class ClientInfo {

public:

    ClientInfo(boost::asio::io_service &io_service) : socket(io_service), read_buffer(), authenticated(false) {}

    ClientInfo(boost::asio::io_service &io_service, size_t max_buffer_size) : socket(io_service),
                                                                              read_buffer(max_buffer_size),
                                                                              authenticated(
                                                                                      false) {}

    ClientInfo(std::string name, std::string token, std::string pin);

    boost::asio::ip::tcp::socket &getSocket();

    const boost::asio::streambuf &getReadBuffer() const;



    long getPing() const;


    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf read_buffer;

   [[nodiscard]] inline bool isAuthenticated() const{
        return authenticated;
    }
    inline void setAuthenticated(bool authenticated){
        this->authenticated = authenticated;
   }

    [[nodiscard]] inline std::string getName() const{
         return name;
    }
    inline void setName(const std::string &name){
        this->name = name;
    }

    [[nodiscard]] inline std::string getToken() const{
        return token;
    }
    inline void setToken(const std::string &token){
        this->token = token;
    }

    inline void setHandshakeAttempts(int attempts){
        this->handshakeAttempts = attempts;
    }
    [[nodiscard]] inline int getHandshakeAttempts() const{
        return handshakeAttempts;
    }


    [[nodiscard]] inline std::string getPin() const{
        return pin;
    }


    [[nodiscard]] inline std::string getPublicKey() const{
        return publicKey;
    }
    [[nodiscard]] inline void setPublicKey(const std::string &pubKey){
        this->publicKey = pubKey;
    }

    inline bool hasPublicKey(const std::string &pubKey){
        return publicKeys.find(pubKey) != publicKeys.end();
    }


    inline void addPublicKey(const std::string &pubKey){
        publicKeys.insert(pubKey);
    }

private:
    long ping;
    bool authenticated;
    std::string name;
    std::string token;
    std::string pin;
    std::string publicKey;
    int handshakeAttempts;

    //set of client's public key
    std::set<std::string> publicKeys;

};
