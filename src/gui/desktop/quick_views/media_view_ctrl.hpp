
#ifndef MEDIAVIEWCTRL_HPP
#define MEDIAVIEWCTRL_HPP

#include <QObject>

#include <database/photo_types.hpp>


class MediaViewCtrl: public QObject
{
    Q_OBJECT
    Q_PROPERTY(Photo::Id photoID WRITE setSource READ source NOTIFY sourceChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
    Q_ENUMS(Mode)

public:
    enum class Mode
    {
        Unknown,
        StaticImage,
        AnimatedImage,
        Video,
    };

    void setSource(const Photo::Id &);

    const Photo::Id& source() const;
    QString path() const;
    Mode mode() const;

signals:
    void sourceChanged(const Photo::Id &) const;
    void pathChanged(const QString &) const;
    void modeChanged(Mode) const;

private:
    QString m_path;
    Mode m_mode;
    Photo::Id m_id;

    void setPath(const QString &);
    void setMode(Mode);
};

#endif

