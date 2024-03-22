
#include "BoostServer.h"
#include <msgpack.hpp>
#include <random>


/**
 * Démarre l'écoute du serveur sur le port spécifié.
 *
 * @param port Numéro de port sur lequel le serveur doit écouter.
 */
void BoostServer::Listen(uint16_t port) {
    auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen();
    StartAccept();
}

/**
 * Commence l'attente d'une nouvelle connexion.
 *
 * Cette fonction initialise une nouvelle attente de connexion asynchrone.
 */
void BoostServer::StartAccept() {
    auto con_handle = m_connections.emplace(m_connections.begin(), m_ioservice);
    auto handler = boost::bind(&BoostServer::HandleAccept, this, con_handle, boost::asio::placeholders::error);
    m_acceptor.async_accept(con_handle->socket, handler);
}

/**
 * Gère une nouvelle connexion acceptée.
 *
 * Cette fonction est appelée lorsqu'une nouvelle connexion est acceptée par le serveur.
 * Elle gère l'envoi d'un message de bienvenue et initialise la lecture asynchrone.
 *
 * @param con_handle Gestionnaire de la connexion représentant la nouvelle connexion acceptée.
 * @param err Code d'erreur, s'il y en a, lors de l'acceptation de la connexion.
 */
void BoostServer::HandleAccept(con_handle_t con_handle, boost::system::error_code const &err) {
    if (!err) {
        std::cout << "Connection from: " << con_handle->socket.remote_endpoint().address().to_string() << "\n";
        DoAsyncRead(con_handle);
    } else {
        std::cerr << "We had an error: " << err.message() << std::endl;
        m_connections.erase(con_handle);
    }
    StartAccept();
}

/**
 * Initie une opération de lecture asynchrone jusqu'à un délimiteur donné.
 *
 * @param con_handle Gestionnaire de la connexion pour laquelle l'opération de lecture est initiée.
 */
void BoostServer::DoAsyncRead(con_handle_t con_handle) {
    auto handler = boost::bind(&BoostServer::HandleRead, this, con_handle, boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred);
    boost::asio::async_read_until(con_handle->socket, con_handle->read_buffer, "\n", handler);

}


/**
 * Gère la lecture asynchrone des données reçues d'une connexion.
 *
 * @param con_handle Gestionnaire de la connexion pour laquelle la lecture est effectuée.
 * @param err Code d'erreur, s'il y en a, lors de l'opération de lecture.
 * @param bytes_transfered Nombre d'octets lus pendant l'opération de lecture.
 */
void BoostServer::HandleRead(con_handle_t con_handle, boost::system::error_code const &err, size_t bytes_transfered) {
    if (bytes_transfered > 0) {

        std::istream is(&con_handle->read_buffer);
        std::string str;
        std::getline(is, str);

        msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
        msgpack::object obj = oh.get();

        ClientInfo &client = *con_handle;
        //check if the client is authenticated
        if (client.isAuthenticated()) {
            ChatRequest chatRequest;
            obj.convert(chatRequest);

            auto target = findClient(chatRequest.targetName);
            //  if (!target) {
            // Handle error: Target client does not exist
            //    return;
            //}

            if (!client.hasPublicKey(target->getName())) {
                sendPublicKey(con_handle, client, *target);
                sendPublicKey(con_handle, *target, client);
                client.addPublicKey(target->getName());
                target->addPublicKey(client.getName());
                std::cout << "Sending public key of " << target->getName() << std::endl;

            } else {
                std::cout << "Message from " << chatRequest.name << " to " << chatRequest.targetName << ": "
                          << chatRequest.message << std::endl;

                handleMessageRequest(con_handle, chatRequest); //Read chat request and send message to target
            }

        } else {
            HandshakeRequest handshakeRequest;
            obj.convert(handshakeRequest);
            handleUnauthenticatedRequest(con_handle, handshakeRequest);

        }

    }

    if (!err) {
        DoAsyncRead(con_handle);
    } else {
        std::cerr << "Client disconnected: " << err.message() << std::endl;
        m_connections.erase(con_handle);
    }
}

/**
 * Gère l'achèvement d'une opération d'écriture asynchrone.
 *
 * @param con_handle Gestionnaire de la connexion pour laquelle l'écriture a été effectuée.
 * @param msg_buffer Tampon partagé contenant le message qui a été écrit.
 * @param err Code d'erreur, s'il y en a, lors de l'opération d'écriture.
 */
