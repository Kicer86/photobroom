
#include "faces_dialog.hpp"

#include <QPainter>

#include <core/icore_factory_accessor.hpp>
#include <core/ipython_thread.hpp>

#include "ui_faces_dialog.h"

using namespace std::placeholders;

FacesDialog::FacesDialog(ICoreFactoryAccessor* coreAccessor, QWidget *parent):
    QDialog(parent),
    m_faceRecognizer(coreAccessor),
    ui(new Ui::FacesDialog),
    m_pythonThread(coreAccessor->getPythonThread())
{
    ui->setupUi(this);

    qRegisterMetaType<QVector<QRect>>("QVector<QRect>");
    connect(this, &FacesDialog::gotFacesLocations,
            this, &FacesDialog::applyFacesLocations);
}


FacesDialog::~FacesDialog()
{
    delete ui;
}


void FacesDialog::load(const QString& photo)
{
    QPixmap pixmap(photo);
    ui->imageView->setPixmap(pixmap);

    m_faceRecognizer.findFaces(photo, std::bind(&FacesDialog::gotFacesLocations, this, _1));
}


void FacesDialog::applyFacesLocations(const QVector<QRect>& locations)
{
    const QPixmap* pixmap = ui->imageView->pixmap();

    QImage image = pixmap->toImage();
    QPainter painter(&image);

    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(20);
    painter.setPen(pen);
    painter.drawRects(locations);

    QPixmap new_pixmap = QPixmap::fromImage(image);
    ui->imageView->setPixmap(new_pixmap);
}
