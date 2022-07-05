
#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <QObject>

#include "file_info.hpp"


class Utils: public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE static FileInfo* fileInfo();
};

#endif
