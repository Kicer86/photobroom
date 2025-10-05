#ifndef PHOTO_DELTA_FETCHER_SETUP_HPP
#define PHOTO_DELTA_FETCHER_SETUP_HPP

#include <memory>
#include <QObject>

#include <database/database_tools/photo_delta_fetcher.hpp>

namespace Gui
{
    namespace Utils
    {
        template<typename Receiver, typename Slot>
        void resetPhotoDeltaFetcher(std::unique_ptr<PhotoDeltaFetcher>& fetcher,
                                    Database::IDatabase* database,
                                    Receiver* receiver,
                                    Slot slot)
        {
            if (database)
            {
                auto translator = std::make_unique<PhotoDeltaFetcher>(*database);

                QObject::connect(translator.get(), &PhotoDeltaFetcher::photoDataDeltaFetched,
                                 receiver, slot);

                fetcher = std::move(translator);
            }
            else
                fetcher.reset();
        }
    }
}

#endif // PHOTO_DELTA_FETCHER_SETUP_HPP
