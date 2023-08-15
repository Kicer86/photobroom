
#include "helpers.hpp"

#include <QImage>
#include <QRect>
#include <QRgb>


dlib::matrix<dlib::rgb_pixel> qimage_to_dlib_matrix(const QImage& qimage)
{
    dlib::matrix<dlib::rgb_pixel> matrix;

    const QRect size = qimage.rect();
    matrix.set_size(size.height(), size.width());

    for(int r = 0; r < size.height(); r++)
        for(int c = 0; c < size.width(); c++)
        {
            const QRgb rgb = qimage.pixel(c, r);
            matrix(r, c) = dlib::rgb_pixel(static_cast<unsigned char>(qRed(rgb)),
                                           static_cast<unsigned char>(qGreen(rgb)),
                                           static_cast<unsigned char>(qBlue(rgb)));
        }

    return matrix;
}
