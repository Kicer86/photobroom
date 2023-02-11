
#ifndef APHOTOCHANGELOGOPERATOR_HPP
#define APHOTOCHANGELOGOPERATOR_HPP

#include "iphoto_change_log_operator.hpp"
#include "database_export.h"

namespace Database
{
    class DATABASE_EXPORT APhotoChangeLogOperator: public IPhotoChangeLogOperator
    {
        public:
            void storeDifference(const DiffDelta &, const Photo::DataDelta &) override;
            void groupCreated(const Group::Id &, const Group::Type &, const Photo::Id& representative) override;
            void groupDeleted(const Group::Id &, const Photo::Id& representative, const std::vector<Photo::Id>& members) override;

        protected:
            enum Operation
            {
                Add     = 1,
                Modify  = 2,
                Remove  = 3,
            };

            enum Field
            {
                Tags    = 1,
                Group   = 2,
            };

            virtual void append(const Photo::Id &, Operation, Field, const QString& data) = 0;
            void process(const Photo::Id &, const Tag::TagsList &, const Tag::TagsList &);
            void process(const Photo::Id &, const GroupInfo &, const GroupInfo &);

            QString format(int, Operation, Field, const QString &);

        private:
            QString fieldToStr(Field);
            QString opToStr(Operation);
            QString dataToStr(Field, Operation, const QString &);
    };
}

#endif // APHOTOCHANGELOGOPERATOR_HPP
