
#ifndef MEDIAITEM_HPP
#define MEDIAITEM_HPP

#include <QQuickPaintedItem>
#include <database/photo_data.hpp>

class AMediaItem: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(Photo::Id photoID WRITE setSource READ source)
    Q_ENUMS(State)

public:
    enum class State
    {
        NotFetched,
        Fetching,
        Fetched
    };

    AMediaItem(QQuickItem* parent = nullptr);

    void setSource(const Photo::Id &);

    State state() const;
    const Photo::Id& source() const;

protected:
    void setState(State);

private:
    State m_state;
    Photo::Id m_id;

signals:
    void stateChanged();
};

#endif
