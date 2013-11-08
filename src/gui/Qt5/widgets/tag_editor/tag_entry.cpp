/*
    TagEntry class which is responsible for displaying information
    about single tag.
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

#include <assert.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "ivalue_widget.hpp"


TagInfo::TagInfo(const std::initializer_list<QString>& data): name(), typeInfo()
{
    assert(data.size() == 2);
    
    name = *(data.begin());
    typeInfo = (data.begin() + 1)->toStdString();
}


TagInfo::TagInfo(const ITagData::TagInfo &coreTagInfo): name(), typeInfo()
{
    name = coreTagInfo.getTypeInfo().getName();
    
    switch (coreTagInfo.getTypeInfo().getType())
    {
        case TagNameInfo::Type::Text:
            typeInfo = "QLineEdit";
            break;
            
        case TagNameInfo::Type::Time:
            typeInfo = "QTimeEdit";
            break;
            
        case TagNameInfo::Type::Date:
            typeInfo = "QDateEdit";
            break;
    }
}


TagInfo::TagInfo(const QString& name, const std::string& type): name(name), typeInfo(type)
{

}


bool TagInfo::operator==(const TagInfo& other) const
{
    return name == other.name;
}


bool TagInfo::operator<(const TagInfo& other) const
{
    return name < other.name;
}


/***********************************************************************/


TagEntry::TagEntry(const TagInfo& tagInfo, QWidget *p, Qt::WindowFlags f):
    m_tagName(nullptr),
    m_tagValue(nullptr),
    m_tagInfo(tagInfo)
{
    m_tagName = new QLabel(tagInfo.name, this);
    
    int id = QMetaType::type(tagInfo.typeInfo.c_str());
    assert(id != 0);
    QObject* rawObject = static_cast<QObject *>(QMetaType::create(id));
    assert(rawObject != nullptr);
    m_tagValue = dynamic_cast<IValueWidget *>(rawObject);
    assert(m_tagValue != nullptr);
    m_tagValue->setParent(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagName);
    mainLayout->addWidget(m_tagValue->getWidget());
    
    connect(m_tagValue, SIGNAL(textEdited(QString)), this, SIGNAL(tagEdited()));
}


TagEntry::~TagEntry()
{

}


void TagEntry::setTagValue(const QString &value)
{
    m_tagValue->setValue(value);
}


void TagEntry::clear()
{
    m_tagValue->clear();
    m_tagName->clear();
}


TagInfo TagEntry::getTagInfo() const
{
    return m_tagInfo;
}


QString TagEntry::getTagValue() const
{   
    const QString result = m_tagValue->getValue();
    
    return result;
}
