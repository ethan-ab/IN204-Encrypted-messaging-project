#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <list>
#include <memory>
#include "../client/ClientInfo.h"
#include "../utils/Utils.h"
#include "../database/SqlManager.h"


struct BaseRequest {
    std::string name;
    std::string token;
    MSGPACK_DEFINE (name, token);

    virtual ~BaseRequest() = default;
};

struct HandshakeRequest : public BaseRequest {
    std::string version;
    std::string publicKey;
    MSGPACK_DEFINE (MSGPACK_BASE(BaseRequest), version, publicKey);
};

struct PublicKeyResponse : public BaseRequest {
    std::string publicKey;
    MSGPACK_DEFINE (MSGPACK_BASE(BaseRequest), publicKey);
};

struct ChatRequest : public BaseRequest {
    std::string message;
    std::string targetName;
    bool waitingKey;

    MSGPACK_DEFINE (MSGPACK_BASE(BaseRequest), message, targetName, waitingKey);
};


class BoostServer {

    boost::asio::io_service m_ioservice;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::list<ClientInfo> m_connections;
    using con_handle_t = std::list<ClientInfo>::iterator;


public:
    BoostServer() : m_ioservice(), m_acceptor(m_ioservice), m_connections(), m_sqlManager("server.db") {}

    void HandleRead(con_handle_t con_handle, boost::system::error_code const &err, size_t bytes_transfered);

    void DoAsyncRead(con_handle_t con_handle);

    void
    HandleWrite(con_handle_t con_handle, std::shared_ptr<std::string> msg_buffer, boost::system::error_code const &err);

    void HandleAccept(con_handle_t con_handle, boost::system::error_code const &err);


    void StartAccept();

    void Listen(uint16_t port);

    void Run();

    ClientInfo *findClient(const std::string &name);

    void handleMessageRequest(con_handle_t con_handle, const ChatRequest& request);

    void handleUnauthenticatedRequest(con_handle_t con_handle, HandshakeRequest request);

    //con_handle && shared pointer
    void sendResponse(con_handle_t con_handle, ClientInfo &target, std::shared_ptr<std::string> response);

private:
    SqlManager m_sqlManager;

    void sendPublicKey(con_handle_t con_handle, ClientInfo &client, ClientInfo &target);
};
