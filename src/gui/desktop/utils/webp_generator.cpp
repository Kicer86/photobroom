
#include <webp/encode.h>
#include <webp/mux.h>

#include "webp_generator.hpp"


WebPGenerator& WebPGenerator::append(const QImage& image)
{
    m_frames.push_back(image);

    return *this;
}


WebPGenerator& WebPGenerator::setDelay(const std::chrono::milliseconds& delay)
{
    m_delayForFrames = delay;
    return *this;
}


WebPGenerator& WebPGenerator::setLoopDelay(const std::chrono::milliseconds& delay)
{
    m_delayForLoop = delay;
    return *this;
}


WebPGenerator& WebPGenerator::setLossless()
{
    m_lossless = true;
    return *this;
}


QByteArray WebPGenerator::save()
{
    // partialy based on:
    // https://code.qt.io/cgit/qt/qtimageformats.git/tree/src/plugins/imageformats/webp/qwebphandler.cpp#n206

    QSize size;

    for (const QImage& image: m_frames)
    {
        const auto imgSize = image.size();
        size.setWidth(std::max(size.width(), imgSize.width()));
        size.setHeight(std::max(size.height(), imgSize.height()));
    }

    WebPAnimEncoderOptions enc_options;
    WebPAnimEncoderOptionsInit(&enc_options);

    std::unique_ptr<WebPAnimEncoder, void(*)(WebPAnimEncoder*)> enc(WebPAnimEncoderNew(size.width(), size.height(), &enc_options), &WebPAnimEncoderDelete);

    WebPConfig config;
    if (!WebPConfigInit(&config))
        return {};

    config.lossless = m_lossless? 1: 0;

    int timestamp = 0;
    for (const auto& image: m_frames)
    {
        QImage srcImage = image;
        const bool alpha = srcImage.hasAlphaChannel();
        const QImage::Format newFormat = alpha ? QImage::Format_RGBA8888 : QImage::Format_RGB888;

        if (srcImage.format() != newFormat)
            srcImage = srcImage.convertToFormat(newFormat);

        WebPPicture picture;
        if (!WebPPictureInit(&picture))
            return {};

        picture.width = size.width();
        picture.height = size.height();
        picture.use_argb = 1;

        if (!WebPPictureAlloc(&picture))
            return {};

        if (alpha)
            WebPPictureImportRGBA(&picture, srcImage.constBits(), static_cast<int>(srcImage.bytesPerLine()));
        else
            WebPPictureImportRGB(&picture, srcImage.constBits(), static_cast<int>(srcImage.bytesPerLine()));

        WebPAnimEncoderAdd(enc.get(), &picture, timestamp, &config);
        WebPPictureFree(&picture);

        timestamp += static_cast<int>(m_delayForFrames.count());
    }

    const int lastFrameTimestamp = timestamp + static_cast<int>(m_delayForLoop.count());
    WebPAnimEncoderAdd(enc.get(), nullptr, lastFrameTimestamp, &config);

    WebPData webp_data;
    WebPDataInit(&webp_data);

    WebPAnimEncoderAssemble(enc.get(), &webp_data);

    return QByteArray(reinterpret_cast<const char *>(webp_data.bytes), static_cast<qsizetype>(webp_data.size));
}
