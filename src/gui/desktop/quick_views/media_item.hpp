
#ifndef MEDIAITEM_HPP
#define MEDIAITEM_HPP

#include <QQuickPaintedItem>


class MediaItem: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_ENUMS(State)

public:
    enum class State
    {
        NotFetched,
        Fetching,
        Fetched
    };

    MediaItem(QQuickItem* parent = nullptr);

    State state() const;

protected:
    void setState(State);

private:
    State m_state;

signals:
    void stateChanged();
};

#endif
