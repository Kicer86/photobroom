/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef PEOPLEMANIPULATOR_HPP
#define PEOPLEMANIPULATOR_HPP

#include <core/core_factory_accessor.hpp>
#include <core/function_wrappers.hpp>
#include <core/ilogger.hpp>
#include <core/oriented_image.hpp>
#include <database/photo_types.hpp>
#include <database/idatabase.hpp>


class IFace
{
public:
    virtual ~IFace() = default;

    virtual const QRect& rect() const = 0;
    virtual const QString& name() const = 0;
    virtual const OrientedImage& image() const = 0;

    virtual void setName(const QString &) = 0;
    virtual void store() = 0;
};


struct IFacesSaver;

class FaceEditor
{
public:
    FaceEditor(Database::IDatabase &, ICoreFactoryAccessor &, const ILogger &);

    std::vector<std::unique_ptr<IFace>> getFacesFor(const Photo::Id &);

private:
    std::weak_ptr<IFacesSaver> m_facesSaver;
    std::unique_ptr<ILogger> m_logger;
    Database::IDatabase& m_db;
    ICoreFactoryAccessor& m_core;
};

#endif
