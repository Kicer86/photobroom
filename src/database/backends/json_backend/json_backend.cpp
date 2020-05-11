
#include <QFileInfo>

#include "json_backend.hpp"
#include "database/project_info.hpp"


namespace Database
{

    bool JsonBackend::addPhotos(std::vector<Photo::DataDelta>& photos)
    {

    }


    bool JsonBackend::update(const Photo::DataDelta& delta)
    {

    }


    std::vector<TagTypeInfo> JsonBackend::listTags()
    {

    }


    std::vector<TagValue> JsonBackend::listTagValues(const TagTypes &, const std::vector<IFilter::Ptr> &)
    {

    }


    Photo::Data JsonBackend::getPhoto(const Photo::Id &)
    {

    }


    int JsonBackend::getPhotosCount(const std::vector<IFilter::Ptr> &)
    {

    }


    void JsonBackend::set(const Photo::Id &id, const QString& name, int value)
    {

    }


    std::optional<int> JsonBackend::get(const Photo::Id& id, const QString& name)
    {

    }


    std::vector<Photo::Id> JsonBackend::markStagedAsReviewed()
    {

    }


    BackendStatus JsonBackend::init(const ProjectInfo& prjInfo)
    {
        BackendStatus status;

        const QFileInfo db_file_info(prjInfo.databaseLocation);

        if (prjInfo.backendName == "Json" &&
            db_file_info.exists()         &&
            db_file_info.isReadable()     &&
            db_file_info.isWritable())
        {

        }
        else
            status = StatusCodes::OpenFailed;

        return status;
    }


    void JsonBackend::closeConnections()
    {

    }


    IGroupOperator& JsonBackend::groupOperator()
    {

    }


    IPhotoOperator& JsonBackend::photoOperator()
    {

    }


    IPhotoChangeLogOperator& JsonBackend::photoChangeLogOperator()
    {

    }


    IPeopleInformationAccessor& JsonBackend::peopleInformationAccessor()
    {

    }

}
