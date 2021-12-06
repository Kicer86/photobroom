
#ifndef MEDIAITEM_HPP
#define MEDIAITEM_HPP

#include <QQuickPaintedItem>
#include <database/photo_data.hpp>

class AMediaItem: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Status status READ state NOTIFY stateChanged)
    Q_PROPERTY(Photo::Id photoID WRITE setSource READ source NOTIFY sourceChanged)
    Q_ENUMS(Status)

public:
    enum class Status
    {
        NotFetched,
        Fetching,
        Fetched
    };

    AMediaItem(QQuickItem* parent = nullptr);

    void setSource(const Photo::Id &);

    Status state() const;
    const Photo::Id& source() const;

protected:
    void setState(Status);

private:
    Status m_state;
    Photo::Id m_id;

signals:
    void stateChanged();
    void sourceChanged(const Photo::Id &);
};

#endif
