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

#include <memory>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "text_widget.hpp"

namespace
{
    struct IType
    {
        virtual ~IType() {}
        virtual IValueWidget* construct() = 0;
    };
    
    template<typename T>
    struct TypeData: public IType
    {
        virtual ~TypeData() {}
        T* construct() { return new T; }
    };
    
    std::map<TagNameInfo::Type, std::shared_ptr<IType>> typesMap = 
    { 
        {TagNameInfo::Type::Text, std::shared_ptr<IType>(new TypeData<TextWidget>) },
        {TagNameInfo::Type::Time, std::shared_ptr<IType>(new TypeData<TextWidget>) },
        {TagNameInfo::Type::Date, std::shared_ptr<IType>(new TypeData<TextWidget>) }
    };
}


TagNameInfo::Type TagInfo::defaultType()
{
    return TagNameInfo::Text;
}


IValueWidget* TagInfo::construct(const TagNameInfo::Type& type)
{
    auto& t = typesMap[type];
    
    IValueWidget* result = t->construct();
    
    return result;
}


/***********************************************************************/


TagEntry::TagEntry(const TagNameInfo& tagInfo, QWidget *p, Qt::WindowFlags f):
    m_tagName(nullptr),
    m_tagValue(nullptr),
    m_tagInfo(tagInfo)
{
    m_tagName = new QLabel(tagInfo.getName(), this);
    
    m_tagValue = TagInfo::construct(tagInfo.getType());
    assert(m_tagValue != nullptr);
    m_tagValue->setParent(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagName);
    mainLayout->addWidget(m_tagValue->getWidget());
    
    connect(m_tagValue, SIGNAL(changed()), this, SIGNAL(tagEdited()));
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


TagNameInfo TagEntry::getTagInfo() const
{
    return m_tagInfo;
}


QString TagEntry::getTagValue() const
{   
    const QString result = m_tagValue->getValue();
    
    return result;
}
