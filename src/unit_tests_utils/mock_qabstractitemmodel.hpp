
#ifndef MOCK_QABSTRACTITEMMODEL_HPP
#define MOCK_QABSTRACTITEMMODEL_HPP

#include <QAbstractItemModel>

#include <gmock/gmock.h>


struct MockQAbstractItemModel: QAbstractItemModel
{
    MOCK_CONST_METHOD3(index, QModelIndex(int, int, const QModelIndex &));
    MOCK_CONST_METHOD1(parent, QModelIndex(const QModelIndex &));
    MOCK_CONST_METHOD1(rowCount, int(const QModelIndex &));
    MOCK_CONST_METHOD1(columnCount, int(const QModelIndex &));
    MOCK_CONST_METHOD2(data, QVariant(const QModelIndex &, int));

    // for convenience:
    QModelIndex constructIndex(int r, int c, void* ptr = nullptr)
    {
        return createIndex(r, c, ptr);
    }
};


struct StubQAbstractItemModel: MockQAbstractItemModel
{
    QModelIndex addItem(int r, int c, const QModelIndex& p)
    {
        const QModelIndex item = createIndex(r, c, reinterpret_cast<void *>(++i));

        ON_CALL(*this, parent(item))
            .WillByDefault(testing::Return(p));

        ON_CALL(*this, index(r, c, p))
            .WillByDefault(testing::Return(item));

        return item;
    }

    int i = 0;
};

#endif
