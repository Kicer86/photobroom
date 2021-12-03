
#ifndef MEDIAVIEWCTRL_HPP
#define MEDIAVIEWCTRL_HPP

#include <QObject>

class MediaViewCtrl: public QObject
{
    Q_OBJECT
    Q_PROPERTY(Mode mode READ mode NOTIFY modeChanged)
    Q_ENUMS(Mode)

public:
    enum class Mode
    {
        Unknown,
        StaticImage,
        AnimatedImage,
    };

    Mode mode() const;

signals:
    void modeChanged(Mode) const;

private:
    Mode m_mode;
};

#endif
