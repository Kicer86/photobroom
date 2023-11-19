
#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QAbstractListModel>

#include <core/function_wrappers.hpp>
#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>
#include <database/person_data.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_editor.hpp"

class FacesModel: public QAbstractListModel
{
        Q_OBJECT

        Q_PROPERTY(Photo::Id photoID MEMBER m_id REQUIRED)
        Q_PROPERTY(Database::IDatabase* database WRITE setDatabase READ database REQUIRED)
        Q_PROPERTY(ICoreFactoryAccessor* core MEMBER m_core REQUIRED)
        Q_PROPERTY(int state READ state NOTIFY stateChanged)
        Q_PROPERTY(QList<QVariant> facesMask READ facesMask NOTIFY facesMaskChanged)

    public:
        enum Roles
        {
            FaceRectRole = Qt::UserRole + 1,
            UncertainRole,
        };

        Q_ENUMS(Roles)

        FacesModel(QObject* parent = nullptr);
        ~FacesModel();

        void setDatabase(Database::IDatabase *);
        Database::IDatabase* database();

        int state() const;
        QList<QVariant> facesMask() const;

        // QAbstractItemModel:
        int rowCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;
        bool setData(const QModelIndex &, const QVariant &, int role) override;

    private:
        Photo::Id m_id;
        Database::IDatabase* m_database = nullptr;
        ICoreFactoryAccessor* m_core = nullptr;
        std::vector<std::unique_ptr<IFace>> m_faces;
        std::vector<bool> m_isUncertain;
        QSize m_photoSize;
        int m_state = 0;

        void updateFaceInformation(std::shared_ptr<std::vector<std::unique_ptr<IFace>>>);

        void initialSetup();
        void updateDetectionState(int);

        void apply();

    signals:
        void stateChanged(int) const;
        void facesMaskChanged(const QList<QVariant> &) const;
};

#endif // FACES_DIALOG_HPP
