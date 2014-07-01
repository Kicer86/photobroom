
#include "photos_staging_area.hpp"

#include <memory>

#include <QBoxLayout>
#include <QPushButton>

#include <analyzer/photo_crawler_builder.hpp>
#include <analyzer/iphoto_crawler.hpp>
#include <core/tag.hpp>
#include <database/databasebuilder.hpp>
#include <database/idatabase.hpp>

#include "components/tag_editor/tag_editor_widget.hpp"
#include "photos_view_widget.hpp"

struct PhotosReceiver: IMediaNotification
{
    PhotosReceiver(): m_view(nullptr) {}

    void setView(PhotosViewWidget* view)
    {
        m_view = view;
    }

    virtual void found(const std::string& path)
    {
        m_view->addPhoto(path);
    }

    PhotosViewWidget* m_view;
};


PhotosStagingArea::PhotosStagingArea(Database::IFrontend* frontend, QWidget *p):
    QWidget(p),
    m_editor(nullptr),
    m_tagEditor(nullptr),
    m_frontend(frontend),
    m_photosReceiver(new PhotosReceiver)
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

    m_photosReceiver->setView(m_editor);
}


PhotosStagingArea::~PhotosStagingArea()
{

}


void PhotosStagingArea::pathToAnalyze(QString path)
{
    IPhotoCrawler* crawler = PhotoCrawlerBuilder().build();
    crawler->crawl(path.toStdString(), m_photosReceiver.get());
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
    m_editor->storePhotos();
}
