
#include "preparing_photos_widget.hpp"
#include "private/preparing_photos_widget_private.hpp"

#include <QVBoxLayout>

#include "photos_editor_widget.hpp"

#include "analyzer/photo_crawler_builder.hpp"


PreparingPhotosWidget::PreparingPhotosWidget(QWidget *parent):
    QWidget(parent),
    m_editor(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    BrowseLine *browse = new BrowseLine(this);
    m_editor = new PhotosEditorWidget(this);

    browse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(browse, SIGNAL(addPath(QString)), this, SLOT(pathToAnalyze(QString)));

    layout->addWidget(browse);
    layout->addWidget(m_editor);
}


PreparingPhotosWidget::~PreparingPhotosWidget()
{

}


void PreparingPhotosWidget::pathToAnalyze(QString path)
{

}
