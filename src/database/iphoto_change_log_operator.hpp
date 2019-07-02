
#ifndef IPHOTO_CHANGE_LOG_OPERATOR_HPP
#define IPHOTO_CHANGE_LOG_OPERATOR_HPP


#include <QStringList>


namespace Database
{
    struct IPhotoChangeLogOperator
    {
        virtual ~IPhotoChangeLogOperator() = default;

        virtual void storeDifference(const Photo::Data &, const Photo::DataDelta &) = 0;

        // for debug / tests
        virtual QStringList dumpChangeLog() = 0;
    };
}

#endif
