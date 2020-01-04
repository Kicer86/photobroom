
#include "dlib_face_recognition_api.hpp"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/matrix.h>
#include <dlib/dnn.h>
#include <dlib/image_transforms.h>
#include <QRgb>

#include <system/filesystem.hpp>


namespace dlib_api
{
    namespace
    {
        QString models_path()
        {
            const QString path = FileSystem().getDataPath() + "/face_recognition_models";

            return path;
        }

        template<typename T>
        T deserialize_from_file(const QString& file_name)
        {
            T object;
            dlib::deserialize(file_name.toStdString()) >> object;

            return object;
        }

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

        QRect dlib_rect_to_qrect(const dlib::rectangle& rect)
        {
            const QRect qrect(rect.left(), rect.top(),
                              rect.right() - rect.left(), rect.bottom() - rect.top());

            return qrect;
        }

        template<typename DlibRect>
        QVector<QRect> dlib_rects_to_qrects(const std::vector<DlibRect>& dlib_rects)
        {
            QVector<QRect> qrects;
            for (const auto& rect: dlib_rects)
            {
                const QRect qrect = dlib_rect_to_qrect(rect);
                qrects.push_back(qrect);
            }

            return qrects;
        }

        // based on:
        // https://github.com/davisking/dlib/blob/6b581d91f6b9b847a8163420630ef947e7cc88db/tools/python/src/cnn_face_detector.cpp
        class cnn_face_detection_model_v1
        {

        public:

            cnn_face_detection_model_v1(const std::string& model_filename)
            {
                dlib::deserialize(model_filename) >> net;
            }

            std::vector<dlib::mmod_rect> detect (
                const QImage& qimage,
                const int upsample_num_times
            )
            {
                dlib::pyramid_down<2> pyr;
                std::vector<dlib::mmod_rect> face_rects;

                dlib::matrix<dlib::rgb_pixel> image = qimage_to_dlib_matrix(qimage);

                // Upsampling the image will allow us to detect smaller faces but will cause the
                // program to use more RAM and run longer.
                for (int i = 0; i < upsample_num_times; i++)
                    pyramid_up(image, pyr);

                auto dets = net(image);

                // Scale the detection locations back to the original image size
                // if the image was upscaled.
                for (auto&& d : dets) {
                    d.rect = pyr.rect_down(d.rect, upsample_num_times);
                    face_rects.push_back(d);
                }

                return face_rects;
            }

        private:

            template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
            template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

            template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
            template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

            using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

            net_type net;
        };

        // based on:
        // https://github.com/davisking/dlib/blob/f7f6f6761817f2e6e5cf10ae4235fc5742779808/tools/python/src/face_recognition.cpp
        class face_recognition_model_v1
        {

        public:

            face_recognition_model_v1(const std::string& model_filename)
            {
                dlib::deserialize(model_filename) >> net;
            }

            dlib::matrix<double,0,1> compute_face_descriptor (
                const QImage& img,
                const dlib::full_object_detection& face,
                const int num_jitters,
                float padding = 0.25
            )
            {
                std::vector<dlib::full_object_detection> faces(1, face);
                return compute_face_descriptors(img, faces, num_jitters, padding)[0];
            }

            dlib::matrix<double,0,1> compute_face_descriptor_from_aligned_image (
                const QImage& img,
                const int num_jitters
            )
            {
                std::vector<QImage> images{img};
                return batch_compute_face_descriptors_from_aligned_images(images, num_jitters)[0];
            }

            std::vector<dlib::matrix<double,0,1>> compute_face_descriptors (
                const QImage& img,
                const std::vector<dlib::full_object_detection>& faces,
                const int num_jitters,
                float padding = 0.25
            )
            {
                std::vector<QImage> batch_img(1, img);
                std::vector<std::vector<dlib::full_object_detection>> batch_faces(1, faces);
                return batch_compute_face_descriptors(batch_img, batch_faces, num_jitters, padding)[0];
            }

