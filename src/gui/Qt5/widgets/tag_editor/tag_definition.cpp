
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
    
    m_comboBox->setEditable(true);
    m_button->setEnabled(false);
    
    connect(m_button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
    connect(m_comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(comboChanged(QString)));
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


void TagDefinition::comboChanged(const QString &txt) const
{
    m_button->setDisabled(txt.isEmpty());
}
