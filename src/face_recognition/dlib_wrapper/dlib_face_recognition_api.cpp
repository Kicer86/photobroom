
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/dnn.h>
#include <QRgb>

#include <core/ilogger.hpp>
#include <core/lazy_ptr.hpp>

#include "cnn_face_detector.hpp"
#include "face_recognition.hpp"
#include "dlib_face_recognition_api.hpp"
#include "helpers.hpp"

import broom.system;


#ifdef DLIB_USE_CUDA
#define CUDA_AVAILABLE true
#else
#define CUDA_AVAILABLE false
#endif


namespace dlib_api
{
    namespace
    {
        constexpr char predictor_5_point_model[] = "shape_predictor_5_face_landmarks.dat";
        constexpr char predictor_68_point_model[] = "shape_predictor_68_face_landmarks.dat";
        constexpr char human_face_model[] = "mmod_human_face_detector.dat";
        constexpr char face_recognition_model[] = "dlib_face_recognition_resnet_model_v1.dat";

        int dlib_cuda_devices()
        {
            int devices = 0;

            try
            {
                // get_num_devices() may throw an exception if there is no nvidia device
                devices = dlib::cuda::get_num_devices();
            }
            catch (const dlib::cuda_error& err)
            {
                std::cerr << err.what() << '\n';

                devices = 0;
            }

            return devices;
        }

        bool has_hardware_accelearion()
        {
            // if cuda was disabled during dlib build then get_num_devices() will return 1 which is not what we want
            const bool has = (CUDA_AVAILABLE ? dlib_cuda_devices() : 0) > 0;

            return has;
        }

        QString rectToString(const QRect& rect)
        {
            return QString("%1,%2 (%3x%4)")
                    .arg(rect.left())
                    .arg(rect.top())
                    .arg(rect.width())
                    .arg(rect.height());
        }

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

        template<const char* name>
        QString modelPath()
        {
            const QString full_path = models_path() + "/" + name;

            return full_path;
        }

        template<typename T, const char* model>
        struct ObjectDeserializer
        {
            T operator()() const
            {
                const QString model_path = modelPath<model>();

                return deserialize_from_file<T>(model_path);
            }
        };

        QRect dlib_rect_to_qrect(const dlib::rectangle& rect)
        {
            const QRect qrect(static_cast<int>(rect.left()),
                              static_cast<int>(rect.top()),
                              static_cast<int>(rect.right() - rect.left()),
                              static_cast<int>(rect.bottom() - rect.top()));

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
            const auto cnn_face_detection_model = modelPath<human_face_model>();
            return new cnn_face_detection_model_v1(cnn_face_detection_model.toStdString());
        }
    }


    struct FaceLocator::Data
    {
        lazy_ptr<cnn_face_detection_model_v1> cnn_face_detector;
        lazy_ptr<dlib::frontal_face_detector> hog_face_detector;
        std::unique_ptr<ILogger> logger;
        const bool cuda_available;

        explicit Data(ILogger* l, bool ca)
            : cnn_face_detector(&construct_cnn_face_detector)
            , hog_face_detector(&dlib::get_frontal_face_detector)
            , logger(l->subLogger("FaceLocator"))
            , cuda_available(ca)
        {

        }
    };


    FaceLocator::FaceLocator(ILogger* logger):
        m_data(std::make_unique<Data>(logger, has_hardware_accelearion()))
    {
        if (m_data->cuda_available == false)
            m_data->logger->warning("No CUDA devices. Hardware acceleration disabled.");
    }


    FaceLocator::~FaceLocator()
    {

    }


    QVector<QRect> FaceLocator::face_locations(const QImage& qimage, int number_of_times_to_upsample)
    {
        std::optional<QVector<QRect>> faces;

        // when there are no cuda devices, cnn will perform poorly so do not use it
        if (m_data->cuda_available)
        {
            // Use cnn by default as it is fast and most accurate.
            // However it may fail (returned optional will be empty)
            m_data->logger->debug(QString("Looking for faces with cnn in image of size %1x%2")
                .arg(qimage.width())
                .arg(qimage.height()));

            faces = _face_locations_cnn(qimage, number_of_times_to_upsample);

            if (faces.has_value() == false)
                m_data->logger->debug("Image too big for cnn");
        }

        if (faces.has_value() == false)
        {
            m_data->logger->debug(QString("Looking for faces with hog in image of size %1x%2")
                .arg(qimage.width())
                .arg(qimage.height()));

            faces = _face_locations_hog(qimage, number_of_times_to_upsample);

            // use faces found by hog to retry cnn search for more accurate results
            if (faces.has_value())
            {
                // when cuda_available is false, cnn is slow, but faces are small so we cal live with that
                m_data->logger->debug(QString("Found %1 face(s). Trying cnn to improve faces positions").arg(faces->size()));

                for(QRect& face: faces.value())
                {
                    m_data->logger->debug(QString("Trying cnn for face %1")
                                .arg(rectToString(face)));

                    auto cnn_faces = _face_locations_cnn(qimage, face);

                    if (cnn_faces.has_value() == false)
                        m_data->logger->debug("Face too big for cnn");
                    else if (cnn_faces->size() == 1)
                    {
                        // replace hog face with cnn face
                        face = cnn_faces->front();

                        m_data->logger->debug(QString("Improved face position to %1")
                                .arg(rectToString(face)));
                    }
                }
            }
        }

        QVector<QRect> facesFound = faces.has_value()? faces.value(): QVector<QRect>();
        m_data->logger->debug(QString("Found %1 face(s)").arg(facesFound.size()));

        return facesFound;
    }


