set(PROTO_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Message.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SOURCE_FILES})

add_library(protoLib STATIC ${PROTO_SRCS} ${PROTO_HDRS})

target_include_directories(protoLib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(protoLib PUBLIC protobuf::libprotoc protobuf::libprotobuf)