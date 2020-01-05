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

#ifndef CNN_FACE_DETECTION_MODEL_V1_HPP
#define CNN_FACE_DETECTION_MODEL_V1_HPP

#include <string>
#include <vector>
#include <dlib/dnn.h>

class QImage;

/**
 * @brief face detection tool
 * Based on https://github.com/davisking/dlib/blob/6b581d91f6b9b847a8163420630ef947e7cc88db/tools/python/src/cnn_face_detector.cpp
 */
class cnn_face_detection_model_v1
{

public:

    cnn_face_detection_model_v1(const std::string& model_filename);

    std::vector<dlib::mmod_rect> detect (
        const QImage& qimage,
        const int upsample_num_times
    );

private:

    template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
    template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

    template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
    template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

    using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

    net_type net;
};

#endif // CNN_FACE_DETECTION_MODEL_V1_H
