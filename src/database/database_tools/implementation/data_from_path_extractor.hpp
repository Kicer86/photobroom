
#ifndef DATAFROMPATHEXTRACTOR_HPP
#define DATAFROMPATHEXTRACTOR_HPP

#include <QRegularExpression>

#include <core/tag.hpp>


class DataFromPathExtractor
{
    public:
        DataFromPathExtractor();

        Tag::TagsList extract(const QString& path) const;

    private:
        QRegularExpression m_expr1;
        QRegularExpression m_expr2;

        Tag::TagsList extractRegex1(const QString& path) const;
        Tag::TagsList extractRegex2(const QString& path) const;
};


#endif // DATAFROMPATHEXTRACTOR_HPP
