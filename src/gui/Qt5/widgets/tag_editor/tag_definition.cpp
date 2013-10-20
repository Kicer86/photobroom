
#include "tag_definition.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>

TagDefinition::TagDefinition(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{    
    m_button = new QPushButton("+", this);
    m_comboBox = new QComboBox(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    
    layout->addWidget(m_comboBox);
    layout->addWidget(m_button);
    
    connect(m_button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
}


TagDefinition::~TagDefinition()
{

}


void TagDefinition::setModel(QAbstractItemModel* model) const
{
    m_comboBox->setModel(model);
}


void TagDefinition::buttonPressed() const
{
    const QString name = m_comboBox->currentText();
    emit tagChoosen(name);
}
