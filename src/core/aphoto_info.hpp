
#ifndef PHOTO_INFO_HPP
#define PHOTO_INFO_HPP

#include <memory>
#include <cstdint>
#include <string>

#include "core_export.h"

class QString;
class QPixmap;

struct ITagData;
struct APhotoInfoInitData;
struct HashAssigner;

class CORE_EXPORT APhotoInfo
{
    public:
        struct IObserver
        {
            IObserver() {};
            virtual void photoUpdated() = 0;
        };

        typedef std::shared_ptr<APhotoInfo> Ptr;
        typedef std::string Hash;

        APhotoInfo(const std::string &path);      //load all data from provided path
        APhotoInfo(const APhotoInfoInitData &);   //load all data from provided struct
        APhotoInfo(const APhotoInfo &) = delete;
        virtual ~APhotoInfo();

        const std::string& getPath() const;
        std::shared_ptr<ITagData> getTags() const;

        //photo data
        const QPixmap& getThumbnail() const;     // a temporary thumbnail may be returned when final one is not yet generated

        // Function may return empty hash, when it is not yet calculated.
        // The returned value is hash of photo's content (pixels) not whole file itself.
        const Hash& getHash() const;

        void registerObserver(IObserver *);

    protected:
        void updated();

    private:
        struct Data;
        std::unique_ptr<Data> m_data;

        friend struct HashAssigner;
        void setHash(const Hash &);
};


struct CORE_EXPORT APhotoInfoInitData
{
    APhotoInfoInitData();

    std::string path;
    std::shared_ptr<ITagData> tags;
    APhotoInfo::Hash hash;
};

#endif
