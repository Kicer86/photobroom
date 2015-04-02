/*
 * Factory for tag value widgets
 * Copyright (C) 2015  Michał <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "tag_value_widget_factory.hpp"

#include "tag_value_widgets.hpp"

TagValueWidgetFactory::TagValueWidgetFactory()
{

}


TagValueWidgetFactory::~TagValueWidgetFactory()
{

}


QWidget* TagValueWidgetFactory::construct(const TagNameInfo::Type& type)
{
    QWidget* result = nullptr;
    
    switch(type)
    {
        case TagNameInfo::Invalid: result = new EmptyEdit; break;
        case TagNameInfo::Text:    result = new LineEdit;  break;
        case TagNameInfo::Date:    result = new DataEdit;  break;
        case TagNameInfo::Time:    result = new TimeEdit;  break;
    }
    
    return result;
}
