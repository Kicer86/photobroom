
#ifndef PROPERTY_INITIALIZER_HPP_INCLUDED
#define PROPERTY_INITIALIZER_HPP_INCLUDED


#include <set>
#include <QObject>
#include <QStringList>

#include "core_export.h"


class CORE_EXPORT PropertyAwaiter: public QObject
{
    Q_OBJECT

public:
    explicit PropertyAwaiter(QObject* object, const std::vector<const char *>& propertiesToAwait, std::function<void()> callback);
    virtual ~PropertyAwaiter() = default;

private:
    std::function<void()> m_callback;
    std::set<int> m_remainingProperties;

private slots:
    void propertyChanged();
};

#endif
