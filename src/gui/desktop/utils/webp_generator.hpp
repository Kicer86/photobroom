
#ifndef WEBP_GENERATOR_HPP_INCLUDED
#define WEBP_GENERATOR_HPP_INCLUDED

#include <chrono>
#include <QByteArray>
#include <QImage>


class WebPGenerator
{
    public:
        WebPGenerator& append(const QImage &);
        WebPGenerator& setDelay(const std::chrono::milliseconds &);
        WebPGenerator& setLossless();
        QByteArray save();

    private:
        std::vector<QImage> m_frames;
        std::chrono::milliseconds m_delayForFrames = std::chrono::milliseconds(100);
        bool m_lossless = false;
};

#endif