    QVector<QRect> FaceLocator::face_locations_cnn(const QImage& qimage, int number_of_times_to_upsample)
    {
        const auto dlib_results = m_data->cnn_face_detector->detect(qimage, number_of_times_to_upsample);
        const auto faces = dlib_rects_to_qrects(dlib_results);

        return faces;
    }


    QVector<QRect> FaceLocator::face_locations_hog(const QImage& qimage, int number_of_times_to_upsample)
    {
        dlib::matrix<dlib::rgb_pixel> image = qimage_to_dlib_matrix(qimage);

        const auto dlib_results = (*m_data->hog_face_detector)(image, number_of_times_to_upsample);
        const QVector<QRect> faces = dlib_rects_to_qrects(dlib_results);

        return faces;
    }


    std::optional<QVector<QRect>> FaceLocator::_face_locations_cnn(const QImage& qimage, int number_of_times_to_upsample)
    {
        std::optional<QVector<QRect>> faces;

        try
        {
            faces = face_locations_cnn(qimage, number_of_times_to_upsample);
        }
        catch(const dlib::cuda_error &)
        {
#ifdef DLIB_HAS_CUDA_MEMORY_OVERFLOW_BUG
            // image was too big for being processed
            // due to an issue in dlib, we just need to call face_locations_cnn here again
            QImage empty_image(10, 10, QImage::Format_Mono);
            try
            {
                face_locations_cnn(empty_image, 0);
            }
            catch(const dlib::cuda_error &)
            {
                // we will end up here as long as https://github.com/davisking/dlib/issues/1984 exists
                // covered by learning tests
            }
#endif
        }

        return faces;
    }


    std::optional<QVector<QRect>> FaceLocator::_face_locations_cnn(const QImage& image, const QRect& rect)
    {
        // enlarge original rect by some margins so we won't miss face
        const int width = rect.width();
        const int height = rect.height();
        const int horizontalMargin = static_cast<int>(width * .2);
        const int verticalMargin = static_cast<int>(height * .2);

        const QPoint origin(rect.left() - horizontalMargin, rect.top() - verticalMargin);
        const QSize faceWithMarginsSize(width + horizontalMargin * 2, height + verticalMargin *2);
        const QRect rectWithMargins(origin, faceWithMarginsSize);
        const QImage imageWithMargins = image.copy(rectWithMargins);

        std::optional< QVector< QRect > > faces;
        for (int upsample = 0; upsample < 3; upsample++)
        {
            faces = _face_locations_cnn(imageWithMargins, upsample);

            if (faces.has_value())
            {
                // there can be 0 if cnn failed to find face(should not happend) or more than 1 (due to margins possibly)
                // at this moment only one face is being handled
                if (faces->size() == 1)
                {
                    faces->front().translate(origin);
                    break;
                }
                else
                    faces.reset();
            }
        }

        return faces;
    }


    std::optional<QVector<QRect>> FaceLocator::_face_locations_hog(const QImage& qimage, int number_of_times_to_upsample)
    {
        return face_locations_hog(qimage, number_of_times_to_upsample);
    }


    struct FaceEncoder::Data
    {
        Data(ILogger* log)
            : face_encoder( modelPath<face_recognition_model>().toStdString() )
            , predictor_5_point(ObjectDeserializer<dlib::shape_predictor, predictor_5_point_model>())
            , predictor_68_point(ObjectDeserializer<dlib::shape_predictor, predictor_68_point_model>())
            , logger(log)
        {
        }

        face_recognition_model_v1 face_encoder;

        lazy_ptr<dlib::shape_predictor> predictor_5_point;
        lazy_ptr<dlib::shape_predictor> predictor_68_point;

        ILogger* logger;
    };


    FaceEncoder::FaceEncoder(ILogger* logger)
        : m_data(std::make_unique<Data>(logger))
    {
    }


    FaceEncoder::~FaceEncoder()
    {

    }


    FaceEncodings FaceEncoder::face_encodings(const QImage& qimage, int num_jitters, EncodingsModel model)
    {
        // here we assume, that given image is a face extracted from image with help of face_locations()
        const QSize size = qimage.size();

        m_data->logger->debug(
            QString("Calculating encodings for face of size %1x%2")
                .arg(size.width())
                .arg(size.height())
        );

        const dlib::rectangle face_location(0, 0, size.width() - 1 , size.height() -1);
        const dlib::shape_predictor& pose_predictor = model == Large?
                                                      *m_data->predictor_68_point :
                                                      *m_data->predictor_5_point;

        const auto image = qimage_to_dlib_matrix(qimage);
        const auto object_detection = pose_predictor(image, face_location);

        FaceEncodings result;

        try
        {
            const auto encodings = m_data->face_encoder.compute_face_descriptor(qimage, object_detection, num_jitters);
            result = FaceEncodings(encodings.begin(), encodings.end());
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


    bool check_system_prerequisites()
    {
        if (CUDA_AVAILABLE && dlib_cuda_devices() == 0)  // dlib built with cuda, bo no cuda devices? dlib would crash
            return false;
        else                                             // any other combination is fine
            return true;
    }

}
