
#include <QMetaMethod>
#include "property_awaiter.hpp"


PropertyAwaiter::PropertyAwaiter(QObject *object, const std::vector<const char *>& propertiesToAwait, std::function<void()> callback)
    : QObject()
    , m_callback(callback)
{
    assert(object);

    const auto targetSlotIndex = metaObject()->indexOfMethod("propertyChanged()");
    const QMetaMethod targetSlot = metaObject()->method(targetSlotIndex);

    const QMetaObject* metaObject = object->metaObject();
    for (const auto& property: propertiesToAwait)
    {
        const int propertyIndex = metaObject->indexOfProperty(property);
        assert(propertyIndex >= 0);

        const QMetaProperty metaProperty = metaObject->property(propertyIndex);
        const QMetaMethod notifySignal = metaProperty.notifySignal();
        assert(notifySignal.isValid());

        const auto signalIndex = notifySignal.methodIndex();
        m_remainingProperties.insert(signalIndex);

        connect(object, notifySignal, this, targetSlot);
    }
}


void PropertyAwaiter::propertyChanged()
{
    const auto source = senderSignalIndex();
    m_remainingProperties.erase(source);

    if (m_remainingProperties.empty())
        m_callback();
}
