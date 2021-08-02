
#ifndef DATAFROMPATHEXTRACTOR_HPP
#define DATAFROMPATHEXTRACTOR_HPP

#include <QRegularExpression>

#include <core/tag.hpp>


class DataFromPathExtractor
{
    public:
        DataFromPathExtractor();

        Tag::TagsList extract(const QString& path);

    private:
        QRegularExpression m_expr;
};


#endif // DATAFROMPATHEXTRACTOR_HPP
