
#ifndef EXIV2_EXIF_READER_HPP
#define EXIV2_EXIF_READER_HPP

#include <memory>
#include <string>

#include <exiv2/exiv2.hpp>

#include "aexif_reader.hpp"


template<typename, typename = void>
struct has_auto_ptr_type : std::false_type
{
    using ptr_type = int;
};

template<typename T>
struct has_auto_ptr_type<T, std::void_t<typename T::AutoPtr>> : std::true_type
{
    using ptr_type = typename T::AutoPtr;
};

template<typename, typename = void>
struct has_unique_ptr_type : std::false_type
{
    using ptr_type = int;
};

template<typename T>
struct has_unique_ptr_type<T, std::void_t<typename T::UniquePtr>> : std::true_type
{
    using ptr_type = typename T::UniquePtr;
};

template <typename T>
struct Exiv2Helper
{
    using Ptr = typename std::conditional<has_unique_ptr_type<T>::value, typename has_unique_ptr_type<T>::ptr_type, typename has_auto_ptr_type<T>::ptr_type>::type;
};


class Exiv2ExifReader final: public AExifReader
{
    public:
        Exiv2ExifReader();
        Exiv2ExifReader(const Exiv2ExifReader &) = delete;
        Exiv2ExifReader(Exiv2ExifReader &&) = delete;

        Exiv2ExifReader& operator=(const Exiv2ExifReader &) = delete;
        Exiv2ExifReader& operator=(Exiv2ExifReader &&) = delete;

    private:
        bool hasExif(const QString & path) override;
        virtual void collect(const QString &) override;
        virtual std::optional<std::string> read(TagType) const override;

        Exiv2Helper<Exiv2::Image>::Ptr m_exif_data;
        QString m_path;
};

#endif
