
#ifndef TAG_FEEDER_FACTORY
#define TAG_FEEDER_FACTORY

#include <memory>
#include <map>
#include <thread>

#include "iexif_reader.hpp"

#include "core_export.h"


class CORE_EXPORT ExifReaderFactory: public IExifReaderFactory
{
    public:
        ExifReaderFactory();
        ExifReaderFactory(const ExifReaderFactory &) = delete;

        ExifReaderFactory& operator=(const ExifReaderFactory &) = delete;

        // ITagFeederFactory:
        IExifReader& get() override;

    private:
        std::map<std::thread::id, std::unique_ptr<IExifReader>> m_feeders;
};

#endif
