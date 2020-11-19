
#include "faces_dialog.hpp"

#include <cassert>

#include <QDrag>
#include <QLineEdit>
#include <QMimeData>
#include <QPainter>
#include <QQuickItem>
#include <QStyledItemDelegate>

#include <core/down_cast.hpp>
#include <core/iexif_reader.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/image_tools.hpp>
#include <database/photo_data.hpp>
#include <project_utils/project.hpp>

#include "ui_faces_dialog.h"
#include "ui_utils/icompleter_factory.hpp"
#include "quick_views/qml_utils.hpp"

using namespace std::placeholders;

namespace
{
    class TableDelegate: public QStyledItemDelegate
    {
        public:
            TableDelegate(ICompleterFactory* completerFactory, QObject* p):
                QStyledItemDelegate(p),
                m_completerFactory(completerFactory)
            {
            }

            QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override
            {
                const QString value = index.data(Qt::EditRole).toString();
                auto completer = m_completerFactory->createPeopleCompleter();

                QLineEdit* lineEdit = new QLineEdit(value, parent);
                lineEdit->setGeometry(option.rect);
                lineEdit->show();
                lineEdit->setCompleter(completer);

                return lineEdit;
            }

        private:
            ICompleterFactory* m_completerFactory;
    };
}

FacesDialog::FacesDialog(const Photo::Data& data, ICompleterFactory* completerFactory, ICoreFactoryAccessor* coreAccessor, Project* prj, QWidget *parent):
    QDialog(parent),
    m_id(data.id),
    m_peopleManipulator(data.id, *prj->getDatabase(), *coreAccessor),
    m_faces(),
    m_photoPath(data.path),
    ui(new Ui::FacesDialog),
    m_exif(coreAccessor->getExifReaderFactory()->get())
{
    ui->setupUi(this);

    ui->quickView->setSource(QUrl("qrc:/ui/Dialogs/FacesDialog.qml"));
    ui->peopleList->setItemDelegate(new TableDelegate(completerFactory, this));
    ui->unassignedList->setItemDelegate(new TableDelegate(completerFactory, this));

    setUnassignedVisible(false);

    connect(&m_peopleManipulator, &PeopleManipulator::facesAnalyzed,
            this, &FacesDialog::updateFaceInformation);

    connect(this, &FacesDialog::accepted,
            this, &FacesDialog::apply);

    connect(ui->peopleList, &QTableWidget::cellClicked, this, &FacesDialog::selectFace);

    updateDetectionState(0);

    setImage();
}


FacesDialog::~FacesDialog()
{
    delete ui;
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


void FacesDialog::applyUnassigned(const Photo::Id &, const QStringList& unassigned)
{
    const int count = unassigned.size();
    ui->unassignedList->setRowCount(count);

    for(auto it = unassigned.begin(); it != unassigned.end(); ++it)
    {
        const std::size_t idx = std::distance(unassigned.cbegin(), it);
        ui->unassignedList->setItem(idx, 0, new QTableWidgetItem(*it));
    }

    setUnassignedVisible(count > 0);
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


void FacesDialog::selectFace(int id)
{
    QRect selectionArea( QPoint(), m_photoSize);

    if (id >= 0)
        selectionArea = m_faces[id];

    QMetaObject::invokeMethod(ui->quickView->rootObject(), "selectFace", Qt::QueuedConnection, Q_ARG(QVariant, selectionArea));
}


void FacesDialog::updateDetectionState(int state)
{
    QMetaObject::invokeMethod(ui->quickView->rootObject(), "setDetectionState", Qt::QueuedConnection, Q_ARG(QVariant, state));
}


void FacesDialog::setUnassignedVisible(bool visible)
{
    ui->unassignedGroup->setVisible(visible);
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
