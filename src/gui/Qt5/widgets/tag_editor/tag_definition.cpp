
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
}


TagDefinition::~TagDefinition()
{

}
