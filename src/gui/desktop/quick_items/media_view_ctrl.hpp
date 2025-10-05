
#ifndef MEDIAVIEWCTRL_HPP
#define MEDIAVIEWCTRL_HPP

#include <QFuture>
#include <QQmlEngine>
#include <QObject>
#include <QUrl>

#include <core/icore_factory_accessor.hpp>
#include <core/property_awaiter.hpp>
#include <database/photo_types.hpp>


class MediaViewCtrl: public QObject
{
    Q_OBJECT
    Q_PROPERTY(ICoreFactoryAccessor* core MEMBER m_core REQUIRED NOTIFY coreChanged)
    Q_PROPERTY(Photo::Id photoID WRITE setSource READ source NOTIFY sourceChanged)
    Q_PROPERTY(QString photoIDString READ photoIDString NOTIFY photoIDStringChanged)
    Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
    Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
    Q_ENUMS(Mode)
    QML_ELEMENT

public:
    MediaViewCtrl();
    ~MediaViewCtrl();

    enum class Mode
    {
        Unknown,
        Error,
        StaticImage,
        AnimatedImage,
        EquirectangularProjectionImage,
        Video,
    };

    void setSource(const Photo::Id &);

    const Photo::Id& source() const;
    QString photoIDString() const;
    QUrl path() const;
    Mode mode() const;

signals:
    void coreChanged() const;
    void sourceChanged(const Photo::Id &) const;
    void photoIDStringChanged(const QString &) const;
    void pathChanged(const QUrl &) const;
    void modeChanged(Mode) const;

private:
    PropertyAwaiter m_initializer;
    QFuture<std::pair<QUrl, Mode>> m_pathFetchFuture;
    QUrl m_path;
    Mode m_mode = Mode::Unknown;
    ICoreFactoryAccessor* m_core = nullptr;
    Photo::Id m_id;

    void setPath(const QUrl &);
    void setMode(Mode);
    void process();
};

#endif

