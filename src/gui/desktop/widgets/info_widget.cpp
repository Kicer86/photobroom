/*
 * Widget with tips
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

#include "info_widget.hpp"

#include <QPropertyAnimation>
#include <QVBoxLayout>

InfoBaloonWidget::InfoBaloonWidget(QWidget* parent_widget):
    QLabel(parent_widget),
    m_animated(false)
{
    setMargin(4);
    setFrameStyle(NoFrame);

    //style
    setStyleSheet(
        "QLabel {                           "
        "   border-radius: 13px;            "
        "   background:    CornflowerBlue;  "
        "   color:         White;           "
        "   padding:       5px;             "
        "}                                  "
    );
}


InfoBaloonWidget::~InfoBaloonWidget()
{

}


void InfoBaloonWidget::enableAnimations(bool animate)
{
    m_animated = animate;
}


void InfoBaloonWidget::showEvent(QShowEvent* event)
{
    if (m_animated)
    {
        const QSize sizeHint = this->sizeHint();

        QPropertyAnimation *animation = new QPropertyAnimation(this, "maximumHeight");
        animation->setDuration(100);
        animation->setStartValue(0);
        animation->setEndValue(sizeHint.height());

        animation->start();

        connect(animation, &QAbstractAnimation::finished, animation, &QObject::deleteLater);
    }

    QLabel::showEvent(event);
}
