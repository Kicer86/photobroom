
#include "qml_utils.hpp"

#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlPropertyMap>

#include "quick_items/thumbnail_image_provider.hpp"


namespace QmlUtils
{
    QObject* findQmlObject(QQuickWidget* qml, const QString& objectName)
    {
        auto rootObject = qml->rootObject();
        return (rootObject->objectName() == objectName)?
            rootObject :
            rootObject->findChild<QObject*>(objectName);
    }


    QQuickItem* findQuickItem(QQuickWidget* qml, const QString& objectName)
    {
        QObject* obj = findQmlObject(qml, objectName);

        return qobject_cast<QQuickItem *>(obj);
    }


    void registerObject(QQuickWidget* qml, const QString& objectName, QObject* object)
    {
        auto rootContext = qml->rootContext();
        rootContext->setContextProperty(objectName, object);
    }


    void registerObjectProperties(QQuickWidget* qml, const QString& objectName, QQmlPropertyMap* properties)
    {
        auto rootContext = qml->rootContext();
        rootContext->setContextProperty(objectName, properties);
    }


    void registerImageProviders(QQuickWidget* widget, IThumbnailsManager& thbMgr)
    {
        widget->engine()->addImageProvider("thumbnail", new ThumbnailImageProvider(thbMgr));
    }
}
