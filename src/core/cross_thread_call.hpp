
/*
 * Utility for passing calls from one thread to another
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef CROSS_THREAD_CALL
#define CROSS_THREAD_CALL

// http://stackoverflow.com/questions/21646467/how-to-execute-a-functor-in-a-given-thread-in-qt-gcd-style

#include <functional>

#include <QMetaObject>
#include <QObject>
#include <QPointer>

#include "core_export.h"


// extends QMetaObject::invokeMethod by version with arguments
template<typename Obj, typename F, typename... Args>
void invokeMethod(Obj* object, const F& method, Args&&... args)
{
    static_assert(std::is_base_of<QObject, Obj>::value, "Obj must be QObject");
    QMetaObject::invokeMethod(object, [object, method, args...]()
    {
        (object->*method)(args...);
    });
}


template<typename F, typename... Args>
void call_from_this_thread(QPointer<QObject> object, const F& function, Args&&... args)
{
    if (object.data() != nullptr)
        QMetaObject::invokeMethod(object.data(), [function, args...]()
        {
            function(args...);
        });
}


template<typename... Args, typename F>
std::function<void(Args...)> make_cross_thread_function(QObject* object, const F& function)
{
    std::function<void(Args...)> result = [=](Args&&... args)
    {
        call_from_this_thread(QPointer<QObject>(object), function, std::forward<Args>(args)...);
    };

    return result;
}


template<typename ObjT, typename R, typename ...Args>
std::function<void(Args...)> slot(ObjT* obj, R(ObjT::*method)(Args...))
{
    static_assert(std::is_base_of<QObject, ObjT>::value, "ObjT must be QObject");

    QPointer<ObjT> objPtr(obj);

    return [objPtr, method](Args... args)
    {
        ObjT* object = objPtr.data();

        if (object)
            invokeMethod(object, method, args...);
    };
}


#endif
