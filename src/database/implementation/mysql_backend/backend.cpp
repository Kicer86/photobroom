
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

    DefaultBackend::DefaultBackend() noexcept(true)
    {

    }


    DefaultBackend::~DefaultBackend()
    {
    }


    bool DefaultBackend::init() noexcept(true)
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


    bool DefaultBackend::store(const Entry &entry)
    {
        (void) entry;
        return true;
    }

}
