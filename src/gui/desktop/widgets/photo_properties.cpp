/*
 * Widget for photo properties
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

#include <QGridLayout>
#include <QLabel>


#include "utils/selection_extractor.hpp"


namespace
{
    QString common(const QString& s1, const QString& s2)
    {
        int p = 0;
        for(int i = 0; i < std::min(s1.length(), s2.length()); i++)
        {
            if (s1[i] == s2[i])
                p = i + 1;
            else
                break;
        }

        const QString result = s1.left(p);

        return result;
    }
}


PhotoProperties::PhotoProperties(QWidget* p):
    QWidget(p),
    m_selectionExtractor(nullptr),
    m_locationLabel(new QLabel(this)),
    m_locationValue(new QLabel(this))
{
    QGridLayout* l = new QGridLayout(this);

    l->addWidget(m_locationLabel, 0, 0);
    l->addWidget(m_locationValue, 0, 1);
}


PhotoProperties::~PhotoProperties()
{

}


void PhotoProperties::set(const SelectionExtractor* selection)
{
    m_selectionExtractor = selection;

    connect(m_selectionExtractor, &SelectionExtractor::selectionChanged, this, &PhotoProperties::refreshView);
}


void PhotoProperties::refreshView() const
{
    std::vector<IPhotoInfo::Ptr> photos = m_selectionExtractor->getSelection();

    refreshLabels(photos);
    refreshValues(photos);
}


void PhotoProperties::refreshLabels(const std::vector<IPhotoInfo::Ptr>& photos) const
{
    const std::size_t s = photos.size();

    if (s < 2)
        m_locationLabel->setText(tr("Photo location:"));
    else
        m_locationLabel->setText(tr("Photos location:"));
}


void PhotoProperties::refreshValues(const std::vector<IPhotoInfo::Ptr>& photos) const
{
    const std::size_t s = photos.size();

    if (s == 0)
        m_locationValue->setText("---");
    else if (s == 1)
    {
        const QString relativePath = pathToPrjRelative(photos.front()->getPath());
        m_locationValue->setText(relativePath);
    }
    else
    {
        QString result = photos.front()->getPath();

        for(std::size_t i = 1; i < photos.size(); i++)
            result = common(result, photos[i]->getPath());

        const QString relative = pathToPrjRelative(result);
        const QString decorated = relative + "...";
        m_locationValue->setText(decorated);
    }
}


QString PhotoProperties::pathToPrjRelative(const QString& path) const
{
    assert(path.left(5) == "prj:/");

    const QString result = path.mid(5);
    return result;
}
