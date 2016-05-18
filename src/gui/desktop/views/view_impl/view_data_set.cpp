
#include "view_data_set.hpp"


void for_each_child(QAbstractItemModel* model, const QModelIndex& parent, std::function<void(const QModelIndex &)> function)
{
    int i = 0;
    for(QModelIndex c_idx = model->index(i, 0, parent); c_idx.isValid(); c_idx = model->index(++i, 0, parent))
        function(c_idx);
}


void for_each_child_deep(QAbstractItemModel* model, const QModelIndex& parent, std::function<void(const QModelIndex &)> function)
{
    int i = 0;
    for(QModelIndex c_idx = model->index(i, 0, parent); c_idx.isValid(); c_idx = model->index(++i, 0, parent))
    {
        for_each_child_deep(model, c_idx, function);
        function(c_idx);
    }
}


IViewDataSet::~IViewDataSet()
{

}
