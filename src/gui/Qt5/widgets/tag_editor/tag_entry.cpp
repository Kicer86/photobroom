/*
    TagEntry class
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "tag_entry.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


TagEntry::TagEntry(const QString &name, QWidget *p, Qt::WindowFlags f):
    m_tagName(nullptr),
    m_tagValue(nullptr)
{
    m_tagName = new QLabel(name, this);
    m_tagValue  = new QLineEdit(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagName);
    mainLayout->addWidget(m_tagValue);
    
    connect(m_tagValue, SIGNAL(textEdited(QString)), this, SIGNAL(tagEdited()));
}


TagEntry::~TagEntry()
{

}


void TagEntry::setTagValue(const QString &value)
{
    m_tagValue->setText(value);
}


void TagEntry::clear()
{
    m_tagValue->clear();
    m_tagName->clear();

    //for (const QString &tag: m_baseTags)
    //    m_tagsCombo->addItem(tag);
}


QString TagEntry::getTagName() const
{
    const QString result = m_tagName->text();    
    return result;
}


QString TagEntry::getTagValue() const
{   
    const QString result = m_tagValue->text();
    
    return result;
}