            std::vector<std::vector<dlib::matrix<double,0,1>>> batch_compute_face_descriptors (
                const std::vector<QImage>& batch_imgs,
                const std::vector<std::vector<dlib::full_object_detection>>& batch_faces,
                const int num_jitters,
                float padding = 0.25
            )
            {

                if (batch_imgs.size() != batch_faces.size())
                    throw dlib::error("The array of images and the array of array of locations must be of the same size");

                int total_chips = 0;
                for (const auto& faces : batch_faces)
                {
                    total_chips += faces.size();
                    for (const auto& f : faces)
                    {
                        if (f.num_parts() != 68 && f.num_parts() != 5)
                            throw dlib::error("The full_object_detection must use the iBUG 300W 68 point face landmark style or dlib's 5 point style.");
                    }
                }


                dlib::array<dlib::matrix<dlib::rgb_pixel>> face_chips;
                for (int i = 0; i < batch_imgs.size(); ++i)
                {
                    auto& faces = batch_faces[i];
                    auto& img = batch_imgs[i];

                    std::vector<dlib::chip_details> dets;
                    for (const auto& f : faces)
                        dets.push_back(get_face_chip_details(f, 150, padding));
                    dlib::array<dlib::matrix<dlib::rgb_pixel>> this_img_face_chips;

                    const auto image = qimage_to_dlib_matrix(img);
                    extract_image_chips(image, dets, this_img_face_chips);

                    for (auto& chip : this_img_face_chips)
                        face_chips.push_back(chip);
                }

                std::vector<std::vector<dlib::matrix<double,0,1>>> face_descriptors(batch_imgs.size());
                if (num_jitters <= 1)
                {
                    // extract descriptors and convert from float vectors to double vectors
                    auto descriptors = net(face_chips, 16);
                    auto next = std::begin(descriptors);
                    for (int i = 0; i < batch_faces.size(); ++i)
                    {
                        for (int j = 0; j < batch_faces[i].size(); ++j)
                        {
                            face_descriptors[i].push_back(dlib::matrix_cast<double>(*next++));
                        }
                    }
                    DLIB_ASSERT(next == std::end(descriptors));
                }
                else
                {
                    // extract descriptors and convert from float vectors to double vectors
                    auto fimg = std::begin(face_chips);
                    for (int i = 0; i < batch_faces.size(); ++i)
                    {
                        for (int j = 0; j < batch_faces[i].size(); ++j)
                        {
                            auto& r = mean(mat(net(jitter_image(*fimg++, num_jitters), 16)));
                            face_descriptors[i].push_back(dlib::matrix_cast<double>(r));
                        }
                    }
                    DLIB_ASSERT(fimg == std::end(face_chips));
                }

                return face_descriptors;
            }

            std::vector<dlib::matrix<double,0,1>> batch_compute_face_descriptors_from_aligned_images (
                const std::vector<QImage>& batch_imgs,
                const int num_jitters
            )
            {
                dlib::array<dlib::matrix<dlib::rgb_pixel>> face_chips;
                for (auto& img : batch_imgs) {

                    dlib::matrix<dlib::rgb_pixel> image = qimage_to_dlib_matrix(img);

                    // Check for the size of the image
                    if ((image.nr() != 150) || (image.nc() != 150)) {
                        throw dlib::error("Unsupported image size, it should be of size 150x150. Also cropping must be done as `dlib.get_face_chip` would do it. \
                        That is, centered and scaled essentially the same way.");
                    }

                    face_chips.push_back(image);
                }

                std::vector<dlib::matrix<double,0,1>> face_descriptors;
                if (num_jitters <= 1)
                {
                    // extract descriptors and convert from float vectors to double vectors
                    auto descriptors = net(face_chips, 16);

                    for (auto& des: descriptors) {
                        face_descriptors.push_back(dlib::matrix_cast<double>(des));
                    }
                }
                else
                {
                    // extract descriptors and convert from float vectors to double vectors
                    for (auto& fimg : face_chips) {
                        auto& r = mean(mat(net(jitter_image(fimg, num_jitters), 16)));
                        face_descriptors.push_back(dlib::matrix_cast<double>(r));
                    }
                }
                return face_descriptors;
            }

        private:

            dlib::rand rnd;

            std::vector<dlib::matrix<dlib::rgb_pixel>> jitter_image(
                const dlib::matrix<dlib::rgb_pixel>& img,
                const int num_jitters
            )
            {
                std::vector<dlib::matrix<dlib::rgb_pixel>> crops;
                for (int i = 0; i < num_jitters; ++i)
                    crops.push_back(dlib::jitter_image(img,rnd));
                return crops;
            }


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


    QVector<QRect> face_locations(const QImage& qimage, int number_of_times_to_upsample, Model model)
    {
        if (model == cnn)
        {
            cnn_face_detection_model_v1 cnn_face_detector("");

            const auto dlib_results = cnn_face_detector.detect(qimage, number_of_times_to_upsample);
            const QVector<QRect> faces = dlib_rects_to_qrects(dlib_results);

            return faces;
        }
        else
        {
            dlib::matrix<dlib::rgb_pixel> image = qimage_to_dlib_matrix(qimage);

            auto face_detector = dlib::get_frontal_face_detector();
            const auto dlib_results = face_detector(image, number_of_times_to_upsample);
            const QVector<QRect> faces = dlib_rects_to_qrects(dlib_results);

            return faces;
        }
    }


    QByteArray face_encodings(const QImage& qimage, EncodingsModel model)
    {
        // here we assume, that given image is a face extraceted from image with help of face_locations()
        const QSize size = qimage.size();
        const dlib::rectangle face_location(0, 0, size.width() - 1 , size.height() -1);

        auto predictor_68_point_model = models_path() + "/shape_predictor_68_face_landmarks.dat";
        auto predictor_5_point_model = models_path() + "/shape_predictor_5_face_landmarks.dat";

        dlib::shape_predictor pose_predictor = model == large?
                                               deserialize_from_file<dlib::shape_predictor>(predictor_68_point_model) :
                                               deserialize_from_file<dlib::shape_predictor>(predictor_5_point_model);

        const auto image = qimage_to_dlib_matrix(qimage);
        const auto object_detection = pose_predictor(image, face_location);

        auto face_recognition_model = models_path() + "/dlib_face_recognition_resnet_model_v1.dat";
        face_recognition_model_v1 face_encoder(face_recognition_model.toStdString());

        face_encoder.compute_face_descriptor(qimage, object_detection, 1);
    }
}
