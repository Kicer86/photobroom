
#ifndef ACTIONSMODEL_HPP
#define ACTIONSMODEL_HPP

#include <QAbstractListModel>
#include <QAction>


class ActionsModel: public QAbstractListModel
{
public:
    QVariant data(const QModelIndex & index, int role) const override;
    int rowCount(const QModelIndex & parent) const override;

    void addAction(QAction *);
    void clear();

private:
    std::vector<QAction *> m_actions;
};

#endif
