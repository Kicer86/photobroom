
#ifndef IPHOTOINFORMATION_HPP
#define IPHOTOINFORMATION_HPP


#include <QSize>


struct IPhotoInformation
{
    virtual ~IPhotoInformation() = default;

    virtual QSize size(const QString &) const = 0;
};

#endif
