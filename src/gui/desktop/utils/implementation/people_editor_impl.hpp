
#ifndef PEOPLE_EDITOR_IMPL_HPP_INCLUDED
#define PEOPLE_EDITOR_IMPL_HPP_INCLUDED

#include <functional>

#include <database/person_data.hpp>
#include <database/idatabase.hpp>

#include "../people_editor.hpp"

struct IRecognizePerson
{
    virtual ~IRecognizePerson() = default;
    virtual PersonName recognize(const PersonFullInfo &) = 0;
};


struct Face: public IFace
{
    Face(const Photo::Id& id, const PersonFullInfo& fi, std::shared_ptr<IRecognizePerson> recognizer, std::shared_ptr<OrientedImage> image, std::shared_ptr<Database::IClient> dbClient)
        : m_faceInfo(fi)
        , m_recognizer(recognizer)
        , m_image(image)
        , m_dbClient(dbClient)
        , m_id(id)
    {}

    const QRect& rect() const override
    {
        return m_faceInfo.position;
    }

    const QString& name() const override
    {
        return m_faceInfo.name.name();
    }

    const PersonName& person() const override
    {
        return m_faceInfo.name;
    }

    const OrientedImage& image() const override
    {
        return *m_image;
    }

    void setName(const QString& name) override
    {
        m_faceInfo.name = PersonName(name);
    }

    bool recognize() override
    {
        m_faceInfo.name = m_recognizer->recognize(m_faceInfo);

        return m_faceInfo.name.id().valid();
    }

    void store() override
    {
        m_dbClient->db().exec([id = m_id, pfi = m_faceInfo](Database::IBackend& backend)
        {
            backend.peopleInformationAccessor().store(id, pfi);
        }, "Face: store");
    }

    PersonFullInfo m_faceInfo;
    std::shared_ptr<IRecognizePerson> m_recognizer;
    std::shared_ptr<OrientedImage> m_image;
    std::shared_ptr<Database::IClient> m_dbClient;
    Photo::Id m_id;
};

#endif
