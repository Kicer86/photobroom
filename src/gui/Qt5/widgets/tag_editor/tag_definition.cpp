
#include "tag_definition.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>

#include "converter.hpp"

TagDefinition::TagDefinition(QWidget* p, Qt::WindowFlags f): 
    QWidget(p, f), 
    m_comboBox(nullptr),
    m_button(nullptr)
{    
    m_button = new QPushButton("+", this);
    m_comboBox = new QComboBox(this);
    QHBoxLayout* lay = new QHBoxLayout(this);
    
    lay->addWidget(m_comboBox);
    lay->addWidget(m_button);
    
    m_comboBox->setEditable(true);
    m_button->setEnabled(false);
    
    connect(m_button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
    connect(m_comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(comboChanged(QString)));

    enable(false);
}


TagDefinition::~TagDefinition()
{

}


void TagDefinition::setModel(QAbstractItemModel* model) const
{
    m_comboBox->setModel(model);
}


void TagDefinition::enable(bool e)
{
    m_button->setEnabled(e);
    m_comboBox->setEnabled(e);
}


void TagDefinition::buttonPressed() const
{
    const int idx = m_comboBox->currentIndex();
    //TODO: react on invalid index
    
    TagInfo info = Converter::convert(m_comboBox);

    emit tagChoosen(info);
}


void TagDefinition::comboChanged(const QString &txt) const
{
    m_button->setDisabled(txt.isEmpty());
}
