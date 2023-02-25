
#include <core/itask_executor.hpp>
#include <core/task_executor_utils.hpp>

#include "batch_face_detector.hpp"


#include <QPromise>
#include <QFuture>
#include <core/function_wrappers.hpp>
namespace
{
    template<typename T, typename ObjT, typename F, typename... Args>
    requires std::is_base_of_v<QObject, ObjT>
    auto invoke_and_wait(QPointer<ObjT> object, const F& function, Args&&... args)
    {
        QPromise<T> promise;
        QFuture<T> future = promise.future();

        call_from_object_thread(object, [&promise, &function, &args...]()
        {
            promise.start();
            promise.addResult(function(args...));
            promise.finish();
        });

        future.waitForFinished();

        return future.result();
    }
}


void BatchFaceDetector::setPhotosModel(APhotoDataModel* model)
{
    m_photosModel = model;

    if (m_photosModel != nullptr)
    {
        assert(m_core != nullptr);

        auto task = std::bind(&BatchFaceDetector::processPhotos, this);
        runOn(m_core->getTaskExecutor(), task, "BatchFaceDetector::processPhotos");
    }
}


void BatchFaceDetector::setCore(ICoreFactoryAccessor* core)
{
    m_core = core;
}


APhotoDataModel* BatchFaceDetector::photosModel() const
{
    return m_photosModel;
}


ICoreFactoryAccessor* BatchFaceDetector::core() const
{
    return m_core;
}


void BatchFaceDetector::processPhotos()
{
    QPointer modelPtr(m_photosModel);

    for(;;)
    {
        const std::optional<APhotoDataModel::ExplicitDelta> data = invoke_and_wait<std::optional<APhotoDataModel::ExplicitDelta>>(modelPtr, [modelPtr]
        {
            const auto rows = modelPtr->rowCount();

            std::optional<APhotoDataModel::ExplicitDelta> d = rows > 0? modelPtr->getPhotoData(modelPtr->index(0, 0)) : std::optional<APhotoDataModel::ExplicitDelta>{};

            return d;
        });

        if (data.has_value() == false)
            break;
    }
}
