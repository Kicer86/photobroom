
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

        QImage merge(QList<QImage>::iterator, QList<QImage>::iterator) const;
        QImage merge(const QImage& lhs, const QImage& rhs) const;
        void adjustHorizontalSizes(QImage& lhs, QImage& rhs) const;
        void adjustVerticalSizes(QImage& lhs, QImage& rhs) const;
};

#endif
