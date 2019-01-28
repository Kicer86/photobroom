/*
 * Widget with details about person
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

#ifndef FACEDETAILS_HPP
#define FACEDETAILS_HPP

#include <functional>

#include <QGroupBox>

#include <database/person_data.hpp>


class QLabel;
class QPushButton;

struct ITaskExecutor;
class PeopleOperator;

struct IModelFaceFinder
{
    virtual ~IModelFaceFinder() = default;

    virtual void findBest(const std::vector<PersonInfo> &,
                          const std::function<void(const QString &)> &) = 0;

    virtual QString currentBest(const Person::Id &) const = 0;
};


class FaceDetails: public QGroupBox
{
        Q_OBJECT

    public:
        FaceDetails(const QString &,
                    IModelFaceFinder *,
                    ITaskExecutor *,
                    const std::vector<PersonInfo> &,
                    QWidget *);

        ~FaceDetails();

    private:
        const std::vector<PersonInfo> m_pi;
        QString m_name;
        ITaskExecutor* m_executor;
        QPushButton* m_optButton;
        QLabel* m_photo;
        IModelFaceFinder* m_modelFaceFinder;
        bool m_fetched;

        void setModelPhoto(const QPixmap &);
        void setModelPhoto(const QImage &);

        void optimize();
        void updateRepresentative(const QString &);

        void initialFetch();

        void paintEvent(QPaintEvent *) override;
};

#endif // FACEDETAILS_HPP
