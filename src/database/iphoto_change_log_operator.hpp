
#ifndef IPHOTO_CHANGE_LOG_OPERATOR_HPP
#define IPHOTO_CHANGE_LOG_OPERATOR_HPP


#include <QStringList>

namespace Database
{
    struct IPhotoChangeLogOperator
    {
        virtual ~IPhotoChangeLogOperator() = default;

        // for debug / tests
        virtual QStringList dumpChangeLog() = 0;
    };
}

#endif
