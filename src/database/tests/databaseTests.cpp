
#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

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

    struct Backend: public Database::IBackend
    {
        Backend(): m_entries() {}
        virtual ~Backend() {}

        virtual bool store(const APhotoInfo::Ptr& entry) override final
        {
            m_entries.push_back(entry);

            return true;
        }

        virtual bool init() override final
        {
            return true;
        }

        virtual void closeConnections() override final
        {

        }

        virtual std::vector<TagNameInfo> listTags() override final
        {
            return std::vector<TagNameInfo>();
        }

        virtual std::vector<TagValueInfo> listTagValues(const TagNameInfo&) override final
        {
            return std::vector<TagValueInfo>();
        }

        std::vector<APhotoInfo::Ptr> m_entries;
    };
}


TEST(MemoryDatabaseShould, AcceptAFileAndSendItToBackendAsSoonAsBackendIsSet)
{
    std::shared_ptr<FSImpl> fs = std::make_shared<FSImpl>();

    *(fs->m_stream) << "Test content of file to store";

    Database::MemoryDatabase* db = new Database::MemoryDatabase(fs);
    db->addPhoto(APhotoInfo::Ptr());

    std::shared_ptr<Backend> backend = std::make_shared<Backend>();
    db->setBackend(backend);

    db->close();
    delete db;             //"flush" data ;)

    const int s = backend->m_entries.size();
    ASSERT_EQ(1, s);
}
