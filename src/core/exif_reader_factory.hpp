
#ifndef TAG_FEEDER_FACTORY
#define TAG_FEEDER_FACTORY

#include <memory>
#include <map>
#include <thread>

#include "iexif_reader.hpp"

#include "core_export.h"

struct IPhotosManager;

class CORE_EXPORT ExifReaderFactory: public IExifReaderFactory
{
    public:
        ExifReaderFactory();
        ExifReaderFactory(const ExifReaderFactory &) = delete;

        ExifReaderFactory& operator=(const ExifReaderFactory &) = delete;

        void set(IPhotosManager *);

        // ITagFeederFactory:
        std::shared_ptr<IExifReader> get() override;

    private:
        std::map<std::thread::id, std::shared_ptr<IExifReader>> m_feeders;
        IPhotosManager* m_photosManager;
};

#endif
