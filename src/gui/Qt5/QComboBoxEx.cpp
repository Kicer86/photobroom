
#include "QComboBoxEx.hpp"

#include <QAbstractItemModel>


ModelDeleter::ModelDeleter(QComboBoxEx *combo): m_combo(combo)
{

}


void ModelDeleter::operator()(QAbstractItemModel* model)
{
    m_combo->disconnect(model);
}


QComboBoxEx::QComboBoxEx(QWidget* parent): QComboBox(parent), m_model(nullptr, ModelDeleter(this))
{

}


QComboBoxEx::~QComboBoxEx()
{

}


void QComboBoxEx::setDropValues(QAbstractItemModel* model)
{    
    m_model.reset(model);
    
    connect(model, SIGNAL(dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &)),
            this, SLOT(dataChanged()));
}


void QComboBoxEx::dataChanged()
{

}

