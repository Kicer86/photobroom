
#include <iostream>
#include <sstream>
#include <vector>

#include <QVariant>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ifs.hpp"
#include "idatabase.hpp"
#include "photo_iterator.hpp"


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

        virtual bool store(const PhotoInfo::Ptr& entry) override
        {
            m_entries.push_back(entry);

            return true;
        }

        virtual bool init(const char *) override
        {
            return true;
        }

        virtual void closeConnections() override { }

        virtual std::vector<TagNameInfo> listTags() override
        {
            return std::vector<TagNameInfo>();
        }

        virtual std::set<TagValueInfo> listTagValues(const TagNameInfo&) override
        {
            return std::set<TagValueInfo>();
        }

        virtual Database::PhotoIterator getPhotos()
        {
            return Database::PhotoIterator();
        }

        virtual unsigned int getPhotosCount()
        {
            return 0;
        }

        std::vector<PhotoInfo::Ptr> m_entries;
    };

    /*
    struct MockPhotoInfo: IPhotoInfo
    {
        MOCK_CONST_METHOD0(getPath, const std::string & ());
        MOCK_CONST_METHOD0(getTags, std::shared_ptr<ITagData>());
        MOCK_CONST_METHOD0(getHash, const Hash & ());
    };
    */

    struct MockQuery: Database::IQuery
    {
        MOCK_METHOD0(gotoNext, bool());
        MOCK_METHOD1(getField, QVariant(const QString &));
        MOCK_CONST_METHOD0(valid, bool());
        MOCK_METHOD0(clone, std::shared_ptr<IQuery>());
    };
}

//TODO: rewrite
TEST(MemoryDatabaseShould, AcceptAFileAndSendItToBackendAsSoonAsBackendIsSet)
{
    /*
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
    */
}
