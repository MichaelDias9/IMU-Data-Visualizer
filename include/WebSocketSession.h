#pragma once
#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include "Config.h"

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class WebSocketSession {
public:
    WebSocketSession(net::io_context& ioc, unsigned short port, GyroBuffer& gyrobuffer, 
                     AccelBuffer& accelBuffer, MagBuffer& magBuffer);
    
    void run();
    
private:
    void handleConnection(tcp::socket socket);
    void readLoop();
    void processMessage(size_t bytes);

    tcp::acceptor acceptor_;
    std::optional<beast::websocket::stream<tcp::socket>> ws_;
    beast::flat_buffer buffer_;

    GyroBuffer& gyroBuffer_;
    AccelBuffer& accelBuffer_;
    MagBuffer& magBuffer_;
};