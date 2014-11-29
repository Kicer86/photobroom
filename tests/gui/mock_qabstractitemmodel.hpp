
#ifndef MOCK_QABSTRACTITEMMODEL_HPP
#define MOCK_QABSTRACTITEMMODEL_HPP

#include <gmock/gmock.h>

#include <QAbstractItemModel>

struct MockQAbstractItemModel: QAbstractItemModel
{
    MOCK_CONST_METHOD3(index, QModelIndex(int, int, const QModelIndex &));
    MOCK_CONST_METHOD1(parent, QModelIndex(const QModelIndex &));
    MOCK_CONST_METHOD1(rowCount, int(const QModelIndex &));
    MOCK_CONST_METHOD1(columnCount, int(const QModelIndex &));
    MOCK_CONST_METHOD2(data, QVariant(const QModelIndex &, int));
};

#endif
