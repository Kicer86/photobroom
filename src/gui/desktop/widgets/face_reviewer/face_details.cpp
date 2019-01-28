/*
 * Widget with details about person
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "face_details.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMetaObject>
#include <QPushButton>
#include <QVBoxLayout>

#include <core/cross_thread_call.hpp>
#include <core/itask_executor.hpp>
#include <core/task_executor_utils.hpp>

#include "utils/people_operator.hpp"

using namespace std::placeholders;


FaceDetails::FaceDetails(const QString& name,
                         IModelFaceFinder* finder,
                         ITaskExecutor* executor,
                         const std::vector<PersonInfo>& pi,
                         QWidget* p):
    QGroupBox(name, p),
    m_pi(pi),
    m_name(name),
    m_executor(executor),
    m_optButton(nullptr),
    m_photo(nullptr),
    m_modelFaceFinder(finder),
    m_fetched(false)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    QVBoxLayout* dl = new QVBoxLayout;
    m_optButton = new QPushButton(tr("Find better"), this);
    m_photo = new QLabel(this);
    m_photo->setContextMenuPolicy(Qt::CustomContextMenu);
    QLabel* occurences = new QLabel(this);

    dl->addWidget(occurences);
    dl->addWidget(m_optButton);

    l->addWidget(m_photo);
    l->addLayout(dl);
    l->addStretch();

    connect(m_optButton, &QPushButton::clicked, this, &FaceDetails::optimize);
    connect(m_photo, &QWidget::customContextMenuRequested,
            this, &FaceDetails::openContextMenu);

    occurences->setText(tr("On %n photo(s)", "", static_cast<int>(pi.size())));
}


FaceDetails::~FaceDetails()
{

}


void FaceDetails::setModelPhoto(const QPixmap& p)
{
    m_photo->setPixmap(p);
}


void FaceDetails::setModelPhoto(const QImage& img)
{
    const QPixmap pixmap = QPixmap::fromImage(img);
    setModelPhoto(pixmap);
}


void FaceDetails::optimize()
{
    m_optButton->clearFocus();
    m_optButton->setDisabled(true);

    m_modelFaceFinder->findBest(m_pi, slot(this, &FaceDetails::updateRepresentative));
}


void FaceDetails::updateRepresentative(const QString& path)
{
    m_optButton->setEnabled(true);

    if (path.isEmpty() == false)
    {
        QPointer<FaceDetails> This(this);
        runOn(m_executor, [This, path]
        {
            FaceDetails* fd = This.data();

            if (fd)
            {
                const QImage faceImg(path);
                const QImage scaled = faceImg.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

                // do not call slot directly - make sure it will be called from main thread
                invokeMethod(fd, qOverload<const QImage &>(&FaceDetails::setModelPhoto), scaled);
            }
        });
    }
}


void FaceDetails::initialFetch()
{
    if (m_pi.empty() == false)
        updateRepresentative(m_modelFaceFinder->currentBest(m_pi.front().p_id));
}


void FaceDetails::paintEvent(QPaintEvent* event)
{
    QGroupBox::paintEvent(event);

    if (m_fetched == false)
    {
        QMetaObject::invokeMethod(this, &FaceDetails::initialFetch, Qt::QueuedConnection);
        m_fetched = true;
    }
}


void FaceDetails::openContextMenu(const QPoint& point)
{
    QMenu menu(this);
    QAction* go     = menu.addAction(tr("Go to photo"));


    const QPoint pos = this->mapToGlobal(point);
    QAction* result = menu.exec(pos);

    if (result == go)
    {
        gotoPerson();
    }
}


void FaceDetails::gotoPerson()
{

}
