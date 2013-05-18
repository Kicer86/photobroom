

#include "database/implementation/memorydatabase.hpp"
#include "database/iconfiguration.hpp"
#include "database/ifs.hpp"

#include <iostream>
#include <sstream>

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
};


TEST(MemoryDatabaseShould, AcceptAFileAndSendItToBackend)
{
	struct Config: public Database::IConfiguration
	{
		virtual std::string getLocation() const
		{
			return "";
		}
	};
	
	Database::MemoryDatabase *db = new Database::MemoryDatabase( new Config, std::shared_ptr<FS>(new FSImpl) );
    CHECK_EQUAL(true, false);
}
