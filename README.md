# IMU Data Visualizer 

A C++ ImPlot project plotting real time gyroscope, accelerometer, and magnetometer sensor data. (9DOF IMU)

## Features 

  * Adjust plot sizes, y-axis zoom, and time axis zoom during run time.  
  * High performance by leveraging downsampling for plotting tens of thousands of data points simultaneously

## Installation

  * Uses CMake build generator. To build, run the following commands from the project root directory:
    * cmake -S . -B build
    * cmake --build build
  * Note: all libraries are included except for 'boost'. If you don't already have boost, then install it and add the boost home environment variable so that cmake can find it with find_package()
    
  * Run program with "build/IMUTool" from the project root.
    * Set sensor frequencies in Config.h for correct timing behaviour.
    * Decrease MAX_PLOT_POINTS in Config.h if plots or data aren't displaying properly

## Usage

  * The program uses websockets to accept incoming data from your sensors. 
  * The websocket expects incoming messages as a c string with the format:
    *  "Acc: [%f, %f, %f], Gyro: [%f, %f, %f], Mag: [%f, %f, %f]"
  


    
