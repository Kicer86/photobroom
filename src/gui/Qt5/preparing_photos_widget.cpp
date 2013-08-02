
#include "preparing_photos_widget.hpp"
#include "private/preparing_photos_widget_private.hpp"

#include <QVBoxLayout>
#include <QItemSelectionModel>

#include "photos_editor_widget.hpp"
#include "tag_editor_widget.hpp"

#include "analyzer/photo_crawler_builder.hpp"
#include "analyzer/iphoto_crawler.hpp"


PreparingPhotosWidget::PreparingPhotosWidget(QWidget *p):
    QWidget(p),
    m_editor(nullptr),
    m_tagEditor(nullptr)
{

    BrowseLine *browse = new BrowseLine(this);
    m_editor = new PhotosEditorWidget(this);
    m_tagEditor = new TagEditorWidget(this);

    browse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(browse, SIGNAL(addPath(QString)), this, SLOT(pathToAnalyze(QString)));

    connect(m_editor->getSelectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(selectionChanged(const QItemSelection &))
            );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(browse);
    mainLayout->addWidget(m_editor);
    mainLayout->addWidget(m_tagEditor);    
}


PreparingPhotosWidget::~PreparingPhotosWidget()
{

}


void PreparingPhotosWidget::pathToAnalyze(QString path)
{
    std::shared_ptr<IPhotoCrawler> crawler = PhotoCrawlerBuilder().build();

    std::vector<std::string> files = crawler->crawl(path.toStdString());

    m_editor->addPhotos(files);
}


void PreparingPhotosWidget::selectionChanged(const QItemSelection &selection)
{
}
