/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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


#include "photo_properties.hpp"

#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>

#include <system/filesystem.hpp>


namespace
{
    QString geometryToStr(const QSize& geometry)
    {
        const QString result = geometry.isValid()?
            QObject::tr("%1×%2").arg(geometry.width()).arg(geometry.height()):
            QObject::tr("?");

        return result;
    }

    QString geometryToStr(const Photo::Data& photoInfo)
    {
        return geometryToStr(photoInfo.geometry);
    }

    QString cutPrj(const QString& path)
    {
        const QString result = path.left(5) == "prj:/"? path.mid(5): path;

        return result;
    }
}


PhotoPropertiesWidget::PhotoPropertiesWidget(QWidget* p):
    QScrollArea(p),
    m_locationLabel(new QLabel(this)),
    m_sizeLabel(new QLabel(this)),
    m_geometryLabel(new QLabel(this)),
    m_locationValue(new QLabel(this)),
    m_sizeValue(new QLabel(this)),
    m_geometryValue(new QLabel(this))
{
    QWidget* area = new QWidget(this);
    QGridLayout* l = new QGridLayout(area);

    l->addWidget(m_locationLabel, 0, 0);
    l->addWidget(m_sizeLabel, 1, 0);
    l->addWidget(m_geometryLabel, 2, 0);

    l->addWidget(m_locationValue, 0, 1);
    l->addWidget(m_sizeValue, 1, 1);
    l->addWidget(m_geometryValue, 2, 1);

    l->setColumnStretch(1, 1);
    l->setRowStretch(3, 1);

    setWidgetResizable(true);
    setWidget(area);

    m_locationValue->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
    m_sizeValue->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
    m_geometryValue->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

    m_locationValue->setWordWrap(true);
}


PhotoPropertiesWidget::~PhotoPropertiesWidget()
{

}


void PhotoPropertiesWidget::setPhotos(const std::vector<Photo::Data>& photos)
{
    refreshLabels(photos);
    refreshValues(photos);
}


void PhotoPropertiesWidget::refreshLabels(const std::vector<Photo::Data>& photos) const
{
    const std::size_t s = photos.size();

    if (s < 2)
    {
        m_locationLabel->setText(tr("Photo location:"));
        m_sizeLabel->setText(tr("Photo size:"));
        m_geometryLabel->setText(tr("Photo geometry:"));
    }
    else
    {
        m_locationLabel->setText(tr("Photos location:"));
        m_sizeLabel->setText(tr("Photos size:"));
        m_geometryLabel->setText("Photos geometry:");
    }
}


void PhotoPropertiesWidget::refreshValues(const std::vector<Photo::Data>& photos) const
{
    const std::size_t s = photos.size();

    // calcualte photos size
    std::size_t size = 0;
    for(std::size_t i = 0; i < s; i++)
    {
        const QFileInfo info(photos[i].path);
        size += info.size();
    }

    const QString size_human = sizeHuman(size);

    // handle various cases
    if (s == 0)
    {
        m_locationValue->setText("---");
        m_sizeValue->setText("---");
        m_geometryValue->setText("---");
    }
    else if (s == 1)
    {
        const Photo::Data& photo = photos.front();
        const QString& filePath = photo.path;
        const QString geometry = geometryToStr(photo);

        // update values
        m_locationValue->setText(cutPrj(filePath));
        m_sizeValue->setText(size_human);

        m_geometryValue->setText(geometry);
    }
    else
    {
        // 'merge' paths
        QString result = photos.front().path;

        for(std::size_t i = 1; i < s; i++)
        {
            const QString anotherPhotoPath = photos[i].path;
            result = FileSystem().commonPath(result, anotherPhotoPath);
        }

        const QString decorated_path = result + (result.right(1) == QDir::separator()? QString("") : QDir::separator()) + "...";

        // try to merge geometry
        const QSize geometry = photos.front().geometry;
        const bool equal = std::all_of(photos.cbegin(), photos.cend(), [&geometry](const Photo::Data& photo)
        {
            return photo.geometry == geometry;
        });

        const QString geometryStr = equal? geometryToStr(geometry): "---";

        // update values
        m_locationValue->setText(cutPrj(decorated_path));
        m_sizeValue->setText(size_human);
        m_geometryValue->setText(geometryStr);
    }
}


QString PhotoPropertiesWidget::sizeHuman(qint64 size) const
{
    int i = 0;
    for(; i < 4 && size > 20480; i++)
        size /= 1024;

    const int sh = static_cast<int>(size);

    QString units;
    switch (i)
    {
        case 0:  units = tr("%n byte(s)",  "", sh); break;
        case 1:  units = tr("%n kbyte(s)", "", sh); break;
        case 2:  units = tr("%n Mbyte(s)", "", sh); break;
        default: units = tr("%n Gbyte(s)", "", sh); break;
    }

    return units;
}
