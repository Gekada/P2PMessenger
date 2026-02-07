
set(KADEMLIALIB_HEADERS
        include/node.h
        include/routing_table.h
        include/node_entry.h
        include/constants.h
        include/utils.h
)

set(KADEMLIALIB_SOURCES
        src/node.cpp
        src/routing_table.cpp
        src/utils.cpp
)

option(BUILD_TEST_APP "Enables build of an executable for testing purposes" ON)

find_package(Protobuf CONFIG REQUIRED)

qt_add_library(kademliaLib STATIC
        ${KADEMLIALIB_SOURCES}
        ${KADEMLIALIB_HEADERS}
)

add_subdirectory(proto)

target_link_libraries(kademliaLib PRIVATE Qt6::Core Qt6::Network protoLib)
target_include_directories(kademliaLib PUBLIC include)

if (BUILD_TEST_APP)
    add_executable(cliTest app/main.cpp)
    target_link_libraries(cliTest PRIVATE kademliaLib Qt6::Core Qt6::Network protoLib)
    target_include_directories(cliTest PUBLIC include)
endif ()