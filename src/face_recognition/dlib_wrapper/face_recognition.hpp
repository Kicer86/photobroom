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

/**
 * @brief tool for face recognition
 * based on:
 * https://github.com/davisking/dlib/blob/f7f6f6761817f2e6e5cf10ae4235fc5742779808/tools/python/src/face_recognition.cpp
 */

namespace dlib_api
{

    class face_recognition_model_v1
    {

    public:

        face_recognition_model_v1(const std::string& model_filename);

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

        dlib::rand rnd;

        std::vector<dlib::matrix<dlib::rgb_pixel>> jitter_image(
            const dlib::matrix<dlib::rgb_pixel>& img,
            const int num_jitters
        );


        template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
        using residual = dlib::add_prev1<block<N,BN,1,dlib::tag1<SUBNET>>>;

        template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
        using residual_down = dlib::add_prev2<dlib::avg_pool<2,2,2,2,dlib::skip1<dlib::tag2<block<N,BN,2,dlib::tag1<SUBNET>>>>>>;

        template <int N, template <typename> class BN, int stride, typename SUBNET>
        using block  = BN<dlib::con<N,3,3,1,1,dlib::relu<BN<dlib::con<N,3,3,stride,stride,SUBNET>>>>>;

        template <int N, typename SUBNET> using ares      = dlib::relu<residual<block,N,dlib::affine,SUBNET>>;
        template <int N, typename SUBNET> using ares_down = dlib::relu<residual_down<block,N,dlib::affine,SUBNET>>;

        template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
        template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
        template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
        template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
        template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

        using anet_type = dlib::loss_metric<dlib::fc_no_bias<128,dlib::avg_pool_everything<
                                            alevel0<
                                            alevel1<
                                            alevel2<
                                            alevel3<
                                            alevel4<
                                            dlib::max_pool<3,3,2,2,dlib::relu<dlib::affine<dlib::con<32,7,7,2,2,
                                            dlib::input_rgb_image_sized<150>
                                            >>>>>>>>>>>>;
        anet_type net;
    };
}


#endif // FACE_RECOGNITION_MODEL_V1_HPP
