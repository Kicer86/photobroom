
#include "backend.hpp"

#include <stdexcept>

#include <boost/filesystem.hpp>

#include "configuration/configurationfactory.hpp"
#include "configuration/iconfiguration.hpp"
#include "configuration/entrydata.hpp"

#include "entry.hpp"
#include "databasebuilder.hpp"


namespace Database
{

    DefaultBackend::DefaultBackend()
    {
        auto entry = ConfigurationFactory::get()->findEntry(Database::databaseLocation);

        if (entry)
        {
            boost::filesystem::path storage(entry->value());

            if (boost::filesystem::exists(storage) == false)
            {
                const bool status = boost::filesystem::create_directories(storage);
                if (status == false)
                    throw std::runtime_error("Could not create database directory: " + storage.string());
            }
        }
    }


    DefaultBackend::~DefaultBackend()
    {
    }


    bool DefaultBackend::store(const Entry &entry)
    {
        (void) entry;
        return true;
    }

}
