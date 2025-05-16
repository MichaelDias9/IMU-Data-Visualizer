#include <atomic>
#include <thread>
#include <iostream>

#include <boost/asio.hpp>
namespace net = boost::asio;

#include "WebSocketServer.h"
#include "RaylibApp.h"
#include "SharedData.h"
#include "ComplementaryFilter.h"

std::atomic<bool> filterRunning(false);

void complementaryFilterThread() {
  ComplementaryFilter filter(0.5f); // Tune alpha as needed
  auto lastTime = std::chrono::high_resolution_clock::now();

  while (filterRunning) {
      auto now = std::chrono::high_resolution_clock::now();
      float deltaTime = std::chrono::duration<float>(now - lastTime).count();
      lastTime = now;

      Vector3f accel = SharedData::Instance().getAcceleration();
      Vector3f gyro = SharedData::Instance().getRotationRate();
      filter.update(deltaTime, accel, gyro);

      Attitude att = filter.getAttitude();
      SharedData::Instance().setAttitude(att.x, att.y, att.z, att.w);

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

int main() {
  try {
      net::io_context ioc;
      WebSocketServer server(ioc, 8000);
      server.run();

      std::thread serverThread([&ioc]() { ioc.run(); });

      filterRunning = true;
      std::thread filterThread(complementaryFilterThread);

      runRaylibApp();
 
      filterRunning = false;
      ioc.stop();

      serverThread.join();
      filterThread.join();
  }
  catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
