
#include "qml_utils.hpp"

#include <QQuickItem>
#include <QQuickView>
#include <QQuickWidget>
#include <QQmlApplicationEngine>
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


    QObject* findQmlObject(QQmlApplicationEngine& engine, const QString& objectName)
    {
        QObject* child = nullptr;

        for(auto rootObject: engine.rootObjects())
        {
            child = (rootObject->objectName() == objectName)?
                rootObject :
                rootObject->findChild<QObject*>(objectName);

            if (child != nullptr)
                break;
        }

        return child;
    }


    QQuickItem* findQuickItem(QQuickWidget* qml, const QString& objectName)
    {
        QObject* obj = findQmlObject(qml, objectName);

        return qobject_cast<QQuickItem *>(obj);
    }


    QQuickItem* findQuickItem(QQmlApplicationEngine& engine, const QString& objectName)
    {
        QObject* obj = findQmlObject(engine, objectName);

        return qobject_cast<QQuickItem *>(obj);
    }


    void registerObjectProperties(QQuickWidget* qml, const QString& objectName, QQmlPropertyMap* properties)
    {
        auto rootContext = qml->rootContext();
        rootContext->setContextProperty(objectName, properties);
    }


    void registerImageProviders(QQmlApplicationEngine& engine, IThumbnailsManager& thbMgr)
    {
        engine.addImageProvider("thumbnail", new ThumbnailImageProvider(thbMgr));
    }


    void registerImageProviders(QQuickWidget* widget, IThumbnailsManager& thbMgr)
    {
        widget->engine()->addImageProvider("thumbnail", new ThumbnailImageProvider(thbMgr));
    }
}
