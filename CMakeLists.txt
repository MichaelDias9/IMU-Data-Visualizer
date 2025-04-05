cmake_minimum_required(VERSION 3.20)
project(IMUTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Boost configuration --- #
set(Boost_NO_BOOST_CMAKE ON)
set(BOOST_ROOT "H:/Libraries/boost_1_87_0")
set(BOOST_INCLUDEDIR "H:/Libraries/boost_1_87_0")
set(BOOST_LIBRARYDIR "H:/Libraries/boost_1_87_0/stage/lib")
find_package(Boost 1.87.0 REQUIRED COMPONENTS system thread)

# --- Fetch raylib --- #
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    CMAKE_ARGS 
        -DBUILD_SHARED_LIBS=OFF
        -DUSE_EXTERNAL_GLFW=OFF
        -DSUPPORT_WINMM_HIGH_RES_TIMER=OFF
)
FetchContent_MakeAvailable(raylib)

# --- Build ImGui from source --- #
file(GLOB IMGUI_SOURCES
    "third_party/imgui/*.cpp"
    "third_party/imgui/backends/imgui_impl_glfw.cpp"  # Include necessary backends if required
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends
)

# +++ Add ImPlot library +++ #
file(GLOB IMPLOT_SOURCES "third_party/implot/*.cpp")
add_library(implot STATIC ${IMPLOT_SOURCES})
target_include_directories(implot PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/implot
)
target_link_libraries(implot PRIVATE imgui)

# --- rlImGui static library --- #
add_library(rlImGui STATIC third_party/rlImGui/rlImGui.cpp)
target_include_directories(rlImGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/rlImGui
    ${raylib_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
)
target_link_libraries(rlImGui PUBLIC raylib imgui implot)

# --- Main executable --- #
add_executable(IMUTool
    src/main.cpp
    src/WebSocketServer.cpp
    src/RaylibApp.cpp
    src/ComplementaryFilter.cpp
)

target_include_directories(IMUTool PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui  
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/implot
)
target_link_libraries(IMUTool PRIVATE 
    Boost::system
    Boost::thread
    rlImGui 
    imgui
    implot
)

# --- Copy resources --- #
add_custom_command(TARGET IMUTool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:IMUTool>/resources
)