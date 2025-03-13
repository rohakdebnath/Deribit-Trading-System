cmake_minimum_required(VERSION 3.16)

project(OMS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0167 OLD)

add_definitions(-DASIO_STANDALONE -D_WEBSOCKETPP_CPP11_STL_ -DWEBSOCKETPP_STRICT_ANSI -DBOOST_ASIO_NO_DEPRECATED)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(spdlog REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

file(GLOB SOURCES "src/*.cpp")

add_executable(OMS ${SOURCES})

target_link_libraries(OMS
    ${Boost_LIBRARIES}
    spdlog::spdlog
    ${CURL_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
    /opt/homebrew/lib/libjsoncpp.dylib
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(OMS PRIVATE -Wall -Wextra -pedantic)
endif()
