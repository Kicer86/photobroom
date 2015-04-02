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

#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>

#include "itag_value_widget.hpp"


struct EmptyEdit: QWidget, ITagValueWidget
{
    explicit EmptyEdit(QWidget* p = 0): QWidget(p) {}
    
    QString getValue() const override
    {
        return "";
    }
    
    void setValue(const QString &) override
    {
        
    }
    
    QWidget* getWidget() override
    {
        return this;
    }
};


struct LineEdit: QLineEdit, ITagValueWidget
{
    explicit LineEdit(QWidget* p = 0): QLineEdit(p) {}
    
    QString getValue() const override
    {
        return QLineEdit::text();
    }
    
    void setValue(const QString& t) override
    {
        QLineEdit::setText(t);
    }
    
    QWidget* getWidget() override
    {
        return this;
    }
};


struct DataEdit: QDateEdit, ITagValueWidget
{
    explicit DataEdit(QWidget* p = 0): QDateEdit(p) {}
    
    QString getValue() const override
    {
        return QDateEdit::date().toString();
    }
    
    void setValue(const QString& t) override
    {
        QDateEdit::setDate(QDate::fromString(t));
    }
    
    QWidget* getWidget() override
    {
        return this;
    }
};


struct TimeEdit: QTimeEdit, ITagValueWidget
{
    explicit TimeEdit(QWidget* p = 0): QTimeEdit(p) {}
    
    QString getValue() const override
    {
        return QTimeEdit::time().toString();
    }
    
    void setValue(const QString& t) override
    {
        QTimeEdit::setTime(QTime::fromString(t));
    }
    
    QWidget* getWidget() override
    {
        return this;
    }
};


///////////////////////////////////////////////////////////////////////////////


TagValueWidgetFactory::TagValueWidgetFactory()
{

}


TagValueWidgetFactory::~TagValueWidgetFactory()
{

}


std::unique_ptr<ITagValueWidget> TagValueWidgetFactory::construct(const TagNameInfo::Type& type)
{
    ITagValueWidget* result = nullptr;
    
    switch(type)
    {
        case TagNameInfo::Invalid: result = new EmptyEdit; break;
        case TagNameInfo::Text:    result = new LineEdit;  break;
        case TagNameInfo::Date:    result = new DataEdit;  break;
        case TagNameInfo::Time:    result = new TimeEdit;  break;
    }
    
    return std::unique_ptr<ITagValueWidget>(result);
}
