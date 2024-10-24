
#include <functional>
#include <QRegularExpression>

#include <core/data_from_path_extractor.hpp>
#include <core/function_wrappers.hpp>
#include <core/qmodel_utils.hpp>
#include <database/filter.hpp>
#include <database/iphoto_operator.hpp>
#include <database/photo_flags_values.hpp>

#include "photos_data_guesser.hpp"


using namespace std::placeholders;

namespace
{
    enum Roles
    {
        photoPathRole            = Qt::UserRole + 1,
        PhotoIDRole,
        SuggestedDateRole,
        SuggestedTimeRole,
    };
}

ENUM_ROLES_SETUP(Roles);


PhotosDataGuesser::PhotosDataGuesser()
    : m_db(nullptr)
{

}


void PhotosDataGuesser::setDatabase(Database::IDatabase* db)
{
    clear();

    m_db = db;
}


Database::IDatabase * PhotosDataGuesser::database() const
{
    return m_db;
}


Photo::Id PhotosDataGuesser::getId(int row) const
{
    const auto rowNo = static_cast<std::size_t>(row);
    const auto& photos = internalData();
    return photos[rowNo].photoData.getId();
}


QVariant PhotosDataGuesser::data(const QModelIndex& index, int role) const
{
    const auto rowNo = static_cast<std::size_t>(index.row());
    const auto& photos = internalData();
    const auto& photo = photos[rowNo];

    if (role == photoPathRole)
        return photo.photoData.get<Photo::Field::Path>();
    else if (role == PhotoIDRole)
        return QVariant::fromValue(photo.photoData.getId());
    else if (role == SuggestedDateRole)
        return photo.date.toString(Qt::ISODate);
    else if (role == SuggestedTimeRole)
        return photo.time.toString();
    else
        return {};
}


QHash<int, QByteArray> PhotosDataGuesser::roleNames() const
{
    const auto roles = parseRoles<Roles>();
    const QHash<int, QByteArray> rolesMap(roles.begin(), roles.end());

    return rolesMap;
}


void PhotosDataGuesser::loadData(QPromise<DataVector>&& promise)
{
    if (m_db != nullptr)
        m_db->exec([promise = std::move(promise)](Database::IBackend& backend) mutable
        {
            const Database::FilterPhotosWithFlags analyzed({ { Photo::FlagsE::ExifLoaded, PhotosAnalyzerConsts::ExifFlagVersion } });
            const Database::FilterPhotosWithTag date(Tag::Types::Date);
            const Database::FilterNotMatchingFilter noDate(date);
            const Database::GroupFilter filters = {analyzed, noDate};
            const auto ids = backend.photoOperator().getPhotos(filters);

            static const DataFromPathExtractor extractor;
            std::vector<CollectedData> photos;

            for(const Photo::Id& id: ids)
            {
                if (promise.isCanceled())
                    break;

                const auto photoData = backend.getPhotoDelta<Photo::Field::Path>(id);
                const auto tags = extractor.extract(photoData.get<Photo::Field::Path>());

                if (tags.empty() == false)
                {
                    CollectedData data;

                    auto it = tags.find(Tag::Types::Date);

                    if (it != tags.end())
                        data.date = it->second.getDate();

                    it = tags.find(Tag::Types::Time);

                    if (it != tags.end())
                        data.time = it->second.getTime();

                    data.photoData = photoData;
                    photos.push_back(data);
                }
            }

            promise.addResult(photos);
            promise.finish();
        });
}


void PhotosDataGuesser::applyRows(const QList<int>& included, AHeavyListModel::ApplyToken token)
{
    std::vector<CollectedData> photosToProcess;
    photosToProcess.reserve(included.size());

    const auto& photos = internalData();

    for (const int i: included)
        photosToProcess.push_back(photos[i]);

    m_db->exec([photosToProcess, token = std::move(token)](Database::IBackend& backend)
    {
        std::vector<Photo::DataDelta> deltasToStore;
        deltasToStore.reserve(photosToProcess.size());

        for (const auto& info: photosToProcess)
        {
            auto photoDelta = backend.getPhotoDelta<Photo::Field::Tags>(info.photoData.getId());
            auto tags = photoDelta.get<Photo::Field::Tags>();

            if (info.date.isValid())
                tags[Tag::Types::Date] = info.date;

            if (info.time.isValid())
                tags[Tag::Types::Time] = info.time;

            photoDelta.insert<Photo::Field::Tags>(tags);

            deltasToStore.push_back(photoDelta);
        }

        if (deltasToStore.empty() == false)
            backend.update(deltasToStore);
    });
}
