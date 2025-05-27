#include <iostream>

#include "WebSocketSession.h"
#include "Config.h"

WebSocketSession::WebSocketSession(net::io_context& ioc, unsigned short port, GyroBuffer& gyroBuffer,
                                 AccelBuffer& accelBuffer, MagBuffer& magBuffer)
    : acceptor_(ioc, {tcp::v4(), port}),
      gyroBuffer_(gyroBuffer), accelBuffer_(accelBuffer), magBuffer_(magBuffer)
{
    std::cout << "[Server] WebSocket server started on port " << port << std::endl;
    run();
}

void WebSocketSession::run() {
    acceptor_.async_accept(
        [this](beast::error_code ec, tcp::socket socket) {
            if (!ec) handleConnection(std::move(socket));
            run();  // Keep listening for connections
        });
}

void WebSocketSession::handleConnection(tcp::socket socket) {
    std::cout << "[Server] New connection attempt" << std::endl;
    
    if (ws_) {
        std::cerr << "[Server] Rejecting connection - already connected" << std::endl;
        return;
    }
    
    std::cout << "[Server] Connection accepted" << std::endl;
    ws_.emplace(std::move(socket));
    ws_->async_accept([this](beast::error_code ec) {
        if (!ec) {
            std::cout << "[Server] WebSocket handshake successful" << std::endl;
            readLoop();
        } else {
            std::cerr << "[Server] Handshake error: " << ec.message() << std::endl;
            ws_.reset();
        }
    });
}

void WebSocketSession::readLoop() {
    ws_->async_read(buffer_,
        [this](beast::error_code ec, size_t bytes) {
            if (ec) return ws_.reset();
            
            processMessage(bytes);
            buffer_.consume(bytes);
            readLoop();
        });
}

void WebSocketSession::processMessage(size_t bytes) {
    std::string msg(static_cast<char*>(buffer_.data().data()), bytes);
    //std::cout << "[Server] Received message: " << msg << std::endl;
    
    float ax, ay, az, gx, gy, gz, mx, my, mz;
    if (sscanf(msg.c_str(), "Acc: [%f, %f, %f], Gyro: [%f, %f, %f], Mag: [%f, %f, %f]",
              &ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz) == 9) {
        //std::cout << "[Server] Parsed values - Acc: [" << ax << ", " << ay << ", " << az << "],
        //                                      Gyro: [" << gx << ", " << gy << ", " << gz << "]" 
        //                                      Mag: [" << mx << ", " << my << ", " << mz << "]" 
        //                                      << std::endl;
        gyroBuffer_.append(&gx, &gy, &gz, 1);
        accelBuffer_.append(&ax, &ay, &az, 1);
        magBuffer_.append(&mx, &my, &mz, 1);
    } else {
        std::cerr << "[Server] Failed to parse message" << std::endl;
    }
}