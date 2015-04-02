 
/*
* tag value widgets
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
 

#ifndef TAG_VALUE_WIDGETS_HPP
#define TAG_VALUE_WIDGETS_HPP

#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>

struct EmptyEdit: QWidget
{
    explicit EmptyEdit(QWidget* p = 0): QWidget(p) {}
    
    Q_INVOKABLE QString getValue() const
    {
        return "";
    }
    
    Q_OBJECT
};


struct LineEdit: QLineEdit
{
    explicit LineEdit(QWidget* p = 0): QLineEdit(p) {}
    
    Q_INVOKABLE QString getValue() const
    {
        return QLineEdit::text();
    }
    
    Q_OBJECT
};


struct DataEdit: QDateEdit
{
    explicit DataEdit(QWidget* p = 0): QDateEdit(p) {}
    
    Q_INVOKABLE QString getValue() const
    {
        return QDateEdit::date().toString();
    }
    
    Q_OBJECT
};


struct TimeEdit: QTimeEdit
{
    explicit TimeEdit(QWidget* p = 0): QTimeEdit(p) {}
    
    Q_INVOKABLE QString getValue() const
    {
        return QTimeEdit::time().toString();
    }

    Q_OBJECT
};

#endif // TAG_VALUE_WIDGETS_HPP
