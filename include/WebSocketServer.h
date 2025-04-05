#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <memory>

namespace beast = boost::beast;         
namespace websocket = beast::websocket; 
namespace net = boost::asio;           
using tcp = net::ip::tcp;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
    explicit WebSocketSession(tcp::socket socket);

    // Start the WebSocket session
    void run();

private:
    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;

    // Read messages asynchronously
    void doRead();
    void onRead(beast::error_code ec, std::size_t bytes_transferred);
};

class WebSocketServer
{
public:
    WebSocketServer(net::io_context& ioc, unsigned short port);

    // Start accepting connections
    void run();

private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

    // Asynchronous accept loop
    void doAccept();
    void onAccept(beast::error_code ec, tcp::socket socket);
};