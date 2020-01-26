
#include "dlib_face_recognition_api.hpp"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/dnn.h>
#include <QRgb>
#include <optional>

#include <core/lazy_ptr.hpp>
#include <system/filesystem.hpp>

#include "cnn_face_detector.hpp"
#include "face_recognition.hpp"


namespace dlib_api
{
    namespace
    {
        // helpers

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

        cnn_face_detection_model_v1* construct_cnn_face_detector()
        {
            static const auto cnn_face_detection_model = models_path() + "/mmod_human_face_detector.dat";
            return new cnn_face_detection_model_v1(cnn_face_detection_model.toStdString());
        }
    }


    struct FaceLocator::Data
    {
        lazy_ptr<cnn_face_detection_model_v1, decltype(&construct_cnn_face_detector)> cnn_face_detector;

        Data()
            : cnn_face_detector(&construct_cnn_face_detector)
        {
        }
    };


    FaceLocator::FaceLocator():
        m_data(std::make_unique<Data>())
    {

    }


    FaceLocator::~FaceLocator()
    {

    }


    QVector<QRect> FaceLocator::face_locations(const QImage& qimage, int number_of_times_to_upsample, Model model)
    {
        std::optional<QVector<QRect>> faces;

        if (model == cnn || model == automatic)
            faces = face_locations_cnn(qimage, number_of_times_to_upsample);

        if (model == hog || (model == automatic && faces.has_value() == false) )
            faces = face_locations_hog(qimage, number_of_times_to_upsample);

        return faces.has_value()? faces.value(): QVector<QRect>();
    }


    std::optional<QVector<QRect>> FaceLocator::face_locations_cnn(const QImage& qimage, int number_of_times_to_upsample)
    {
        std::optional<QVector<QRect>> faces;

        try
        {
            const auto dlib_results = m_data->cnn_face_detector->detect(qimage, number_of_times_to_upsample);
            faces = dlib_rects_to_qrects(dlib_results);
        }
        catch(const dlib::cuda_error& err)
        {
            std::cerr << err.what() << std::endl;
        }

        return faces;
    }


    QVector<QRect> FaceLocator::face_locations_hog(const QImage& qimage, int number_of_times_to_upsample)
    {
        dlib::matrix<dlib::rgb_pixel> image = qimage_to_dlib_matrix(qimage);

        static auto face_detector = dlib::get_frontal_face_detector();

        const auto dlib_results = face_detector(image, number_of_times_to_upsample);
        const QVector<QRect> faces = dlib_rects_to_qrects(dlib_results);

        return faces;
    }


    std::vector<double> face_encodings(const QImage& qimage, int num_jitters, EncodingsModel model)
    {
        // here we assume, that given image is a face extraceted from image with help of face_locations()
        const QSize size = qimage.size();
        const dlib::rectangle face_location(0, 0, size.width() - 1 , size.height() -1);

        static const auto predictor_68_point_model = models_path() + "/shape_predictor_68_face_landmarks.dat";
        static const auto predictor_5_point_model = models_path() + "/shape_predictor_5_face_landmarks.dat";
        static const auto predictor_68_point = deserialize_from_file<dlib::shape_predictor>(predictor_68_point_model);
        static const auto predictor_5_point = deserialize_from_file<dlib::shape_predictor>(predictor_5_point_model);

        const dlib::shape_predictor& pose_predictor = model == large?
                                                      predictor_68_point :
                                                      predictor_5_point;

        const auto image = qimage_to_dlib_matrix(qimage);
        const auto object_detection = pose_predictor(image, face_location);

        auto face_recognition_model = models_path() + "/dlib_face_recognition_resnet_model_v1.dat";
        face_recognition_model_v1 face_encoder(face_recognition_model.toStdString());

        std::vector<double> result;

        try
        {
            const auto encodings = face_encoder.compute_face_descriptor(qimage, object_detection, num_jitters);
            result = std::vector<double>(encodings.begin(), encodings.end());
        }
        catch(const dlib::cuda_error& err)
        {
            std::cerr << err.what() << std::endl;
        }

        return result;
    }


    std::vector<bool> compare_faces(const std::vector<FaceEncodings>& known_face_encodings, const FaceEncodings& face_encoding_to_check, double tolerance)
    {
        const std::size_t faces = known_face_encodings.size();
        const std::vector distances = face_distance(known_face_encodings, face_encoding_to_check);

        std::vector<bool> results(faces, false);

        for(std::size_t i = 0; i < faces; i++)
            results[i] = distances[i] <= tolerance;

        return results;
    }


    std::vector<double> face_distance(const std::vector<FaceEncodings>& face_encodings, const FaceEncodings& face_to_compare)
    {
        std::vector<double> results;
        results.reserve(face_encodings.size());

        for(const FaceEncodings& face_encoding: face_encodings)
        {
            const std::size_t size = std::min(face_encoding.size(), face_to_compare.size());
            assert(size == 128);

            std::vector<double> diff(size, 0.0);

            for(std::size_t i = 0; i < size; i++)
                diff[i] = face_encoding[i] - face_to_compare[i];

            // calculating 2-norm from `diff` as in original python code
            // https://docs.scipy.org/doc/numpy/reference/generated/numpy.linalg.norm.html#numpy.linalg.norm
            // https://en.wikipedia.org/wiki/Norm_(mathematics)  -> p-norm

            const double norm_squared = std::accumulate(diff.begin(), diff.end(), 0.0, [](double sum, double v)
            {
                return sum + std::fabs(v) * std::fabs(v);
            });

            const double norm = std::sqrt(norm_squared);

            results.push_back(norm);
        }

        return results;
    }
}
