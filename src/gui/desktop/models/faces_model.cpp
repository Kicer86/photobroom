
#include <cassert>

#include <core/down_cast.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/image_tools.hpp>
#include <core/model_compositor.hpp>
#include <core/qmodel_utils.hpp>
#include <database/photo_data.hpp>
#include <project_utils/project.hpp>

#include "utils/people_list_model.hpp"
#include "utils/qml_utils.hpp"

#include "faces_model.hpp"


using namespace std::placeholders;

ENUM_ROLES_SETUP(FacesModel::Roles);


FacesModel::FacesModel(QObject *parent):
    QAbstractListModel(parent),
    m_id(),
    m_peopleManipulator(),
    m_faces()
{
    QMetaObject::invokeMethod(this, &FacesModel::initialSetup, Qt::QueuedConnection);
}


int FacesModel::state() const
{
    return m_state;
}


QList<QVariant> FacesModel::facesMask() const
{
    if (m_photoSize.isValid())
    {
        QRegion reg(0, 0, m_photoSize.width(), m_photoSize.height());

        for (const QRect& rect: m_faces)
            reg-=QRegion(rect);

        const QList<QVariant> qmlListOfRects(reg.begin(), reg.end());

        return qmlListOfRects;
    }
    else
        return {};
}


int FacesModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() == false? static_cast<int>(m_facesCount): 0;
}


QVariant FacesModel::data(const QModelIndex& index, int role) const
{
    const std::size_t row = static_cast<std::size_t>(index.row());

    if (index.column() == 0 && row < m_peopleManipulator->facesCount())
    {
        if (role == Qt::DisplayRole)
            return m_peopleManipulator->name(row);
        else if (role == Roles::FaceRectRole)
            return m_peopleManipulator->position(row);
    }

    return {};
}


QHash<int, QByteArray> FacesModel::roleNames() const
{
    RETURN_MODEL_ROLES(QAbstractListModel, Roles);
}


void FacesModel::updateFaceInformation()
{
    const auto faces_count = m_peopleManipulator->facesCount();

    updateDetectionState(faces_count == 0? 2: 1);

    m_faces.clear();
    for(std::size_t i = 0; i < faces_count; i++)
        m_faces.push_back(m_peopleManipulator->position(i));

    updatePeopleList();

    for(std::size_t i = 0; i < faces_count; i++)
    {
        const QRect& pos = m_peopleManipulator->position(i);
        const QString& name = m_peopleManipulator->name(i);

        applyFaceName(pos, name);
    }

    if (faces_count > 0)
    {
        beginInsertRows(QModelIndex(), 0, faces_count - 1);
        m_facesCount = m_peopleManipulator->facesCount();
        endInsertRows();
    }

    m_photoSize = m_peopleManipulator->photoSize();

    emit facesMaskChanged(facesMask());
}


void FacesModel::applyFaceName(const QRect& face, const PersonName& person)
{
    const QString name = person.name();

    auto it = std::find(m_faces.cbegin(), m_faces.cend(), face);

    if (it != m_faces.cend())
    {
        const auto idx = std::distance(m_faces.cbegin(), it);
        //ui->peopleList->setItem(static_cast<int>(idx), 0, new QTableWidgetItem(name));
    }
}


void FacesModel::updatePeopleList()
{
    /*
    const int rowCount = ui->peopleList->rowCount();
    const int peopleCount = static_cast<int>(m_faces.size());

    if (rowCount < peopleCount)
        ui->peopleList->setRowCount(peopleCount);
    */
}


void FacesModel::selectFace()
{
    QRect selectionArea(QPoint(), m_photoSize);

    /*
    const auto selected = ui->peopleList->selectedItems();

    if (selected.empty() == false)
    {
        const auto item = selected.front();
        const int row = item->row();
        selectionArea = m_faces[row];
    }

    if (selected.empty())
        QMetaObject::invokeMethod(ui->quickView->rootObject(), "clearFaceSelection", Qt::QueuedConnection);
    else
        QMetaObject::invokeMethod(ui->quickView->rootObject(), "selectFace", Qt::QueuedConnection, Q_ARG(QVariant, selectionArea));
    */
}


void FacesModel::initialSetup()
{
    assert(m_id.valid());
    assert(m_database);
    assert(m_core);
    assert(m_peopleManipulator.get() == nullptr);

    m_peopleManipulator = std::make_unique<PeopleManipulator>(m_id, *m_database, *m_core);
    connect(m_peopleManipulator.get(), &PeopleManipulator::facesAnalyzed,
            this, &FacesModel::updateFaceInformation);

    updateDetectionState(0);
}


void FacesModel::updateDetectionState(int state)
{
    emit stateChanged(state);
}


void FacesModel::apply()
{
    const int known_count = 0; //TODO: ui->peopleList->rowCount();
    assert( known_count == m_faces.size());

    for(int i = 0; i < known_count; i++)
    {
        //const auto person = nullptr; //TODO: ui->peopleList->item(i, 0);
        //const QString name = person == nullptr? QString(): person->text();

        //m_peopleManipulator->setName(static_cast<std::size_t>(i), name);
    }

    m_peopleManipulator->store();
}
