
#include "backend.hpp"

#include <stdexcept>

#include <QProcess>

#include <boost/filesystem.hpp>

#include "configuration/configurationfactory.hpp"
#include "configuration/iconfiguration.hpp"
#include "configuration/entrydata.hpp"

#include "entry.hpp"
#include "databasebuilder.hpp"


namespace Database
{

    MySqlBackend::MySqlBackend() noexcept(true)
    {

    }


    MySqlBackend::~MySqlBackend()
    {
    }


    bool MySqlBackend::init() noexcept(true)
    {
        auto entry = ConfigurationFactory::get()->findEntry(Database::databaseLocation);
        bool status = true;

        //create directories
        if (entry)
        {
            boost::filesystem::path storage(entry->value());

            if (boost::filesystem::exists(storage) == false)
                status = boost::filesystem::create_directories(storage);
        }

        //start mysql process


        return status;
    }


    bool MySqlBackend::store(const Entry &entry)
    {
        (void) entry;
        return true;
    }

}
