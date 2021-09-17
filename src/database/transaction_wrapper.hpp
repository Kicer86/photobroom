
#ifndef ATRANSACTION_HPP_INCLUDED
#define ATRANSACTION_HPP_INCLUDED

#include "itransaction.hpp"


namespace Database
{
    template<typename T>
    class TransactionWrapper: public Database::ITransaction, T
    {
        public:
            template<typename ...Args>
            TransactionWrapper(Args&... args): T(args...) {}

            ~TransactionWrapper()
            {
                if (m_abort)                    // on abort restore saved state
                    T::rollback();
                else
                    T::commit();
            }

            void abort() override
            {
                m_abort = true;
            }

        private:
            bool m_abort = false;
    };


    template<typename T>
    class TransactionManager
    {
        public:
            template<typename ...Args>
            std::shared_ptr<ITransaction> openTransaction(Args&... args)
            {
                auto tr = m_tr.lock();

                if (tr.get() == nullptr)
                {
                    tr = std::make_shared<TransactionWrapper<T>>(args...);
                    m_tr = tr;
                }

                return tr;
            }

        private:
            std::weak_ptr<TransactionWrapper<T>> m_tr;
    };
}

#endif
