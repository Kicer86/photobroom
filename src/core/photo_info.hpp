
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include "core_export.h"

class QPixmap;

struct ITagData;
struct APhotoInfoInitData;
struct HashAssigner;

class CORE_EXPORT PhotoInfo final
{
    public:
        typedef std::shared_ptr<PhotoInfo> Ptr;
        typedef std::string Hash;                // Hash is hash of photo's file

        struct CORE_EXPORT Id
        {
                typedef int type;

                Id();
                explicit Id(type);
                Id(const Id &) = default;

                Id& operator=(const Id &) = default;
                operator type() const;
                bool operator!() const;
                bool valid() const;
                type value() const;

            private:
                type m_value;
                bool m_valid;
        };

        struct IObserver
        {
            IObserver() {};
            virtual void photoUpdated() = 0;
        };

        struct Flags
        {
            //information
            bool stagingArea;

            //related to data loading
            bool tagsLoaded;
            bool hashLoaded;
            bool thumbnailLoaded;

            Flags();
        };

        PhotoInfo(const std::string &path);      //load all data from provided path
        PhotoInfo(const APhotoInfoInitData &);   //load all data from provided struct
        PhotoInfo(const PhotoInfo &) = delete;
        virtual ~PhotoInfo();

        PhotoInfo& operator=(const PhotoInfo &) = delete;

        //data getting
        const std::string& getPath() const;
        std::shared_ptr<ITagData> getTags() const;
        const QPixmap& getThumbnail() const;     // a temporary thumbnail may be returned when final one is not yet generated.
        const Hash& getHash() const;             // Do not call until isHashLoaded()
        Id getID() const;

        //status checking
        bool isLoaded() const;                   // returns true if hash is not null, and thumbnail is loaded (photo fully loaded)
        bool isHashLoaded() const;               // returns true if hash is not null
        bool isThumbnailLoaded() const;          // returns true if thumbnail is loaded
        bool areTagsLoaded() const;              // returns true is tags were loaded

        //observers
        void registerObserver(IObserver *);
        void unregisterObserver(IObserver *);

        //data initializing
        void initHash(const Hash &);
        void initThumbnail(const QPixmap &);
        void initID(const Id &);

        //flags
        void markStagingArea(bool = true);            // mark photo as stage area's photo
        Flags getFlags() const;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        void updated();
};


struct CORE_EXPORT APhotoInfoInitData
{
    APhotoInfoInitData();

    std::string path;
    std::shared_ptr<ITagData> tags;
    PhotoInfo::Hash hash;
};

#endif
