
#include "dlib_face_recognition_api.hpp"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/matrix.h>
#include <dlib/dnn.h>
#include <QRgb>

#include <system/filesystem.hpp>

#include "cnn_face_detector.hpp"
#include "face_recognition.hpp"


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


    }


    QVector<QRect> face_locations(const QImage& qimage, int number_of_times_to_upsample, Model model)
    {
        if (model == cnn)
        {
            auto cnn_face_detection_model = models_path() + "/mmod_human_face_detector.dat";
            cnn_face_detection_model_v1 cnn_face_detector(cnn_face_detection_model.toStdString());

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


    std::vector<double> face_encodings(const QImage& qimage, int num_jitters, EncodingsModel model)
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

        const auto encodings = face_encoder.compute_face_descriptor(qimage, object_detection, num_jitters);
        const std::vector<double> result(encodings.begin(), encodings.end());

        return result;
    }
}
