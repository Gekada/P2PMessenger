cmake_minimum_required(VERSION 3.20)
project(p2pMessenger)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Qml
        Network
        Quick
        Protobuf
        Grpc
        REQUIRED
)

add_subdirectory(Messenger)
add_subdirectory(Kademlia)
