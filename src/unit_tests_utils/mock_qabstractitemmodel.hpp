
#ifndef MOCK_QABSTRACTITEMMODEL_HPP
#define MOCK_QABSTRACTITEMMODEL_HPP

#include <QAbstractItemModel>

#include <gmock/gmock.h>


struct MockQAbstractItemModel: QAbstractItemModel
{
    MOCK_METHOD(QModelIndex, index,       (int, int, const QModelIndex &), (const, override));
    MOCK_METHOD(QModelIndex, parent,      (const QModelIndex &),           (const, override));
    MOCK_METHOD(int,         rowCount,    (const QModelIndex &),           (const, override));
    MOCK_METHOD(int,         columnCount, (const QModelIndex &),           (const, override));
    MOCK_METHOD(QVariant,    data,        (const QModelIndex &, int),      (const, override));

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

        auto it = m_items.find(p);
        if (it == m_items.end())
        {
            it = m_items.insert(p, 0);

            ON_CALL(*this, rowCount(p))
                .WillByDefault(testing::ReturnPointee(&(*it)));

            ON_CALL(*this, columnCount(p))
                .WillByDefault(testing::Return(1));
        }

        ++(*it);

        ON_CALL(*this, parent(item))
            .WillByDefault(testing::Return(p));

        ON_CALL(*this, index(r, c, p))
            .WillByDefault(testing::Return(item));

        return item;
    }

    QHash<QModelIndex, int> m_items;
    int i = 0;
};

#endif
