/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef GUI_LEVELEDITOR_H
#define GUI_LEVELEDITOR_H

#include <vector>

#include <QWidget>

class QHBoxLayout;
class QLabel;

class LevelEditor : public QWidget
{
        Q_OBJECT
    
    public:
        LevelEditor(QWidget * = nullptr);
        LevelEditor(const LevelEditor &) = delete;
        ~LevelEditor();
        LevelEditor& operator=(const LevelEditor &) = delete;

        void setLevelNames(const std::vector<QString> &);

    private:
        QHBoxLayout* m_notUsedItemsLayout;
        std::vector<QLabel *> m_levels;

        virtual void mousePressEvent(QMouseEvent *) override;
};

#endif // GUI_LEVELEDITOR_H
