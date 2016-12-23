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

#include <QMenu>
#include <QLineEdit>
#include <QPainter>
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QContextMenuEvent>

#include <configuration/iconfiguration.hpp>
#include <core/base_tags.hpp>
#include <core/exif_reader_factory.hpp>

#include "config_keys.hpp"
#include "info_widget.hpp"
#include "multi_value_line_edit.hpp"
#include "models/db_data_model.hpp"
#include "ui/photos_grouping_dialog.hpp"
#include "ui_utils/icompleter_factory.hpp"
#include "ui_utils/photos_item_delegate.hpp"
#include "views/images_tree_view.hpp"

namespace
{
    const char* expressions_separator = ",";
}

PhotosWidget::PhotosWidget(QWidget* p):
    QWidget(p),
    m_timer(),
    m_thumbnailAcquisitor(),
    m_selectionExtractor(),
    m_model(nullptr),
    m_view(nullptr),
    m_delegate(nullptr),
    m_searchExpression(nullptr),
    m_bottomHintLayout(nullptr),
    m_manager(nullptr),
    m_executor(nullptr),
    m_contextMenu(nullptr)
{
    using namespace std::placeholders;
    auto thumbUpdate = std::bind(&PhotosWidget::thumbnailUpdated, this, _1, _2);
    const QImage image(":/gui/clock.svg");
    m_thumbnailAcquisitor.setInProgressThumbnail(image);
    m_thumbnailAcquisitor.setObserver(thumbUpdate);

    // photos view
    m_view = new ImagesTreeView(this);
    m_delegate = new PhotosItemDelegate(m_view);

    m_delegate->set(&m_thumbnailAcquisitor);
    m_view->setItemDelegate(m_delegate);

    // search panel
    QLabel* searchPrompt = new QLabel(tr("Search:"), this);
    m_searchExpression = new MultiValueLineEdit(expressions_separator, this);

    QHBoxLayout* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchPrompt);
    searchLayout->addWidget(m_searchExpression);

    // bottom tools
    const int thumbnailSize = m_view->getThumbnailHeight();

    QLabel* zoomLabel = new QLabel(tr("Thumbnail size:"), this);
    QSlider* zoomSlider = new QSlider(this);
    QLabel* zoomSizeLabel = new QLabel(this);

    zoomSlider->setOrientation(Qt::Horizontal);
    zoomSlider->setMinimum(40);
    zoomSlider->setMaximum(400);
    zoomSlider->setSingleStep(10);
    zoomSlider->setTickInterval(20);
    zoomSlider->setPageStep(30);
    zoomSlider->setValue(thumbnailSize);
    zoomSlider->setTickPosition(QSlider::TicksBelow);

    QHBoxLayout* bottomTools = new QHBoxLayout;
    bottomTools->addStretch(3);
    bottomTools->addWidget(zoomLabel);
    bottomTools->addWidget(zoomSlider, 1);
    bottomTools->addWidget(zoomSizeLabel);
    bottomTools->setSpacing(0);

    auto updateZoomSizeLabel = [zoomSizeLabel](int size)
    {
        const QString t = QString("%1 px").arg(size);
        zoomSizeLabel->setText(t);
    };

    updateZoomSizeLabel(thumbnailSize);

    // hint layout
    m_bottomHintLayout = new QVBoxLayout;

    // view + hints layout
    QVBoxLayout* view_hints_layout = new QVBoxLayout;
    view_hints_layout->setContentsMargins(0, 0, 0, 0);
    view_hints_layout->setSpacing(0);
    view_hints_layout->addWidget(m_view);
    view_hints_layout->addLayout(m_bottomHintLayout);
    view_hints_layout->addLayout(bottomTools);

    // main layout
    QVBoxLayout* l = new QVBoxLayout(this);
    l->addLayout(searchLayout);
    l->addLayout(view_hints_layout);

    // setup timer
    m_timer.setInterval(500);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &PhotosWidget::applySearchExpression);

    //
    connect(m_searchExpression, &QLineEdit::textEdited, this, &PhotosWidget::searchExpressionChanged);
    connect(m_view, &ImagesTreeView::contentScrolled, this, &PhotosWidget::viewScrolled);
    connect(this, &PhotosWidget::performUpdate, m_view, &ImagesTreeView::refreshView, Qt::QueuedConnection);
    connect(zoomSlider, &QAbstractSlider::valueChanged, [this, updateZoomSizeLabel](int thumbnailHeight)
    {
        updateZoomSizeLabel(thumbnailHeight);
        m_view->setThumbnailHeight(thumbnailHeight);
        m_thumbnailAcquisitor.dismissPendingTasks();
    });
}


