
#define CPPUTEST_MEM_LEAK_DETECTION_DISABLED

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
		std::iostream* openStream(const std::string &, std::ios_base::openmode)
		{
			return &m_stream;
		}

		//close opened file
		void closeStream(std::iostream *stream)
		{
		
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
	
	std::shared_ptr<FSImpl> fs = std::make_shared<FSImpl>();

	fs->m_stream << "Test content of file to store";
	Database::Entry::crc32 crc = 0;
    Config *config = new Config;

	Database::MemoryDatabase *db = new Database::MemoryDatabase(config, fs);
	db->addFile("", Database::IFrontend::Description());

	std::shared_ptr<Backend> backend = std::make_shared<Backend>();
	db->setBackend(backend);
    
    delete db;             //"flush" data ;)
    delete config;
	
    const int s = backend->m_entries.size();
	CHECK_EQUAL(1, s);
}
