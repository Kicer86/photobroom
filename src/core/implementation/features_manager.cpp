/*
 * Photo Broom - photos management tool.
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

#include "features_manager.hpp"

#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>


FeaturesManager::FeaturesManager(ILoggerFactory* loggerFactory)
{
    m_logger = loggerFactory->get("FeaturesManager");
}


FeaturesManager::~FeaturesManager()
{

}


void FeaturesManager::detect()
{
    std::set<QString> features;

    for(IFeatureDetector* detector: m_detectors)
    {
        QStringList detector_features = detector->detect();
        features.insert(detector_features.begin(), detector_features.end());
    }

    for(const QString& feature: features)
    {
        const std::string msg = "feature " + feature.toStdString() + " enabled";
        m_logger->debug(msg);
    }

    m_features.swap(features);
}


void FeaturesManager::add(IFeatureDetector* detector)
{
    m_detectors.push_back(detector);
}


bool FeaturesManager::has(const QString& feature) const
{
    auto it = m_features.find(feature);

    return it != m_features.end();
}
