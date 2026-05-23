set(QML_FILES
        ui/Main.qml
)

set(LIBS
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Network
        Qt6::Quick
        Qt6::Sql
        kademliaLib
        protoLib
)

set(MESSENGERLIB_HEADERS
        include/chat_controller.h

)

set(MESSENGERLIB_SOURCES
        src/chat_controller.cpp
)

set(MESSENGER_SOURCES
        app/main.cpp)

qt_add_library(messengerLib STATIC
        ${MESSENGERLIB_SOURCES}
        ${MESSENGERLIB_HEADERS}
)

target_include_directories(messengerLib PUBLIC include)

target_link_libraries(messengerLib PUBLIC ${LIBS})

qt_add_executable(${PROJECT_NAME} ${MESSENGER_SOURCES})

qt_add_qml_module(${PROJECT_NAME}
        URI P2PMessenger
        VERSION 1.0
        QML_FILES
        ${QML_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE messengerLib)

add_custom_command(
        TARGET p2pMessenger POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "../Kademlia/conf"
        "$<TARGET_FILE_DIR:p2pMessenger>/conf"
)