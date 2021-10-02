
#ifndef ITRANSACTION_HPP_INCLUDED
#define ITRANSACTION_HPP_INCLUDED

namespace Database
{
    class ITransaction
    {
        public:
            virtual ~ITransaction() = default;

            virtual void abort() = 0;
    };
}

#endif
