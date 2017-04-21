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

#ifndef INFOWIDGET_HPP
#define INFOWIDGET_HPP

#include <QLabel>

class InfoBalloonWidget: public QLabel
{
    public:
        InfoBalloonWidget (QWidget* parent = nullptr);
        InfoBalloonWidget (const InfoBalloonWidget &) = delete;
        ~InfoBalloonWidget();

        InfoBalloonWidget& operator=(const InfoBalloonWidget &) = delete;

        void enableAnimations(bool);
        void autoHide(bool);

        void hide();

        // QWidget:
        void focusOutEvent(QFocusEvent *) override;
        void showEvent(QShowEvent *) override;

    private:
        int m_animationSpeed;
        bool m_animated;
        bool m_autoHide;
};

#endif // INFOWIDGET_HPP
