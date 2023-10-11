
#ifndef PEOPLE_EDITOR_IMPL_HPP_INCLUDED
#define PEOPLE_EDITOR_IMPL_HPP_INCLUDED

#include <QRect>
#include <QString>

#include <database/person_data.hpp>
#include <database/idatabase.hpp>

#include "../people_editor.hpp"


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


struct Face: public IFace
{
    Face(const FaceInfo& fi, std::shared_ptr<OrientedImage> image, std::shared_ptr<IFacesSaver> saver)
        : m_faceInfo(fi)
        , m_image(image)
        , m_saver(saver)
    {}

    const QRect& rect() const override
    {
        return m_faceInfo.face.rect;
    }

    const QString& name() const override
    {
        return m_faceInfo.person.name();
    }

    const OrientedImage& image() const override
    {
        return *m_image;
    }

    void setName(const QString& name) override
    {
        m_faceInfo.person = PersonName(name);
    }

    void store() override
    {
        m_saver->store(m_faceInfo);
    }

    FaceInfo m_faceInfo;
    std::shared_ptr<OrientedImage> m_image;
    std::shared_ptr<IFacesSaver> m_saver;
};

#endif
