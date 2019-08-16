
#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <any>
#include <optional>

#include <gmock/gmock.h>


// TODO: workaround for libstdc++/gtest bug
namespace std
{
    template<>
    struct is_copy_constructible<testing::internal::ReferenceOrValueWrapper<std::optional<std::any>>>: std::true_type {};
}


#endif
