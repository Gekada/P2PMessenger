#ifndef SIMPLESERVERFRONT_VIEWTYPES_H
#define SIMPLESERVERFRONT_VIEWTYPES_H

#include <QObject>
#include <QtQmlIntegration>

class ViewTypes : public QObject{
    Q_OBJECT
    QML_ELEMENT
public:
    enum class ViewType {
        AuthorizationView,
        ChatView,
    };
    Q_ENUM(ViewType)

};

#endif //SIMPLESERVERFRONT_VIEWTYPES_H
