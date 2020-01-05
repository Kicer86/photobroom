
#ifndef HELPERS_HPP_INCLUDED
#define HELPERS_HPP_INCLUDED

#include <dlib/matrix.h>
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
            matrix(r, c) = dlib::rgb_pixel(qRed(rgb), qGreen(rgb), qBlue(rgb));
        }

    return matrix;
}


#endif // HELPERS_HPP_INCLUDED
