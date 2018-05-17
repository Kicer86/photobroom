
#include "faces_dialog.hpp"

#include <cassert>

#include <QDrag>
#include <QLineEdit>
#include <QMimeData>
#include <QPainter>
#include <QStyledItemDelegate>

#include <core/down_cast.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ipython_thread.hpp>
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
                QLineEdit* lineEdit = new QLineEdit(value, parent);
                lineEdit->setGeometry(option.rect);
                lineEdit->show();
                const TagNameInfo peopleTag(BaseTagsList::People);
                auto completer = m_completerFactory->createCompleter(peopleTag);
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
    m_people(prj->getProjectInfo().getInternalLocation(ProjectInfo::FaceRecognition), prj->getDatabase(), coreAccessor),
    m_faces(),
    m_photoPath(data.path),
    ui(new Ui::FacesDialog),
    m_pythonThread(coreAccessor->getPythonThread()),
    m_facesToAnalyze(0)
{
    ui->setupUi(this);
    ui->peopleList->setItemDelegate(new TableDelegate(completerFactory, this));
    ui->unassignedList->setItemDelegate(new TableDelegate(completerFactory, this));

    setUnassignedVisible(false);

    qRegisterMetaType<QVector<QRect>>("QVector<QRect>");

    connect(ui->scaleSlider, &QSlider::valueChanged,
            this, &FacesDialog::updateImage);

    connect(&m_people, &PeopleOperator::faces,
            this, &FacesDialog::applyFacesLocations);

    connect(&m_people, &PeopleOperator::recognized,
            this, &FacesDialog::applyFaceName);

    connect(&m_people, &PeopleOperator::unassigned,
            this, &FacesDialog::applyUnassigned);

    connect(this, &FacesDialog::accepted,
            this, &FacesDialog::apply);

    ui->statusLabel->setText(tr("Locating faces..."));
    m_people.fetchFaces(data.id);
    m_people.getUnassignedPeople(data.id);
    updateImage();
}


FacesDialog::~FacesDialog()
{
    delete ui;
}


void FacesDialog::applyFacesLocations(const QVector<FaceData>& faces)
{
    const QString status = faces.isEmpty()? tr("Found %1 face(s).").arg(faces.size()) :
                                            tr("Found %1 face(s). Recognizing people...").arg(faces.size());

    ui->statusLabel->setText(status);

    m_faces = faces;
    updateImage();
    updatePeopleList();

    m_facesToAnalyze = faces.size();

    for(const FaceData& face: faces)
        m_people.recognize(face);
}


void FacesDialog::applyFaceName(const FaceData& face, const PersonData& person)
{
    const QString name = person.name();

    auto it = std::find(m_faces.cbegin(), m_faces.cend(), face);

    if (it != m_faces.cend())
    {
        const std::size_t idx = std::distance(m_faces.cbegin(), it);
        ui->peopleList->setItem(idx, 0, new QTableWidgetItem(name));
    }

    m_facesToAnalyze--;

    if (m_facesToAnalyze == 0)
        ui->statusLabel->setText(tr("All known faces recognized."));
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
    QImage image(m_photoPath);
    const QSize currentSize = image.size();
    const double scale = ui->scaleSlider->value() / 100.0;

    const QSize scaledSize = currentSize * scale;
    image = image.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QVector<QRect> scaledFaces;
    for(const FaceData& faceData: std::as_const(m_faces))
    {
        const QRect& face = faceData.rect;
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
    std::vector<std::pair<FaceData, QString>> known_faces;
    QStringList unknownPeople;

    const int known_count = ui->peopleList->rowCount();
    assert( known_count == m_faces.size());

    for(int i = 0; i < known_count; i++)
    {
        const auto person = ui->peopleList->item(i, 0);
        const QString name = person == nullptr? QString(): person->text();

        auto face_data = std::make_pair(m_faces[i], name);
        known_faces.push_back(face_data);
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
}
