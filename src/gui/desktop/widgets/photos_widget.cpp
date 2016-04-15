/*
 * Widget for Photos
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

#include "photos_widget.hpp"

#include <QLineEdit>
#include <QPainter>
#include <QVBoxLayout>
#include <QLayoutItem>

#include <configuration/iconfiguration.hpp>

#include "config_keys.hpp"
#include "info_widget.hpp"
#include "models/db_data_model.hpp"
#include "ui_utils/photos_item_delegate.hpp"
#include "views/images_tree_view.hpp"


PhotosWidget::PhotosWidget(QWidget* p):
    QWidget(p),
    m_timer(),
    m_model(nullptr),
    m_view(nullptr),
    m_info(nullptr),
    m_delegate(nullptr),
    m_searchExpression(nullptr),
    m_topHintLayout(nullptr)
{
    // photos view
    m_view = new ImagesTreeView(this);
    m_delegate = new PhotosItemDelegate(m_view);

    m_view->setItemDelegate(m_delegate);

    // info baloon
    m_info = new InfoBaloonWidget(this);
    m_info->hide();

    // search panel
    QLabel* searchPrompt = new QLabel(tr("Search:"), this);
    m_searchExpression = new QLineEdit(this);

    QHBoxLayout* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchPrompt);
    searchLayout->addWidget(m_searchExpression);

    // hint layout
    m_topHintLayout = new QVBoxLayout;

    // main layout
    QVBoxLayout* l = new QVBoxLayout(this);
    l->addLayout(searchLayout);
    l->addLayout(m_topHintLayout);
    l->addWidget(m_view);
    l->addWidget(m_info);

    // setup timer
    m_timer.setInterval(500);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &PhotosWidget::applySearchExpression);

    //
    connect(m_searchExpression, &QLineEdit::textEdited, this, &PhotosWidget::searchExpressionChanged);
}


PhotosWidget::~PhotosWidget()
{

}


void PhotosWidget::set(IConfiguration* configuration)
{
    const QVariant marginEntry = configuration->getEntry(ViewConfigKeys::itemsSpacing);
    assert(marginEntry.isValid());
    const int spacing = marginEntry.toInt();

    m_view->setSpacing(spacing);

    m_delegate->set(configuration);
}


void PhotosWidget::setModel(DBDataModel* m)
{
    m_model = m;
    m_view->setModel(m);

    connect(m, &QAbstractItemModel::rowsInserted, this, &PhotosWidget::modelChanged);
    connect(m, &QAbstractItemModel::rowsRemoved, this, &PhotosWidget::modelChanged);
    connect(m, &QAbstractItemModel::dataChanged, this, &PhotosWidget::dataChanged);

    updateHint();
}


QItemSelectionModel* PhotosWidget::viewSelectionModel()
{
    return m_view->selectionModel();
}


void PhotosWidget::setTopHintWidget(QWidget* hintWidget)
{
    if (m_topHintLayout->count() > 0)
    {
        assert(m_topHintLayout->count() == 1);
        QLayoutItem* item = m_topHintLayout->itemAt(0);
        QWidget* widget = item->widget();

        assert(widget != nullptr);
        delete widget;
    }

    if (hintWidget != nullptr)
        m_topHintLayout->addWidget(hintWidget);
}


void PhotosWidget::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);

    updateHint();
}


void PhotosWidget::modelChanged(const QModelIndex &, int, int)
{
    updateHint();
}


void PhotosWidget::updateHint()
{
    // check if model is empty
    const QVariant statusVariant = m_model->data(QModelIndex(), DBDataModel::NodeStatus);
    const bool empty = m_model->rowCount() == 0;

    assert(statusVariant.canConvert(QMetaType::Int));
    const NodeStatus status = static_cast<NodeStatus>(statusVariant.toInt());

    if (status == NodeStatus::Fetched)
        m_info->setText(tr("There are no photos in your collection.\n\nAdd some by choosing 'Add photos' action from 'Photos' menu."));
    else if (status == NodeStatus::Fetching)
        m_info->setText(tr("Loading photos..."));
    else
        m_info->setText("");

    m_info->setVisible( (status == NodeStatus::Fetched && empty && isEnabled()) ||
                         status == NodeStatus::Fetching);
}


void PhotosWidget::searchExpressionChanged(const QString &)
{
    m_timer.start();
}


void PhotosWidget::applySearchExpression()
{
    const QString search = m_searchExpression->text();

    m_model->applyFilters(search);
}


void PhotosWidget::dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)
{
    updateHint();
}
