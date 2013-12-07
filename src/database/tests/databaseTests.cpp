
#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

#include "implementation/memorydatabase.hpp"
#include "ifs.hpp"
#include "implementation/entry.hpp"


namespace 
{
	struct FSImpl: public FS
	{
        FSImpl(): m_stream() {}
		//open and return file
		std::iostream* openStream(const std::string &, std::ios_base::openmode)
		{
			return &m_stream;
		}

		//close opened file
		void closeStream(std::iostream *stream)
		{
            (void) stream;
		}

		std::stringstream m_stream;
	};

	struct Backend: public Database::IBackend
	{
        Backend(): m_entries() {}
		virtual ~Backend() {}

		virtual bool store(const Database::Entry &entry)
		{
			m_entries.push_back(entry);

			return true;
		}

		std::vector<Database::Entry> m_entries;
	};
}


TEST(MemoryDatabaseShould, AcceptAFileAndSendItToBackendAsSoonAsBackendIsSet)
{
	std::shared_ptr<FSImpl> fs = std::make_shared<FSImpl>();

	fs->m_stream << "Test content of file to store";

	Database::MemoryDatabase *db = new Database::MemoryDatabase(fs);
	db->addFile("", Database::IFrontend::Description());

	std::shared_ptr<Backend> backend = std::make_shared<Backend>();
	db->setBackend(backend);
    
    delete db;             //"flush" data ;)
	
    const int s = backend->m_entries.size();
	ASSERT_EQ(1, s);
}
