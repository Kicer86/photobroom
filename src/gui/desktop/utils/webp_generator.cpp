
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

    for (const auto& image: m_frames)
    {
        QImage srcImage = image;
        bool alpha = srcImage.hasAlphaChannel();
        QImage::Format newFormat = alpha ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
        if (srcImage.format() != newFormat)
            srcImage = srcImage.convertToFormat(newFormat);

        WebPConfig config;
        WebPPicture picture;

        if (!WebPPictureInit(&picture) || !WebPConfigInit(&config))
            return {};

        config.lossless = m_lossless? 1: 0;

        picture.width = size.width();
        picture.height = size.height();
        picture.use_argb = 1;

        if (!WebPPictureAlloc(&picture))
            return {};

        if (alpha)
            WebPPictureImportRGBA(&picture, srcImage.constBits(), srcImage.bytesPerLine());
        else
            WebPPictureImportRGB(&picture, srcImage.constBits(), srcImage.bytesPerLine());

        WebPAnimEncoderAdd(enc.get(), &picture, m_delayForFrames.count(), &config);
        WebPPictureFree(&picture);
    }

    WebPData webp_data;
    WebPDataInit(&webp_data);

    WebPAnimEncoderAssemble(enc.get(), &webp_data);

    return QByteArray(reinterpret_cast<const char *>(webp_data.bytes), webp_data.size);
}
