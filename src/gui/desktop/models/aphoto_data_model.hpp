
#ifndef APHOTO_DATA_MODEL_HPP_INCLUDED
#define APHOTO_DATA_MODEL_HPP_INCLUDED

#include <QAbstractItemModel>
#include <database/explicit_photo_delta.hpp>


class APhotoDataModel: public QAbstractItemModel
{
    public:
        using ExplicitDelta = Photo::ExplicitDelta<Photo::Field::Path, Photo::Field::Flags, Photo::Field::GroupInfo>;
        enum Roles
        {
            PhotoDataRole = Qt::UserRole + 1,
            PhotoIdRole,
            _lastRole,
        };

        APhotoDataModel(QObject * = nullptr);
        APhotoDataModel(const APhotoDataModel &) = delete;
        ~APhotoDataModel();

        APhotoDataModel& operator=(const APhotoDataModel &) = delete;

        virtual const ExplicitDelta& getPhotoData(const QModelIndex &) const = 0;
        virtual QHash<int, QByteArray> roleNames() const override;

    protected:
        QHash<int, QByteArray> m_customRoles;

        void registerRole(int, const QByteArray &);
};

#endif