void BoostServer::HandleWrite(con_handle_t con_handle, std::shared_ptr<std::string> msg_buffer,
                              boost::system::error_code const &err) {
    if (!err) {
        if (con_handle->socket.is_open()) {
            // Write completed successfully and connection is open
        }
    } else {
        std::cerr << "We had an error: " << err.message() << std::endl;
        m_connections.erase(con_handle);
    }
}


void BoostServer::Run() {
    m_ioservice.run();
}

ClientInfo *BoostServer::findClient(const std::string &name) {
    for (auto &client: m_connections) {
        if (client.getName() == name) {
            return &client;
        }
    }

    //TODO: check return
    return nullptr;

}

/**
 * Handle chat request and send message to target
 *
 * @param con_handle
 * @param chatRequest
 */
void BoostServer::handleMessageRequest(con_handle_t con_handle, const ChatRequest &chatRequest) {
    std::cout << "Message from " << chatRequest.name << " to " << chatRequest.targetName << ": " << chatRequest.message
              << std::endl;

    std::stringstream buffer;
    msgpack::pack(buffer, chatRequest);

    ClientInfo *target = findClient(chatRequest.targetName);
    auto message = std::make_shared<std::string>(buffer.str() + "\n");
    auto handler = boost::bind(&BoostServer::HandleWrite, this, con_handle, message, boost::asio::placeholders::error);
    boost::asio::async_write(target->getSocket(), boost::asio::buffer(*message), handler);

    // sendResponse(con_handle, target, message); TODO: fix
}


void BoostServer::sendPublicKey(con_handle_t con_handle, ClientInfo &client, ClientInfo &target) {
    ChatRequest response;
    response.name = target.getName();
    response.message = target.getPublicKey(); // Assume 'getPublicKey' gets the public key
    response.waitingKey = true;

    std::stringstream buffer;
    msgpack::pack(buffer, response);

    auto message = std::make_shared<std::string>(buffer.str() + "\n");
    auto handler = boost::bind(&BoostServer::HandleWrite, this, con_handle, message, boost::asio::placeholders::error);
    boost::asio::async_write(client.getSocket(), boost::asio::buffer(*message), handler);
}

/**
 * Handle unauthenticated request and send handshake.
 *
 * @param con_handle
 * @param handshakeRequest
 */
void BoostServer::handleUnauthenticatedRequest(con_handle_t con_handle, HandshakeRequest handshakeRequest) {
    ClientInfo &client = *con_handle;

    if (handshakeRequest.token == client.getToken() &&
        !client.getToken().empty()) { //TODO : Check if token is valid in database
        client.setAuthenticated(true);
        client.setPublicKey(handshakeRequest.publicKey);

        std::cout << "Client " << client.getName() << " authenticated with token: " << client.getToken() << "and key "
                  << client.getPublicKey() << std::endl;
        m_sqlManager.saveClient(client); //TODO: if not exist
        return;
    }

    if (client.getHandshakeAttempts() > 2) {
        std::cout << "Too many handshake attempts, closing connection\n";
        con_handle->socket.close();
        m_connections.erase(con_handle);
        return;
    }

    client.setHandshakeAttempts(client.getHandshakeAttempts() + 1);
    std::cout << "Invalid handshake, générating new session\n" << std::endl;

    if (handshakeRequest.name.empty() || m_sqlManager.isNameTaken(handshakeRequest.name, handshakeRequest.token)) {
        int count = m_connections.size();
        const std::string name = "Client-" + std::to_string(count);
        handshakeRequest.name = name;
    }

    client.setName(handshakeRequest.name);

    const std::string token = "Token" + std::to_string(rand());

    client.setToken(token);
    handshakeRequest.token = token;
    handshakeRequest.version = "1.0"; //TODO: load from config

    std::stringstream buffer;
    msgpack::pack(buffer, handshakeRequest);
    auto buff = std::make_shared<std::string>(buffer.str() + "\n");

    sendResponse(con_handle, client, buff);

}

/**
 * Send buffer to a specific client
 *
 * @param con_handle: connection handle
 * @param target: Target client
 * @param response: Buffer to send
 */
void BoostServer::sendResponse(con_handle_t con_handle, ClientInfo &target, std::shared_ptr<std::string> response) {
    auto handler = boost::bind(&BoostServer::HandleWrite, this, con_handle, response,
                               boost::asio::placeholders::error);
    boost::asio::async_write(con_handle->socket, boost::asio::buffer(*response), handler);
}

