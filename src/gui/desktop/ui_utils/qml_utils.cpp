
#include "qml_utils.hpp"

#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlContext>

namespace QmlUtils
{
    QObject* findQmlObject(QQuickWidget* qml, const QString& objectName)
    {
        auto rootObject = qml->rootObject();
        return rootObject->findChild<QObject*>(objectName);
    }


    void registerObject(QQuickWidget* qml, const QString& objectName, QObject* object)
    {
        auto rootContext = qml->rootContext();
        rootContext->setContextProperty(objectName, object);
    }

}
