#ifndef PHOTO_DELTA_FETCHER_BINDING_HPP
#define PHOTO_DELTA_FETCHER_BINDING_HPP

#include <memory>
#include <utility>

#include <QObject>

#include <database/database_tools/photo_delta_fetcher.hpp>


namespace Gui::Utils
{
    template<typename Receiver, typename Slot>
    class PhotoDeltaFetcherBinding
    {
    public:
        PhotoDeltaFetcherBinding(Receiver& receiver, Slot slot)
            : m_receiver(receiver)
            , m_slot(slot)
        {
        }

        void setDatabase(Database::IDatabase* database)
        {
            if (database != nullptr)
            {
                auto fetcher = std::make_unique<PhotoDeltaFetcher>(*database);

                QObject::connect(fetcher.get(), &PhotoDeltaFetcher::photoDataDeltaFetched,
                                 &m_receiver, m_slot);

                m_fetcher = std::move(fetcher);
            }
            else
                m_fetcher.reset();
        }

        void fetchIds(const std::vector<Photo::Id>& ids, const std::set<Photo::Field>& fields)
        {
            m_fetcher->fetchIds(ids, fields);
        }

    private:
        Receiver& m_receiver;
        Slot m_slot;
        std::unique_ptr<PhotoDeltaFetcher> m_fetcher;
    };
}

#endif
