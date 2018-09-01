/*
 * Tool for people's face reviewing
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef FACEREVIEWER_HPP
#define FACEREVIEWER_HPP

#include <QDialog>

#include <core/callback_ptr.hpp>
#include <database/person_data.hpp>


namespace Database
{
    struct IDatabase;
    struct IBackendOperator;
}


class FaceReviewer: public QDialog
{
        Q_OBJECT

    public:
        FaceReviewer(Database::IDatabase *, QWidget *);
        ~FaceReviewer();

    private:
        safe_callback_ctrl m_safe_callback;
        Database::IDatabase* m_db;
        QWidget* m_canvas;

        void fetchPeople(Database::IBackendOperator *) const;

        void updatePeople(const std::map<PersonName, std::vector<PersonInfo>> &,
                          const std::map<Photo::Id, QString> &);

    // internal signals
    signals:
        void gotPeopleInfo(const std::map<PersonName, std::vector<PersonInfo>> &,
                           const std::map<Photo::Id, QString> &) const;
};

#endif // FACEREVIEWER_HPP
