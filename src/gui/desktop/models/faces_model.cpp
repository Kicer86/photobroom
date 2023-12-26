
#include <cassert>

#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/image_tools.hpp>
#include <core/logger_factory.hpp>
#include <core/model_compositor.hpp>
#include <core/qmodel_utils.hpp>
#include <core/task_executor_utils.hpp>
#include <database/photo_data.hpp>
#include <project_utils/project.hpp>

#include "utils/people_list_model.hpp"
#include "utils/qml_utils.hpp"

#include "faces_model.hpp"


using namespace std::placeholders;

ENUM_ROLES_SETUP(FacesModel::Roles);


FacesModel::FacesModel(QObject *parent):
    QAbstractListModel(parent)
{
    QMetaObject::invokeMethod(this, &FacesModel::initialSetup, Qt::QueuedConnection);
}


FacesModel::~FacesModel()
{
    if (m_database)
        apply();
}


void FacesModel::setDatabase(Database::IDatabase* db)
{
    // flush data when db changes
    if (m_database != nullptr && m_database != db)
        apply();

    m_database = db;
}


Database::IDatabase* FacesModel::database()
{
    return m_database;
}


int FacesModel::state() const
{
    return m_state;
}


QList<QVariant> FacesModel::facesMask() const
{
    if (m_faces.empty())
        return {};
    else
    {
        const QSize photoSize = m_faces.front()->image()->size();
        QRegion reg(0, 0, photoSize.width(), photoSize.height());

        for (const auto& face: m_faces)
            reg -= QRegion(face->rect());

        const QList<QVariant> qmlListOfRects(reg.begin(), reg.end());

        return qmlListOfRects;
    }
}


int FacesModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() == false? static_cast<int>(m_faces.size()): 0;
}


QVariant FacesModel::data(const QModelIndex& index, int role) const
{
    const std::size_t row = static_cast<std::size_t>(index.row());

    if (index.column() == 0 && row < m_faces.size())
    {
        if (role == Qt::DisplayRole)
            return m_faces[row]->name();
        else if (role == Roles::FaceRectRole)
            return m_faces[row]->rect();
        else if (role == Roles::UncertainRole)
            return m_isUncertain[row];
    }

    return {};
}


QHash<int, QByteArray> FacesModel::roleNames() const
{
    RETURN_MODEL_ROLES(QAbstractListModel, Roles);
}


bool FacesModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
    const std::size_t r = static_cast<std::size_t>(index.row());
    if (role == Qt::EditRole && index.column() == 0 && r < m_faces.size())
    {
        m_faces[r]->setName(data.toString());

        if (m_isUncertain[r])
        {
            m_isUncertain[r] = false;

            emit dataChanged(index, index, {UncertainRole});
        }
    }

    return true;
}


void FacesModel::updateFaceInformation(std::shared_ptr<std::vector<std::unique_ptr<IFace>>> faces)
{
    const int faces_count = static_cast<int>(faces->size());

    updateDetectionState(faces_count == 0? 2: 1);

    m_faces.clear();

    if (faces_count > 0)
    {
        beginInsertRows({}, 0, faces_count - 1);
        m_faces = std::move(*faces);
        m_isUncertain.resize(m_faces.size());

        // perform recognition
        for (unsigned int i = 0; i < m_faces.size(); i++)
        {
            const auto& person = m_faces[i]->person();

            if (person.id().valid() == false)
            {
                m_isUncertain[i] = true;
                m_faces[i]->recognize();
            }
        }

        endInsertRows();
    }

    emit facesMaskChanged(facesMask());
}


void FacesModel::initialSetup()
{
    assert(m_id.valid());
    assert(m_database);
    assert(m_core);

    runOn(m_core->getTaskExecutor(), [this]()
    {
        const auto logger = m_core->getLoggerFactory().get("FacesModel");
        const auto faces = std::make_shared<std::vector<std::unique_ptr<IFace>>>(FaceEditor(
            *m_database,
            *m_core,
            *logger).getFacesFor(m_id));
        invokeMethod(this, &FacesModel::updateFaceInformation, faces);
    });

    updateDetectionState(0);
}


void FacesModel::updateDetectionState(int state)
{
    emit stateChanged(state);
}


void FacesModel::apply()
{
    assert(m_faces.size() == m_isUncertain.size());

    for(std::size_t i = 0; i < m_faces.size(); i++)
        if (not m_isUncertain[i])
            m_faces[i]->store();
}
