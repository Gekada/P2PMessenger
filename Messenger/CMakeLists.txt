set(QML_FILES
        ui/Main.qml
        ui/Authorization/LoginView.qml
        ui/Chat/ChatView.qml
)

set(LIBS
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Network
        Qt6::Quick
)

set(MESSENGERLIB_HEADERS
        include/AuthorizationHandler.h
        include/ServerConnection.h
        include/IRequest.h
        include/LoginRequest.h
        include/ViewController.h
        include/ResponseDto.h
        include/ViewTypes.h
)

set(MESSENGERLIB_SOURCES
        src/AuthorizationHandler.cpp
        src/ServerConnection.cpp
        src/LoginRequest.cpp
        src/ViewController.cpp
        src/ResponseDto.cpp
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