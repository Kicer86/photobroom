
#ifndef MOCK_QABSTRACTITEMMODEL_HPP
#define MOCK_QABSTRACTITEMMODEL_HPP

#include <gmock/gmock.h>

#include <QAbstractItemModel>


struct Item
{
    std::vector<Item> children;
};

struct MockQAbstractItemModel: QAbstractItemModel
{
    MOCK_CONST_METHOD3(index, QModelIndex(int, int, const QModelIndex &));
    MOCK_CONST_METHOD1(parent, QModelIndex(const QModelIndex &));
    MOCK_CONST_METHOD1(rowCount, int(const QModelIndex &));
    MOCK_CONST_METHOD1(columnCount, int(const QModelIndex &));
    MOCK_CONST_METHOD2(data, QVariant(const QModelIndex &, int));

    QModelIndex createIndex(int row, int column, void * ptr = 0) const
    {
        return QAbstractItemModel::createIndex(row, column, ptr);
    }

    void define_hierarchy(const Item& topItem)
    {
        m_item = topItem;
    }

    Item m_item;
};

#endif