PhotosWidget::~PhotosWidget()
{

}


void PhotosWidget::set(ITaskExecutor* executor)
{
    m_thumbnailAcquisitor.set(executor);
    m_executor = executor;
}


void PhotosWidget::set(IPhotosManager* manager)
{
    m_manager = manager;
    m_thumbnailAcquisitor.set(manager);
}


void PhotosWidget::set(IConfiguration* configuration)
{
    const QVariant marginEntry = configuration->getEntry(ViewConfigKeys::itemsSpacing);
    assert(marginEntry.isValid());
    const int spacing = marginEntry.toInt();

    m_view->setSpacing(spacing);
    m_delegate->set(configuration);
}


void PhotosWidget::set(ICompleterFactory* completerFactory)
{
    auto allTagTypes = BaseTags::getAll();
    std::set<TagNameInfo> allTags;

    std::copy(allTagTypes.begin(), allTagTypes.end(), std::inserter(allTags, allTags.end()));

    QCompleter* completer = completerFactory->createCompleter(allTags);
    m_searchExpression->setCompleter(completer);
}


void PhotosWidget::setModel(DBDataModel* m)
{
    m_model = m;
    m_view->setModel(m);

    m_selectionExtractor.set(m);
    m_selectionExtractor.set(viewSelectionModel());
}


QItemSelectionModel* PhotosWidget::viewSelectionModel()
{
    return m_view->selectionModel();
}


void PhotosWidget::setBottomHintWidget(InfoBaloonWidget* hintWidget)
{
    if (m_bottomHintLayout->count() > 0)
    {
        assert(m_bottomHintLayout->count() == 1);
        QLayoutItem* item = m_bottomHintLayout->itemAt(0);
        QWidget* widget = item->widget();

        assert(widget != nullptr);
        delete widget;
    }

    if (hintWidget != nullptr)
        m_bottomHintLayout->addWidget(hintWidget);
}


void PhotosWidget::registerContexMenu(QMenu* menu)
{
    m_contextMenu = menu;
}


void PhotosWidget::contextMenuEvent(QContextMenuEvent* e)
{
    // TODO: to extract?
    const std::vector<IPhotoInfo::Ptr> photos = m_selectionExtractor.getSelection();

    QMenu contextMenu;
    QAction* groupPhotos = contextMenu.addAction(tr("Group"));

    QAction* chosenAction = contextMenu.exec(e->globalPos());

    if (chosenAction == groupPhotos)
    {
        ExifReaderFactory factory;
        factory.set(m_manager);

        std::shared_ptr<IExifReader> reader = factory.get();

        PhotosGroupingDialog dialog(photos, reader.get(), m_executor);
        const int status = dialog.exec();

        if (status == QDialog::Accepted)
        {
            const QString photo = dialog.getRepresentative();

            std::vector<Photo::Id> photos_ids;
            for(std::size_t i = 0; i < photos.size(); i++)
                photos_ids.push_back(photos[i]->getID());

            m_model->group(photos_ids, photo);
        }
    }
}


void PhotosWidget::searchExpressionChanged(const QString &)
{
    m_timer.start();
}


void PhotosWidget::viewScrolled()
{
    m_thumbnailAcquisitor.dismissPendingTasks();
}


void PhotosWidget::applySearchExpression()
{
    const QString search = m_searchExpression->text();
    const SearchExpressionEvaluator::Expression expression = SearchExpressionEvaluator(expressions_separator).evaluate(search);

    m_model->applyFilters(expression);
}


void PhotosWidget::thumbnailUpdated(const ThumbnailInfo &, const QImage &)
{
    // TODO: do it smarter (find QModelIndex for provided info)
    emit performUpdate();
}
