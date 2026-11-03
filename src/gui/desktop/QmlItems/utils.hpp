
#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <QObject>

#include <core/filesystem.hpp>


class Utils: public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE static bool openContainingDirectory(const Filesystem::Location& photo);
};

#endif
