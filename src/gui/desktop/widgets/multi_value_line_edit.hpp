/*
 * Multi value line edit - for multi completition
 * Copyright (C) 2016  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef MULTIVALUELINEEDIT_HPP
#define MULTIVALUELINEEDIT_HPP

#include <QLineEdit>


class MultiValueLineEdit: public QLineEdit
{
    public:
        MultiValueLineEdit(const QString& separator, QWidget* parent);
        MultiValueLineEdit(const MultiValueLineEdit &) = delete;

        MultiValueLineEdit& operator=(const MultiValueLineEdit &) = delete;

        void setCompleter(QCompleter *);

    protected:
        void keyPressEvent(QKeyEvent *e) override;

    private:
        QString m_separator;
        QCompleter* m_completer;

        std::pair<int, int> currentWordPos() const;
        void insertCompletion(const QString &);
};

#endif // MULTIVALUELINEEDIT_HPP
