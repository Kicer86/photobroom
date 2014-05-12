/*
    Converting stuff
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


#include "converter.hpp"

#include <QStandardItem>
#include <QComboBox>

#include "tag_entry.hpp"

QStandardItem* Converter::convert(const TagNameInfo& tagInfo)
{
    QStandardItem* item = new QStandardItem(tagInfo.getName());    
    item->setData( tagInfo.getType(), Qt::UserRole );
    item->setData( tagInfo.getSeparator(), Qt::UserRole + 1 );
    
    return item;
}


TagNameInfo Converter::convert(QComboBox *combo)
{
    const int idx = combo->currentIndex();
    const QString itemText = combo->itemText(idx);
    const QVariant typeInfoVar = combo->itemData(idx, Qt::UserRole);
    const QVariant separatorVar = combo->itemData(idx, Qt::UserRole + 1);

    const TagNameInfo::Type typeInfo = typeInfoVar == QVariant::Invalid? 
                                       TagInfo::defaultType(): 
                                       static_cast<TagNameInfo::Type>(typeInfoVar.toInt());

    //TODO: use default separator as variable
    const char separator = separatorVar == QVariant::Invalid? 
                           ';': separatorVar.toChar().toLatin1();
                                       
    TagNameInfo info(itemText, typeInfo, separator);

    return info;
}

