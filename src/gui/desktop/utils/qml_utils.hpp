
#ifndef QML_UTILS_HPP
#define QML_UTILS_HPP

#include <QString>
#include <QVariant>
#include "utils/ithumbnails_manager.hpp"

class QQmlApplicationEngine;
class QQuickItem;
class QQuickView;
class QQuickWidget;
class QQmlPropertyMap;


namespace QmlUtils
{
    QObject* findQmlObject(QQuickWidget *, const QString& objectName);
    QObject* findQmlObject(QQmlApplicationEngine &, const QString& objectName);
    QQuickItem* findQuickItem(QQmlApplicationEngine &, const QString& objectName);
    void registerObjectProperties(QQuickWidget *, const QString& objectName, QQmlPropertyMap* properties);
    void registerImageProviders(QQmlApplicationEngine &, IThumbnailsManager &);
    void registerImageProviders(QQuickWidget *, IThumbnailsManager &);
}

#define INVOKABLE_ACCESSOR_FOR_INTERFACE(INTERFACE_NAME)          \
class QML_ ## INTERFACE_NAME: public QObject                      \
{                                                                 \
    INTERFACE_NAME* m_i;                                          \
    Q_OBJECT                                                      \
public:                                                           \
    explicit QML_ ## INTERFACE_NAME(INTERFACE_NAME* i): m_i(i) {} \
    Q_INVOKABLE INTERFACE_NAME* get() const { return m_i; }       \
}

#endif
