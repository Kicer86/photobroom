
#include "photos_staging_area.hpp"

#include <memory>

#include <QBoxLayout>
#include <QPushButton>

#include <analyzer/photo_crawler_builder.hpp>
#include <analyzer/iphoto_crawler.hpp>
#include <core/tag.hpp>
#include <core/tag_updater.hpp>
#include <database/database_builder.hpp>
#include <database/idatabase.hpp>

#include "components/tag_editor/tag_editor_widget.hpp"
#include "photos_view_widget.hpp"

struct TagGroupUpdater: ITagData
{
    TagGroupUpdater(const std::vector<IPhotoInfo::Ptr>& photos): m_tagUpdaters()
    {
        for(const IPhotoInfo::Ptr& photo: photos)
            m_tagUpdaters.emplace_back(photo);
    }

    virtual void clear()
    {
        perform(&TagUpdater::clear);
    }

    virtual Tag::TagsList getTags() const
    {
        Tag::TagsList tags;

        for(const TagUpdater& tagUpdater: m_tagUpdaters)
        {
            const Tag::TagsList l_tags = tagUpdater.getTags();

            for(auto it = l_tags.begin(); it != l_tags.end(); ++it)
            {
                auto f_it = tags.find(it->first);      //check if this tag already exists in main set of tags

                if (f_it != tags.end())  //it does
                {
                    Tag::ValuesSet new_value( { TagValueInfo("<multiple values>") } );
                    f_it->second = new_value;
                }
                else
                    tags.insert(*it);
            }
        }

        return tags;
    }

    virtual bool isValid() const
    {
        return true;
    }

    virtual void setTag(const TagNameInfo& name, const Tag::ValuesSet& values)
    {
        auto f = static_cast<void(TagUpdater::*)(const TagNameInfo &, const Tag::ValuesSet &)>(&TagUpdater::setTag);
        perform<const TagNameInfo &, const Tag::ValuesSet &>(f, name, values);
    }

    virtual void setTag(const TagNameInfo& name, const TagValueInfo& value)
    {
        auto f = static_cast<void(TagUpdater::*)(const TagNameInfo &, const TagValueInfo &)>(&TagUpdater::setTag);
        perform<const TagNameInfo &, const TagValueInfo &>(f, name, value);
    }

    virtual void setTags(const Tag::TagsList& tags)
    {
        perform<const Tag::TagsList &>(&TagUpdater::setTags, tags);
    }

    private:
        std::deque<TagUpdater> m_tagUpdaters;

        template<typename... Args>
        void perform(void (TagUpdater::*f)(Args...), Args... args)
        {
            for(TagUpdater& tagUpdater: m_tagUpdaters)
                (tagUpdater.*f)(args...);
        }
};


struct PhotosReceiver: IMediaNotification
{
    PhotosReceiver(): m_view(nullptr) {}
    PhotosReceiver(const PhotosReceiver &) = delete;

    PhotosReceiver& operator=(const PhotosReceiver &) = delete;

    void setView(PhotosViewWidget* view)
    {
        m_view = view;
    }

    virtual void found(const QString& path) override
    {
        m_view->addPhoto(path);
    }

    PhotosViewWidget* m_view;
};


PhotosStagingArea::PhotosStagingArea(QWidget *p):
    QWidget(p),
    m_editor(nullptr),
    m_tagEditor(nullptr),
    m_photosReceiver(new PhotosReceiver)
{
    BrowseLine *browse = new BrowseLine(this);
    m_editor = new PhotosViewWidget(this);
    m_tagEditor = new TagEditorWidget(this);

    browse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(browse, SIGNAL(addPath(QString)), this, SLOT(pathToAnalyze(const QString &)));
    connect(m_editor, SIGNAL(selectionChanged(const std::vector<IPhotoInfo::Ptr> &)),
            this, SLOT(viewSelectionChanged(const std::vector<IPhotoInfo::Ptr> &)));

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


void PhotosStagingArea::setDatabase(Database::IDatabase* db)
{
    m_editor->setDatabase(db);
}


void PhotosStagingArea::set(ITaskExecutor* taskExecutor)
{
    m_editor->set(taskExecutor);
}


void PhotosStagingArea::pathToAnalyze(const QString& path)
{
    IPhotoCrawler* crawler = PhotoCrawlerBuilder().build();
    crawler->crawl(path, m_photosReceiver.get());
}


void PhotosStagingArea::viewSelectionChanged(const std::vector<IPhotoInfo::Ptr>& photos)
{
    TagGroupUpdater* tagsUpdater = new TagGroupUpdater(photos);

    std::shared_ptr<ITagData> tagsPtr(tagsUpdater);
    m_tagEditor->setTags(tagsPtr);
}


void PhotosStagingArea::savePhotos()
{
    m_editor->storePhotos();
}
