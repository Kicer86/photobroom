
#include "exif_reader_factory.hpp"

#include <mutex>
#include <thread>

#include "exiv2_exif_reader.hpp"

#include "tag.hpp"
#include "iexif_reader.hpp"

namespace
{
    std::recursive_mutex xmpMutex;
}


ExifReaderFactory::ExifReaderFactory(): m_feeders()
{
    // Pass the locking mechanism to the XMP parser on initialization.
    // Note however that this call itself is still not thread-safe.
    Exiv2::XmpParser::initialize(
        [](void *data, bool doLock) {
            std::recursive_mutex *mutex = static_cast<std::recursive_mutex*>(data);
            if (doLock) {
                mutex->lock();
            } else {
                mutex->unlock();
            }
        },
        &xmpMutex
    );
}


IExifReader* ExifReaderFactory::get()
{
    //ExifTool may not be thread safe. Prepare separate object for each thread
    const auto id = std::this_thread::get_id();
    auto it = m_feeders.find(id);

    if (it == m_feeders.end())
    {
        auto feeder = std::make_unique<Exiv2ExifReader>();
        auto in = m_feeders.emplace(id, std::move(feeder));

        it = in.first;
    }

    return it->second.get();
}
