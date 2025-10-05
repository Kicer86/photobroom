#ifndef PHOTO_DELTA_FETCHER_BINDING_HPP
#define PHOTO_DELTA_FETCHER_BINDING_HPP

#include <memory>
#include <utility>

#include <QObject>

#include <database/database_tools/photo_delta_fetcher.hpp>

namespace Gui
{
    namespace Utils
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

            PhotoDeltaFetcher* get() const
            {
                return m_fetcher.get();
            }

            PhotoDeltaFetcher* operator->() const
            {
                return m_fetcher.get();
            }

            PhotoDeltaFetcher& operator*() const
            {
                return *m_fetcher;
            }

            explicit operator bool() const
            {
                return static_cast<bool>(m_fetcher);
            }

        private:
            Receiver& m_receiver;
            Slot m_slot;
            std::unique_ptr<PhotoDeltaFetcher> m_fetcher;
        };
    }
}

#endif // PHOTO_DELTA_FETCHER_BINDING_HPP
