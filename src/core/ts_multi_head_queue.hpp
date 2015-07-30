/*
 * TS_Queue with support for independend subqueues
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef TS_MULTIHEADQUEUE_HPP
#define TS_MULTIHEADQUEUE_HPP

template<typename T>
class TS_MultiHeadQueue
{
    public:
        TS_MultiHeadQueue()
        {
        }

        TS_MultiHeadQueue(const TS_MultiHeadQueue &) = delete;

        ~TS_MultiHeadQueue()
        {
        }

        TS_MultiHeadQueue& operator=(const TS_MultiHeadQueue &) = delete;
        
    private:
};

#endif // TS_MULTIHEADQUEUE_H
