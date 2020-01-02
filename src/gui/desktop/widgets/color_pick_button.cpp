/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "color_pick_button.hpp"

#include <QColorDialog>


ColorPickButton::ColorPickButton(QWidget* p): QPushButton(p), m_color(QColor(255, 255, 255))
{
    applyColor();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::QSizePolicy::Fixed);

    connect(this, &QPushButton::pressed, this, &ColorPickButton::pickColor);
}


ColorPickButton::~ColorPickButton()
{

}


void ColorPickButton::setColor(const QColor& c)
{
    m_color = c;

    applyColor();
}


const QColor& ColorPickButton::getColor() const
{
    return m_color;
}


void ColorPickButton::applyColor()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(m_color);

    setIcon(pixmap);
}


void ColorPickButton::pickColor()
{
    QColor result = QColorDialog::getColor(m_color, nullptr, "", QColorDialog::ShowAlphaChannel);

    if (result.isValid())
    {
        m_color = result;

        applyColor();
    }
}
