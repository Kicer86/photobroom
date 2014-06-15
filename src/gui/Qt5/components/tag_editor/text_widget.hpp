/*
    <one line to give the program's name and a brief idea of what it does.>
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


#ifndef TEXT_WIDGET_H
#define TEXT_WIDGET_H


#include <QLineEdit>

#include "ivalue_widget.hpp"


class TextWidget: public IValueWidget
{
    public:
        TextWidget();
        TextWidget(const TextWidget &) = delete;
        virtual ~TextWidget();

        TextWidget& operator=(const TextWidget &) = delete;

        //IValueWidget:
        virtual QWidget* getWidget() override;
        virtual QString getValue() const override;
        virtual void clear() override;
        virtual void setParent(QWidget* ) override;
        virtual void setValue(const QString& ) override;

    private:
        QLineEdit* m_lineEdit;
};

#endif // TEXT_WIDGET_H
