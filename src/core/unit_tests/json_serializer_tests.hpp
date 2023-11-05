
#pragma once

#include <deque>
#include <string>
#include <vector>
#include <QRect>


namespace
{
    struct ABC
    {
        int a = 5;
        float b = 45.f;
        double c = 123.;
        std::string d = "Hello";
        QRect e = QRect(1, 2, 3, 4);
        std::vector<QSize> f = {};

        bool operator==(const ABC& other) const
        {
            return std::tie(a, b, c, d, e, f) == std::tie(other.a, other.b, other.c, other.d, other.e, other.f);
        }
    };

    struct DEF
    {
        std::vector<ABC> abc_vec;
        std::deque<ABC> abc_deque;

        bool operator==(const DEF& other) const
        {
            return std::tie(abc_vec, abc_deque) == std::tie(other.abc_vec, other.abc_deque);
        }
    };
}
