
#ifndef SERIESMODEL_HPP
#define SERIESMODEL_HPP

#include <QAbstractItemModel>


#include "database_export.h"

class DATABASE_EXPORT SeriesModel: public QAbstractListModel
{
public:
    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;

};

#endif
