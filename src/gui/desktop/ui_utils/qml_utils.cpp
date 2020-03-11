
#include "qml_utils.hpp"

#include <QQuickWidget>
#include <QQuickItem>

namespace QmlUtils
{
    QObject* findQmlObject(QQuickWidget* qml, const QString& objectName)
    {
        auto rootObject = qml->rootObject();
        return rootObject->findChild<QObject*>(objectName);
    }
}
