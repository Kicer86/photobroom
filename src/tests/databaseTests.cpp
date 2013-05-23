

#include "database/implementation/memorydatabase.hpp"
#include "database/iconfiguration.hpp"
#include "database/ifs.hpp"
#include "database/implementation/entry.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include <CppUTest/TestHarness.h>


TEST_GROUP(MemoryDatabaseShould)
{
	struct FSImpl: public FS
	{
		//open and return file
		std::iostream* openStream(const std::string &filename, std::ios_base::openmode mode)
		{
			return &m_stream;
		}

		//close opened file
		void closeStream(std::iostream *stream)
		{
			delete stream;
		}

		std::stringstream m_stream;
	};

	struct Backend: public Database::IBackend
	{
		virtual ~Backend() {}

		virtual bool store(const Database::Entry &entry)
		{
			m_entries.push_back(entry);

			return true;
		}

		std::vector<Database::Entry> m_entries;
	};
};


TEST(MemoryDatabaseShould, AcceptAFileAndSendItToBackendAsSoonAsBackendIsSet)
{
	struct Config: public Database::IConfiguration
	{
		virtual std::string getLocation() const
		{
			return "";
		}
	};
	
	std::shared_ptr<FSImpl> fs(new FSImpl);

	fs->m_stream << "Test content of file to store";
	Database::Entry::crc32 crc = 0;

	Database::MemoryDatabase *db = new Database::MemoryDatabase( new Config, fs);
	db->addFile("", Database::IFrontend::Description());

	Backend backend;
	db->setBackend(&backend);
    
    delete db;             //"flush" data ;)
	
    const int s = backend.m_entries.size();
	CHECK_EQUAL(2, s);
}
