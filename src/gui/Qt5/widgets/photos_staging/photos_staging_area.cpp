
#include "photos_staging_area.hpp"

#include <memory>

#include <QBoxLayout>
#include <QPushButton>

#include <analyzer/photo_crawler_builder.hpp>
#include <analyzer/iphoto_crawler.hpp>
#include <core/tag.hpp>
#include <database/databasebuilder.hpp>
#include <database/idatabase.hpp>

#include "widgets/tag_editor/tag_editor_widget.hpp"
#include "photos_view_widget.hpp"

PhotosStagingArea::PhotosStagingArea(Database::IFrontend* frontend, QWidget *p):
    QWidget(p),
    m_editor(nullptr),
    m_tagEditor(nullptr),
    m_frontend(frontend)
{
    BrowseLine *browse = new BrowseLine(this);
    m_editor = new PhotosViewWidget(this);
    m_tagEditor = new TagEditorWidget(this);

    browse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(browse, SIGNAL(addPath(QString)), this, SLOT(pathToAnalyze(QString)));
    connect(m_editor, SIGNAL(selectionChanged(const std::vector<PhotoInfo::Ptr> &)),
            this, SLOT(viewSelectionChanged(const std::vector<PhotoInfo::Ptr> &)));

    QHBoxLayout* savePhotosLayout = new QHBoxLayout(nullptr);
    QPushButton* saveButton = new QPushButton(tr("save photos"));
    savePhotosLayout->addStretch(1);
    savePhotosLayout->addWidget(saveButton);
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(savePhotos()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(browse);
    mainLayout->addWidget(m_editor);
    mainLayout->addWidget(m_tagEditor);
    mainLayout->addLayout(savePhotosLayout);
}


PhotosStagingArea::~PhotosStagingArea()
{

}


void PhotosStagingArea::pathToAnalyze(QString path)
{
    std::shared_ptr<IPhotoCrawler> crawler = PhotoCrawlerBuilder().build();

    std::vector<std::string> files = crawler->crawl(path.toStdString());

    m_editor->addPhotos(files);
}


void PhotosStagingArea::viewSelectionChanged(const std::vector<PhotoInfo::Ptr>& photos)
{
    std::vector<std::shared_ptr<ITagData>> tags;

    for(const PhotoInfo::Ptr& photo: photos)
        tags.push_back(photo->getTags());

    TagDataComposite* tagsData = new TagDataComposite;
    tagsData->setTagDatas(tags);

    std::shared_ptr<ITagData> tagsPtr(tagsData);
    m_tagEditor->setTags(tagsPtr);
}


void PhotosStagingArea::savePhotos()
{
    const std::vector<PhotoInfo::Ptr> photos = m_editor->getPhotos();

    for(const PhotoInfo::Ptr& photo: photos)
        m_frontend->addPhoto(photo);
}
