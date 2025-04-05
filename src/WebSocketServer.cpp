#include "WebSocketServer.h"
#include "SharedData.h"

WebSocketSession::WebSocketSession(tcp::socket socket)
    : ws_(std::move(socket))
{
}

void WebSocketSession::run()
{
    // Start the WebSocket handshake
    auto self = shared_from_this();
    ws_.async_accept(
        [self](beast::error_code ec) {
            if (!ec)
            {
                self->doRead();
            }
        });
}

void WebSocketSession::doRead()
{
    auto self = shared_from_this();
    ws_.async_read(
        buffer_,
        [self](beast::error_code ec, std::size_t bytes_transferred) {
            self->onRead(ec, bytes_transferred);
        });
}

void WebSocketSession::onRead(beast::error_code ec, std::size_t bytes_transferred)
{
    if (ec)
    {
        std::cerr << "Read error: " << ec.message() << std::endl;
        return;
    }

    // Extract message from buffer
    std::string message(static_cast<char*>(buffer_.data().data()), bytes_transferred);
    //std::cout << "Received: " << message << std::endl;

    // Clear the Beast buffer
    buffer_.consume(buffer_.size());

    // Expected format: "Acc: [%f, %f, %f], Gyro: [%f, %f, %f]"
    float ax, ay, az, gx, gy, gz;
    int count = sscanf( message.c_str(), "Acc: [%f, %f, %f], Gyro: [%f, %f, %f]", 
                        &ax, &ay, &az, &gx, &gy, &gz);

    if (count == 6)
    {
        // Set the accelerometer values as total acceleration
        SharedData::Instance().setAcceleration(ax, ay, az);

        // Set the gyroscope values as rotation rate
        SharedData::Instance().setRotationRate(gx, gy, gz);
    }
    else
    {
        std::cerr << "Parsing error: received message does not match expected format." << std::endl;
    }

    // Continue reading from the socket
    doRead();
}

// WebSocketServer implementation
WebSocketServer::WebSocketServer(net::io_context& ioc, unsigned short port)
    : ioc_(ioc), acceptor_(ioc, tcp::endpoint(tcp::v4(), port))
{
}

void WebSocketServer::run()
{
    doAccept();
}

void WebSocketServer::doAccept()
{
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            onAccept(ec, std::move(socket));
        });
}

void WebSocketServer::onAccept(beast::error_code ec, tcp::socket socket)
{
    if (!ec)
    {
        // Create a new WebSocket session for each client
        std::make_shared<WebSocketSession>(std::move(socket))->run();
    }
    else
    {
        std::cerr << "Accept error: " << ec.message() << std::endl;
    }

    // Continue accepting other connections
    doAccept();
}