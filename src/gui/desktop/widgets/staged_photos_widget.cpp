/*
 * Widget for Staging area.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */

#include "staged_photos_widget.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "staged_photos_data_model.hpp"
#include "ui/photos_view.hpp"
#include "model_view/images_tree_view.hpp"


StagedPhotosWidget::StagedPhotosWidget(QWidget* p): QWidget(p),
                                                    m_view(new ImagesTreeView(this)),
                                                    m_dataModel(nullptr)
{
    QPushButton* commitButton = new QPushButton(tr("Review finished"));

    QHBoxLayout* h = new QHBoxLayout;
    h->addStretch();
    h->addWidget(commitButton);
    
    QLabel* header = new QLabel(tr("Changed photos:"), this);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(header);;
    l->addWidget(m_view);
    l->addLayout(h);

    connect(commitButton, SIGNAL(clicked(bool)), this, SLOT(applyChanges()));
}


StagedPhotosWidget::~StagedPhotosWidget()
{

}


void StagedPhotosWidget::setModel(StagedPhotosDataModel* model)
{
    m_view->setModel(model);
    m_dataModel = model;
}


void StagedPhotosWidget::set(IConfiguration* configuration)
{
    m_view->set(configuration);
}


QItemSelectionModel* StagedPhotosWidget::getSelectionModel()
{
    return m_view->selectionModel();
}


DBDataModel* StagedPhotosWidget::getModel()
{
    return m_dataModel;
}


QString StagedPhotosWidget::getName()
{
    return windowTitle();
}


void StagedPhotosWidget::applyChanges()
{
    m_dataModel->storePhotos();
}
