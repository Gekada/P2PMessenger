
set(KADEMLIALIB_HEADERS
        include/node.h
        include/routing_table.h
        include/node_entry.h
        include/constants.h
        include/utils.h
        include/message_wrapper.h
        include/message_builder.h
)

set(KADEMLIALIB_SOURCES
        src/node.cpp
        src/routing_table.cpp
        src/utils.cpp
)

option(BUILD_TEST_APP "Enables build of an executable for testing purposes" ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

qt_add_library(kademliaLib STATIC
        ${KADEMLIALIB_SOURCES}
        ${KADEMLIALIB_HEADERS}
)

add_subdirectory(proto)

target_link_libraries(kademliaLib PRIVATE Qt6::Core Qt6::Network protoLib nlohmann_json fmt::fmt cryptopp::cryptopp magic_enum::magic_enum)
target_include_directories(kademliaLib PUBLIC include)

if (BUILD_TEST_APP)
    add_executable(cliTest app/main.cpp)
    target_link_libraries(cliTest PRIVATE kademliaLib Qt6::Core Qt6::Network protoLib nlohmann_json fmt::fmt cryptopp::cryptopp magic_enum::magic_enum)
endif ()

add_custom_command(
        TARGET cliTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/conf"
        "$<TARGET_FILE_DIR:cliTest>/conf"
)

