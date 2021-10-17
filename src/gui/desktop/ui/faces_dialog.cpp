
#include "faces_dialog.hpp"

#include <cassert>

#include <QCompleter>
#include <QDrag>
#include <QLineEdit>
#include <QMimeData>
#include <QPainter>
#include <QQuickItem>
#include <QStyledItemDelegate>

#include <core/down_cast.hpp>
#include <core/iexif_reader.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/model_compositor.hpp>
#include <core/image_tools.hpp>
#include <database/photo_data.hpp>
#include <project_utils/project.hpp>

#include "ui_faces_dialog.h"
#include "utils/people_list_model.hpp"
#include "quick_views/qml_utils.hpp"

using namespace std::placeholders;

namespace
{
    class TableDelegate: public QStyledItemDelegate
    {
        public:
            TableDelegate(Database::IDatabase& db, QObject* p):
                QStyledItemDelegate(p)
            {
                m_peopleModel.setDB(&db);
            }

            QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
            {
                const QString value = index.data(Qt::EditRole).toString();

                QCompleter* completer = new QCompleter;
                ModelCompositor* model_compositor = new ModelCompositor(completer);
                model_compositor->add(&m_peopleModel);

                completer->setModel(model_compositor);

                QLineEdit* lineEdit = new QLineEdit(value, parent);
                lineEdit->setGeometry(option.rect);
                lineEdit->show();
                lineEdit->setCompleter(completer);

                return lineEdit;
            }

        private:
            PeopleListModel m_peopleModel;
    };
}

FacesDialog::FacesDialog(const Photo::Data& data, ICoreFactoryAccessor* coreAccessor, Project* prj, QWidget *parent):
    QDialog(parent),
    m_id(data.id),
    m_peopleManipulator(data.id, prj->getDatabase(), *coreAccessor),
    m_faces(),
    m_photoPath(data.path),
    ui(new Ui::FacesDialog),
    m_exif(coreAccessor->getExifReaderFactory().get())
{
    ui->setupUi(this);

    ui->quickView->setSource(QUrl("qrc:/ui/Dialogs/FacesDialog.qml"));
    ui->peopleList->setItemDelegate(new TableDelegate(prj->getDatabase(), this));

    connect(&m_peopleManipulator, &PeopleManipulator::facesAnalyzed,
            this, &FacesDialog::updateFaceInformation);

    connect(this, &FacesDialog::accepted,
            this, &FacesDialog::apply);

    connect(ui->peopleList, &QTableWidget::itemSelectionChanged, this, &FacesDialog::selectFace);

    updateDetectionState(0);

    setImage();
}


FacesDialog::~FacesDialog()
{
    delete ui;
}


void FacesDialog::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->key() == Qt::Key_Escape)
    {
        keyEvent->accept();
        ui->peopleList->clearSelection();
    }
    else
        QDialog::keyPressEvent(keyEvent);
}


void FacesDialog::updateFaceInformation()
{
    const auto faces_count = m_peopleManipulator.facesCount();

    updateDetectionState(faces_count == 0? 2: 1);

    m_faces.clear();
    for(std::size_t i = 0; i < faces_count; i++)
        m_faces.push_back(m_peopleManipulator.position(i));

    updatePeopleList();

    for(std::size_t i = 0; i < faces_count; i++)
    {
        const QRect& pos = m_peopleManipulator.position(i);
        const QString& name = m_peopleManipulator.name(i);

        applyFaceName(pos, name);
    }

    // convert faces into QRegion containing all but faces
    QRegion reg(0, 0, m_photoSize.width(), m_photoSize.height());

    for (const QRect& rect: m_faces)
        reg-=QRegion(rect);

    QList<QVariant> qmlListOfRects;
    std::copy(reg.begin(), reg.end(), std::back_inserter(qmlListOfRects));

    QMetaObject::invokeMethod(ui->quickView->rootObject(), "setFacesMask", Qt::QueuedConnection, Q_ARG(QVariant, qmlListOfRects));
}


void FacesDialog::applyFaceName(const QRect& face, const PersonName& person)
{
    const QString name = person.name();

    auto it = std::find(m_faces.cbegin(), m_faces.cend(), face);

    if (it != m_faces.cend())
    {
        const long idx = std::distance(m_faces.cbegin(), it);
        ui->peopleList->setItem(idx, 0, new QTableWidgetItem(name));
    }
}


void FacesDialog::setImage()
{
    const OrientedImage oriented_image = Image::normalized(m_photoPath, m_exif);
    m_photoSize = oriented_image->size();

    if (oriented_image->isNull())
    {
        // TODO: display some empty image or something
    }
    else
    {
        QObject* photo = QmlUtils::findQmlObject(ui->quickView, "flickablePhoto");
        photo->setProperty("source", QVariant(oriented_image.get()));
        QMetaObject::invokeMethod(photo, "zoomToFit", Qt::QueuedConnection);
    }
}


void FacesDialog::updatePeopleList()
{
    const int rowCount = ui->peopleList->rowCount();
    const int peopleCount = m_faces.size();

    if (rowCount < peopleCount)
        ui->peopleList->setRowCount(peopleCount);
}


void FacesDialog::selectFace()
{
    QRect selectionArea( QPoint(), m_photoSize);

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
}


void FacesDialog::updateDetectionState(int state)
{
    QMetaObject::invokeMethod(ui->quickView->rootObject(), "setDetectionState", Qt::QueuedConnection, Q_ARG(QVariant, state));
}


void FacesDialog::apply()
{
    const int known_count = ui->peopleList->rowCount();
    assert( known_count == m_faces.size());

    for(int i = 0; i < known_count; i++)
    {
        const auto person = ui->peopleList->item(i, 0);
        const QString name = person == nullptr? QString(): person->text();

        m_peopleManipulator.setName(static_cast<std::size_t>(i), name);
    }

    m_peopleManipulator.store();
}
