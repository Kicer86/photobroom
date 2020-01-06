/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FACE_RECOGNITION_MODEL_V1_HPP
#define FACE_RECOGNITION_MODEL_V1_HPP

#include <string>
#include <dlib/matrix.h>
#include <dlib/dnn.h>

class QImage;

namespace dlib_api
{
    /**
    * @brief tool for face recognition
    * based on:
    * https://github.com/davisking/dlib/blob/f7f6f6761817f2e6e5cf10ae4235fc5742779808/tools/python/src/face_recognition.cpp
    */
    class face_recognition_model_v1
    {

    public:

        face_recognition_model_v1(const std::string& model_filename);
        ~face_recognition_model_v1();

        dlib::matrix<double,0,1> compute_face_descriptor (
            const QImage& img,
            const dlib::full_object_detection& face,
            const int num_jitters,
            float padding = 0.25
        );

        dlib::matrix<double,0,1> compute_face_descriptor_from_aligned_image (
            const QImage& img,
            const int num_jitters
        );

        std::vector<dlib::matrix<double,0,1>> compute_face_descriptors (
            const QImage& img,
            const std::vector<dlib::full_object_detection>& faces,
            const int num_jitters,
            float padding = 0.25
        );

        std::vector<std::vector<dlib::matrix<double,0,1>>> batch_compute_face_descriptors (
            const std::vector<QImage>& batch_imgs,
            const std::vector<std::vector<dlib::full_object_detection>>& batch_faces,
            const int num_jitters,
            float padding = 0.25
        );

        std::vector<dlib::matrix<double,0,1>> batch_compute_face_descriptors_from_aligned_images (
            const std::vector<QImage>& batch_imgs,
            const int num_jitters
        );

    private:

        struct data;
        std::unique_ptr<data> m_data;

        std::vector<dlib::matrix<dlib::rgb_pixel>> jitter_image(
            const dlib::matrix<dlib::rgb_pixel>& img,
            const int num_jitters
        );

    };
}


#endif // FACE_RECOGNITION_MODEL_V1_HPP
