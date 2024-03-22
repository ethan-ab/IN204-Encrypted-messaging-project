
#include "ClientInfo.h"




 boost::asio::ip::tcp::socket &ClientInfo::getSocket()  {
    return socket;
}

const boost::asio::streambuf &ClientInfo::getReadBuffer() const {
    return read_buffer;
}

long ClientInfo::getPing() const {
    return ping;
}

