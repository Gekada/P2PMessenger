import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import P2PMessenger 1.0


ApplicationWindow {
    id: root

    height: 720
    visible: true
    width: 1280

    StackView{
        id: views
        anchors.fill: parent

        initialItem: loginView
    }

    Connections{
        target: viewController

        function onChangeView(viewType){
            views.pop();
            views.push(mapView(viewType));
        }
    }

    Component{
        id: loginView

        LoginView{

        }
    }

    Component{
        id: chatView

        ChatView{

        }
    }

    function mapView(viewType){
        switch(viewType){
            case ViewTypes.ViewType.AuthorizationView:
                return loginView;
            case ViewTypes.ViewType.ChatView:
                return chatView;
        }
        return null;
    }

    onClosing: {
        serverConnection.closeConnection();
    }
}

