
#ifndef MEDIAVIEWCTRL_HPP
#define MEDIAVIEWCTRL_HPP

#include <QObject>
#include <QUrl>

#include <core/function_wrappers.hpp>
#include <database/photo_types.hpp>


class MediaViewCtrl: public QObject
{
    Q_OBJECT
    Q_PROPERTY(Photo::Id photoID WRITE setSource READ source NOTIFY sourceChanged)
    Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
    Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
    Q_ENUMS(Mode)

public:
    MediaViewCtrl() = default;
    ~MediaViewCtrl();

    enum class Mode
    {
        Unknown,
        StaticImage,
        AnimatedImage,
        Video,
    };

    void setSource(const Photo::Id &);

    const Photo::Id& source() const;
    QUrl path() const;
    Mode mode() const;

signals:
    void sourceChanged(const Photo::Id &) const;
    void pathChanged(const QUrl &) const;
    void modeChanged(Mode) const;

private:
    safe_callback_ctrl m_callbackCtrl;
    QUrl m_path;
    Mode m_mode;
    Photo::Id m_id;

    void setPath(const QString &);
    void setMode(Mode);
};

#endif

