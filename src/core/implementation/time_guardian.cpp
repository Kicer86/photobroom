/*
 * Time measure utility
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

#include "time_guardian.hpp"

#include <chrono>
#include <iostream>

struct TimeGuardian::Data
{
    Data(): m_limit(0), m_limitMessage(), m_title(), m_start() {}

    unsigned int m_limit;
    std::string m_limitMessage;
    std::string m_title;

    std::chrono::steady_clock::time_point m_start;
};


TimeGuardian::TimeGuardian(): m_data(new Data)
{

}


TimeGuardian::~TimeGuardian()
{
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    const auto diff = end - m_data->m_start;
    const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    if (diff_ms >= m_data->m_limit)
        std::cerr << m_data->m_title << ": " << diff_ms << "ms. " << m_data->m_limitMessage << std::endl;
}


void TimeGuardian::setLimit(unsigned int ms, const std::string& message)
{
    m_data->m_limit = ms;
    m_data->m_limitMessage = message;
}


void TimeGuardian::setTitle(const std::string& title)
{
    m_data->m_title = title;
}


void TimeGuardian::begin()
{
    m_data->m_start = std::chrono::steady_clock::now();
}

