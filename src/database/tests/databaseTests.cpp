
#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "implementation/memorydatabase.hpp"
#include "ifs.hpp"


namespace
{
    struct FSImpl: public IStreamFactory
    {
        FSImpl(): m_stream(new std::stringstream) {}

        //open and return file
        std::shared_ptr<std::iostream> openStream(const std::string&, std::ios_base::openmode) override
        {
            return m_stream;
        }

        std::shared_ptr<std::stringstream> m_stream;
    };

    struct Backend final: public Database::IBackend
    {
        Backend(): m_entries() {}
        virtual ~Backend() {}

        virtual bool store(const APhotoInfo::Ptr& entry) override
        {
            m_entries.push_back(entry);

            return true;
        }

        virtual bool init() override
        {
            return true;
        }

        virtual void closeConnections() override { }

        virtual std::vector<TagNameInfo> listTags() override
        {
            return std::vector<TagNameInfo>();
        }

        virtual std::vector<TagValueInfo> listTagValues(const TagNameInfo&) override
        {
            return std::vector<TagValueInfo>();
        }

        virtual Database::PhotoIterator getPhotos()
        {
            return Database::PhotoIterator();
        }

        virtual void setPhotosSorting(const std::vector< Database::SortInfo >&) override { }

        std::vector<APhotoInfo::Ptr> m_entries;
    };


    struct MockPhotoInfo: IPhotoInfo
    {
        MOCK_CONST_METHOD0(getPath, const std::string & ());
        MOCK_CONST_METHOD0(getTags, std::shared_ptr<ITagData>());
        MOCK_METHOD0(rawPhotoData, const RawPhotoData & ());
        MOCK_METHOD0(rawThumbnailData, const RawPhotoData & ());
        MOCK_CONST_METHOD0(getHash, const Hash & ());
    };
}


TEST(MemoryDatabaseShould, AcceptAFileAndSendItToBackendAsSoonAsBackendIsSet)
{
    std::shared_ptr<FSImpl> fs = std::make_shared<FSImpl>();
    auto photoInfo = std::make_shared<MockPhotoInfo>();

    using ::testing::ReturnRef;
    const IPhotoInfo::Hash hash("0123456789abcdef");
    EXPECT_CALL(*photoInfo, getHash()).Times(1).WillOnce(ReturnRef(hash));

    *(fs->m_stream) << "Test content of file to store";

    Database::MemoryDatabase* db = new Database::MemoryDatabase(fs);

    db->addPhoto(photoInfo);

    std::shared_ptr<Backend> backend = std::make_shared<Backend>();
    db->setBackend(backend);

    db->close();
    delete db;             //"flush" data ;)

    const int s = backend->m_entries.size();
    ASSERT_EQ(1, s);
}
