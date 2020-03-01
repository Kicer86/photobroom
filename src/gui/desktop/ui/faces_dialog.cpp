
#include "faces_dialog.hpp"

#include <cassert>

#include <QDrag>
#include <QLineEdit>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QStyledItemDelegate>

#include <core/down_cast.hpp>
#include <core/iexif_reader.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/image_tools.hpp>
#include <database/photo_data.hpp>
#include <project_utils/project.hpp>

#include "ui_faces_dialog.h"
#include "ui_utils/icompleter_factory.hpp"

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
    ui->peopleList->setItemDelegate(new TableDelegate(completerFactory, this));
    ui->unassignedList->setItemDelegate(new TableDelegate(completerFactory, this));

    setUnassignedVisible(false);

    qRegisterMetaType<QVector<QRect>>("QVector<QRect>");

    connect(ui->scaleSlider, &QSlider::valueChanged,
            this, &FacesDialog::updateImage);

    connect(&m_peopleManipulator, &PeopleManipulator::facesAnalyzed,
            this, &FacesDialog::updateFaceInformation);

    connect(this, &FacesDialog::accepted,
            this, &FacesDialog::apply);

    ui->statusLabel->setText(tr("Locating faces..."));

    updateImage();
}


FacesDialog::~FacesDialog()
{
    delete ui;
}


void FacesDialog::updateFaceInformation()
{
    const auto faces_count = m_peopleManipulator.facesCount();
    const QString status = tr("Found %n face(s).", "", faces_count);

    ui->statusLabel->setText(status);

    m_faces.clear();
    for(std::size_t i = 0; i < faces_count; i++)
        m_faces.push_back(m_peopleManipulator.position(i));

    updateImage();
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


void FacesDialog::updateImage()
{
    const OrientedImage oriented_image = Image::normalized(m_photoPath, m_exif);

    if (oriented_image->isNull())
    {
        // TODO: display some empty image or something
    }
    else
    {
        const QSize currentSize = oriented_image->size();
        const double scale = ui->scaleSlider->value() / 100.0;

        const QSize scaledSize = currentSize * scale;
        QImage image = oriented_image->scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QVector<QRect> scaledFaces;
        for(const QRect& face: std::as_const(m_faces))
        {
            const QPoint tl = face.topLeft();
            const QSize size = face.size();
            const QRect scaledFace = QRect( tl * scale, size * scale );

            scaledFaces.append(scaledFace);
        }

        if (scaledFaces.isEmpty() == false)
        {
            QPainter painter(&image);

            QPen pen;
            pen.setColor(Qt::red);
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawRects(scaledFaces);

            pen.setWidth(1);
            painter.setBackground( Qt::white );
            painter.setBackgroundMode( Qt::OpaqueMode );
            painter.setPen(pen);

            for(int i = 0; i < scaledFaces.size(); i++)
            {
                const QRect& face = scaledFaces[i];
                const QPoint tl = face.topLeft();
                painter.drawText(tl, QString::number(i + 1));
            }
        }

        QPixmap new_pixmap = QPixmap::fromImage(image);
        ui->imageView->setPixmap(new_pixmap);
    }
}


void FacesDialog::updatePeopleList()
{
    const int rowCount = ui->peopleList->rowCount();
    const int peopleCount = m_faces.size();

    if (rowCount < peopleCount)
        ui->peopleList->setRowCount(peopleCount);
}


void FacesDialog::setUnassignedVisible(bool visible)
{
    ui->unassignedGroup->setVisible(visible);
}


void FacesDialog::apply()
{
    /*
    std::vector<PeopleOperator::FaceInfo> known_faces;
    QStringList unknownPeople;

    const int known_count = ui->peopleList->rowCount();
    assert( known_count == m_faces.size());

    for(int i = 0; i < known_count; i++)
    {
        const auto person = ui->peopleList->item(i, 0);
        const QString name = person == nullptr? QString(): person->text();

        PeopleOperator::FaceInfo face_info(m_faces[i], name, {});
        known_faces.push_back(face_info);
    }

    const int unknown_count = ui->unassignedList->rowCount();

    for(int i = 0; i < unknown_count; i++)
    {
        const auto person = ui->unassignedList->item(i, 0);
        const QString name = person == nullptr? QString(): person->text();

        if (name.isEmpty() == false)
            unknownPeople.append(name);
    }

    m_people.store(m_id, known_faces, unknownPeople);
    */
}
