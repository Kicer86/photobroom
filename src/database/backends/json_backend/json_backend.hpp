
#ifndef JSONBACKEND_HPP
#define JSONBACKEND_HPP

#include "database/ibackend.hpp"

namespace Database
{
    /**
    * \brief json based backend
    */
    class JsonBackend: public IBackend
    {
        public:
            // IBackend interface
            bool addPhotos(std::vector<Photo::DataDelta>& photos) override;
            bool update(const Photo::DataDelta& delta) override;
            std::vector<TagTypeInfo> listTags() override;
            std::vector<TagValue> listTagValues(const TagTypes &, const std::vector<IFilter::Ptr> &) override;
            Photo::Data getPhoto(const Photo::Id &) override;
            int getPhotosCount(const std::vector<IFilter::Ptr> &) override;
            void set(const Photo::Id& id, const QString& name, int value) override;
            std::optional<int> get(const Photo::Id& id, const QString& name) override;
            std::vector<Photo::Id> markStagedAsReviewed() override;
            BackendStatus init(const ProjectInfo &) override;
            void closeConnections() override;
            IGroupOperator& groupOperator() override;
            IPhotoOperator& photoOperator() override;
            IPhotoChangeLogOperator& photoChangeLogOperator() override;
            IPeopleInformationAccessor& peopleInformationAccessor() override;
    };
}

#endif // JSONBACKEND_HPP
