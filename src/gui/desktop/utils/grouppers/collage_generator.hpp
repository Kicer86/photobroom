
#ifndef COLLAGEGENERATOR_HPP
#define COLLAGEGENERATOR_HPP

#include <QStringList>

#include <core/iexif_reader.hpp>


class CollageGenerator
{
    public:
        CollageGenerator(IExifReader &);

        QImage generateCollage(const QStringList& paths) const;

    private:
        IExifReader& m_exifReader;

        QImage merge(const QList<QImage> &) const;
};

#endif
