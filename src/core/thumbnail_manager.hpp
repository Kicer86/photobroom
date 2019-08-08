/*
 * Class responsible for managing thumbnails
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef THUMBNAILMANAGER_HPP
#define THUMBNAILMANAGER_HPP

#include <memory>

#include <QImage>

struct AThumbnailGenerator
{
    virtual ~AThumbnailGenerator() = default;

    template<typename C>
    void generate(const QString& path, int desired_height, C&& c)
    {
        struct Task: ITask
        {
            Task(C&& c): m_c(std::move(c)) {}

            void result(const QImage& result) override
            {
                m_c(result);
            }

            C m_c;
        };

        run(path, desired_height, std::make_unique<Task>(std::move(c)));
    }

    protected:
        struct ITask
        {
            virtual ~ITask() = default;

            virtual void result(const QImage &) = 0;
        };

        virtual void run(const QString &, int, std::unique_ptr<ITask>) = 0;
};

class ThumbnailManager
{
    public:
        ThumbnailManager();

        template<typename C>
        void fetch(const QString& path, int desired_height, C&& c)
        {
            const QImage cached = find(path, desired_height);

            if (cached.isNull())
                m_generator->generate(path, desired_height, [this, &c, desired_height, path] (const QImage& img)
                {
                    assert(img.height() == desired_height);
                    cache(path, img);
                    c(desired_height, img);
                });
            else
                c(desired_height, cached);
        }

    private:
        AThumbnailGenerator* m_generator;

        QImage find(const QString &, int);
        void cache(const QString &, const QImage &);
};

#endif // THUMBNAILMANAGER_HPP
