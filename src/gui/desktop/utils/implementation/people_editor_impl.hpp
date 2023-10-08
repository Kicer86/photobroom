
#ifndef PEOPLE_EDITOR_IMPL_HPP_INCLUDED
#define PEOPLE_EDITOR_IMPL_HPP_INCLUDED

#include <QRect>
#include <QString>

#include <database/person_data.hpp>


struct CalculatedData
{
    QRect position;
    Person::Fingerprint fingerprint;
    QString name;
    Photo::Id ph_id;
};

struct FaceInfo
{
    PersonInfo face;
    PersonName person;
    PersonFingerprint fingerprint;

    FaceInfo(const Photo::Id& id, const QRect& r)
    {
        face.ph_id = id;
        face.rect = r;
    }

    explicit FaceInfo(const PersonInfo& pi)
        : face(pi)
    {

    }
};

struct IFacesSaver
{
    virtual ~IFacesSaver() = default;
    virtual void store(FaceInfo &) = 0;
};

#endif
