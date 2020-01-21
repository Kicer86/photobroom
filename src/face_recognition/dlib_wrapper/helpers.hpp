
#ifndef HELPERS_HPP_INCLUDED
#define HELPERS_HPP_INCLUDED

#include <dlib/matrix.h>

class QImage;

dlib::matrix<dlib::rgb_pixel> qimage_to_dlib_matrix(const QImage& qimage);


#endif // HELPERS_HPP_INCLUDED
