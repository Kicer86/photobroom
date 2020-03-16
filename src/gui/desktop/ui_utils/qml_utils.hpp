
#ifndef QML_UTILS_HPP
#define QML_UTILS_HPP

#include <QString>
#include <QVariant>

class QQuickWidget;

namespace QmlUtils
{
    QObject* findQmlObject(QQuickWidget *, const QString& objectName);
    void registerObject(QQuickWidget *, const QString& objectName, QObject *);
}

#define INVOKABLE_ACCESSOR_FOR_INTERFACE(INTERFACE_NAME)        \
class QML_ ## INTERFACE_NAME: public QObject                   \
{                                                              \
    INTERFACE_NAME* m_i;                                       \
    Q_OBJECT                                                   \
public:                                                        \
    QML_ ## INTERFACE_NAME(INTERFACE_NAME* i): m_i(i) {}       \
    Q_INVOKABLE INTERFACE_NAME* get() const { return m_i; }    \
}

#endif
