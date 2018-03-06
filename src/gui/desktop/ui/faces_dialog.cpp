
#include "faces_dialog.hpp"

#include <QPainter>

#include <core/icore_factory_accessor.hpp>
#include <core/ipython_thread.hpp>

#include "ui_faces_dialog.h"

using namespace std::placeholders;

FacesDialog::FacesDialog(ICoreFactoryAccessor* coreAccessor, QWidget *parent):
    QDialog(parent),
    m_faces(),
    m_faceRecognizer(coreAccessor),
    m_photoPath(),
    ui(new Ui::FacesDialog),
    m_pythonThread(coreAccessor->getPythonThread())
{
    ui->setupUi(this);

    qRegisterMetaType<QVector<QRect>>("QVector<QRect>");
    connect(this, &FacesDialog::gotFacesLocations,
            this, &FacesDialog::applyFacesLocations);

    connect(ui->scaleSlider, &QSlider::valueChanged,
            this, &FacesDialog::updateImage);
}


FacesDialog::~FacesDialog()
{
    delete ui;
}


void FacesDialog::load(const QString& photo)
{
    m_photoPath = photo;

    ui->statusLabel->setText(tr("Locating faces..."));
    m_faceRecognizer.findFaces(photo, std::bind(&FacesDialog::gotFacesLocations, this, _1));
    updateImage();
}


void FacesDialog::applyFacesLocations(const QVector<QRect>& faces)
{
    const QString status = tr("Found %1 face(s)").arg(faces.size());
    ui->statusLabel->setText(status);
    m_faces = faces;
    updateImage();
    updatePeopleList();
}


void FacesDialog::updateImage()
{
    QImage image(m_photoPath);
    const QSize currentSize = image.size();
    const double scale = ui->scaleSlider->value() / 100.0;

    const QSize scaledSize = currentSize * scale;
    image = image.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QVector<QRect> scaledFaces;
    for(QRect face: m_faces)
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


void FacesDialog::updatePeopleList()
{
    const int rowCount = ui->peopleList->rowCount();
    const int peopleCount = m_faces.size();

    if (rowCount < peopleCount)
        ui->peopleList->setRowCount(peopleCount);
}
