
#ifndef PHOTODATAQML_HPP
#define PHOTODATAQML_HPP

#include <QObject>

#include <database/photo_data.hpp>

/**
 * wrapper for Photo::Data which provides Qml compatible access to Photo::Data members
 */
class PhotoDataQml: public QObject
{
        Q_OBJECT

    public:
        Q_PROPERTY(QVariantMap flags READ getFlags)
        Q_PROPERTY(Photo::Data data READ getPhotoData WRITE setPhotoData NOTIFY photoDataChanged)

        PhotoDataQml(QObject* parent = nullptr);
        PhotoDataQml(const Photo::Data &);

        /**
         * @brief access to flags as a QML compatible map
         * @returns map of flags with their values
         *
         * Method (combined with 'flags' property) returns map of flags.
         * Key is a QString equal to numerical value Photo::FlagsE enum
         * Value is a QVariant holding int equal to value of flag
         */
        QVariantMap getFlags() const;

        const Photo::Data& getPhotoData() const;
        void setPhotoData(const Photo::Data &);

    signals:
        void photoDataChanged(const Photo::Data &) const;

    private:
        Photo::Data m_photo;
};

#endif
