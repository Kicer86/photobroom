
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

    struct MySqlBackend::Data
    {
        Data(): m_initialized(false) {}

        bool init() noexcept(true)
        {
            bool status = true;

            if (m_initialized == false)
            {
                auto entry = ConfigurationFactory::get()->findEntry(Database::databaseLocation);

                //create directories
                if (entry)
                {
                    boost::filesystem::path storage(entry->value());

                    if (boost::filesystem::exists(storage) == false)
                        status = boost::filesystem::create_directories(storage);
                }

                //start mysql process


                m_initialized = status;
            }

            return status;
        }

        private:
            bool m_initialized;
    };


    MySqlBackend::MySqlBackend(): m_data(new Data)
    {

    }


    MySqlBackend::~MySqlBackend()
    {
    }


    bool MySqlBackend::store(const Entry &entry)
    {
        (void) entry;
        return true;
    }


    bool MySqlBackend::init()
    {
        return m_data->init();
    }
}
