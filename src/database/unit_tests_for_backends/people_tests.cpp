
#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/photos_with_people.json.hpp"
#include "backend_utils.hpp"

#include "common.hpp"


// TODO: reenable

using testing::UnorderedElementsAre;

template<typename T>
struct PeopleTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(PeopleTest, BackendTypes);


TYPED_TEST(PeopleTest, personIntroduction)
{
    {
        const PersonName p1(Person::Id(), "P 1");
        const Person::Id p1_id = this->m_backend->peopleInformationAccessor().store(p1);

        const PersonName p1_r = this->m_backend->peopleInformationAccessor().person(p1_id);

        EXPECT_EQ(p1_r.name(), p1.name());
        EXPECT_EQ(p1_r.id(), p1_id);
    }

    {
        const PersonName p2(Person::Id(123), "P 2");
        const Person::Id p2_id = this->m_backend->peopleInformationAccessor().store(p2);

        EXPECT_FALSE(p2_id.valid());     // p2 was nonexistent person (id 123 doesn't exist in db), we should get an error

        const PersonName p2_r = this->m_backend->peopleInformationAccessor().person(Person::Id(123));

        EXPECT_FALSE(p2_r.id().valid()); // make sure there is no entry with given id
    }

    {
        const PersonName p2(Person::Id(), "P 2");
        const Person::Id p2_id = this->m_backend->peopleInformationAccessor().store(p2);

        const PersonName p2_dup(Person::Id(), "P 2");
        const Person::Id p2_dup_id = this->m_backend->peopleInformationAccessor().store(p2_dup);

        EXPECT_EQ(p2_dup_id, p2_id);     // we expect to get the same id in case of duplicate
    }
}


TYPED_TEST(PeopleTest, massivePersonIntroduction)
{
    std::set<PersonName> people;
    for(int i = 0; i < 8; i++)
    {
        const PersonName pn(Person::Id(), QString("P 3_%1").arg(i));
        const Person::Id pn_id = this->m_backend->peopleInformationAccessor().store(pn);
        const PersonName f_pn(pn_id, pn.name());     // construct PersonName with full info

        people.insert(f_pn);
    }

    const std::vector<PersonName> pns_r = this->m_backend->peopleInformationAccessor().listPeople();

    // we expect that all inserted people will appear in pns_r
    for (const PersonName& pn: pns_r)
    {
        const auto it = people.find(pn);
        ASSERT_TRUE(it != people.end());
        people.erase(it);
    }

    // `people` should be empty by now
    EXPECT_TRUE(people.empty());
}


/*
TYPED_TEST(PeopleTest, simpleAssignmentToPhoto)
{
    for_all([](Database::IDatabase* db)
    {
        // perform some manipulations with photos' tags
        db->performCustomAction([](Database::IBackend* op)
        {
            // store 2 photos
            Photo::DataDelta pd1, pd2;
            pd1.insert<Photo::Field::Path>("photo1.jpeg");
            pd2.insert<Photo::Field::Path>("photo2.jpeg");

            std::vector<Photo::Id> ids;
            std::vector<Photo::DataDelta> photos = { pd1, pd2 };
            this->m_backend->addPhotos(photos);

            ids.push_back(photos.front().getId());
            ids.push_back(photos.back().getId());

            const IPhotoInfo::Ptr photo1 = this->m_backend->getPhotoFor(ids[0]);
            const TagNameInfo pi1(Tag::Types::_People);
            const TagValue pv1({QString("person 1"), QString("person 2")});

            photo1->setTag(pi1, pv1);

            const IPhotoInfo::Ptr photo2 = this->m_backend->getPhotoFor(ids[1]);
            const TagNameInfo pi2(Tag::Types::_People);
            const TagValue pv2({QString("person 2"), QString("person 3")});

            photo2->setTag(pi2, pv2);

            // verify if people's name were stored
            const auto all_people = this->m_backend->peopleInformationAccessor().listPeople();
            ASSERT_EQ(all_people.size(), 3);
            EXPECT_EQ(all_people[0].name(), "person 1");
            EXPECT_EQ(all_people[1].name(), "person 2");
            EXPECT_EQ(all_people[2].name(), "person 3");

            // verify people assigned to photo1
            const auto photo1_people = this->m_backend->peopleInformationAccessor().listPeople(photo1->getID());
            ASSERT_EQ(photo1_people.size(), 2);
            EXPECT_EQ(photo1_people[0].p_id, all_people[0].id());
            EXPECT_EQ(photo1_people[0].ph_id, photo1->getID());
            EXPECT_FALSE(photo1_people[0].rect.isValid());

            EXPECT_EQ(photo1_people[1].p_id, all_people[1].id());
            EXPECT_EQ(photo1_people[1].ph_id, photo1->getID());
            EXPECT_FALSE(photo1_people[1].rect.isValid());

            // verify people assigned to photo2
            const auto photo2_people = this->m_backend->peopleInformationAccessor().listPeople(photo2->getID());
            ASSERT_EQ(photo2_people.size(), 2);
            EXPECT_EQ(photo2_people[0].p_id, all_people[1].id());
            EXPECT_EQ(photo2_people[0].ph_id, photo2->getID());
            EXPECT_FALSE(photo2_people[0].rect.isValid());

            EXPECT_EQ(photo2_people[1].p_id, all_people[2].id());
            EXPECT_EQ(photo2_people[1].ph_id, photo2->getID());
            EXPECT_FALSE(photo2_people[1].rect.isValid());

            // verfy photo1's tag
            const auto tags1 = photo1->getTags();
            ASSERT_EQ(tags1.size(), 1);                 // just people
            ASSERT_EQ(tags1.begin()->first.getTag(), Tag::Types::People);

            const auto peopleTagValues1 = tags1.begin()->second;
            const auto peopleList1 = peopleTagValues1.getList();
            ASSERT_EQ(peopleList1.size(), 2);           // 2 people
            EXPECT_EQ(peopleList1.front(), TagValue("person 1"));
            EXPECT_EQ(peopleList1.back(), TagValue("person 2"));

            // verfy photo2's tag
            const auto tags2 = photo2->getTags();
            ASSERT_EQ(tags2.size(), 1);                 // just people
            ASSERT_EQ(tags2.begin()->first.getTag(), Tag::Types::People);

            const auto peopleTagValues2 = tags2.begin()->second;
            const auto peopleList2 = peopleTagValues2.getList();
            ASSERT_EQ(peopleList2.size(), 2);           // 2 people
            EXPECT_EQ(peopleList2.front(), TagValue("person 2"));
            EXPECT_EQ(peopleList2.back(), TagValue("person 3"));
        });
    });
}
*/


/*
TYPED_TEST(PeopleTest, assignmentToPhotoTouchesPeople)
{
    for_all([](Database::IDatabase* db)
    {
        // perform some manipulations with photos' tags
        db->performCustomAction([](Database::IBackend* op)
        {
            // store 1 photo
            Photo::DataDelta pd1;
            pd1.insert<Photo::Field::Path>("photo1.jpeg");

            std::vector<Photo::Id> ids;
            std::vector<Photo::DataDelta> photos = { pd1 };
            this->m_backend->addPhotos(photos);

            ids.push_back(photos.front().getId());

            // add fully described person to photo
            const PersonName pn("person 123");
            const auto p_id = this->m_backendstore(pn);
            const QRect p_r(12, 34, 56, 78);
            const PersonInfo pi(p_id, ids[0], p_r);

            this->m_backendstore(pi);

            // verify if person was successfully stored
            {
                const auto people = this->m_backend->peopleInformationAccessor().listPeople(ids[0]);
                ASSERT_EQ(people.size(), 1);
                EXPECT_TRUE(people[0].id.valid());
                EXPECT_EQ(people[0].p_id, p_id);
                EXPECT_EQ(people[0].ph_id, ids[0]);
                EXPECT_EQ(people[0].rect, p_r);
            }

            // add more people to photo
            auto photo = this->m_backend->getPhotoFor(ids[0]);
            auto tags = photo->getTags();
            auto peopleTags = tags.find(TagNameInfo(Tag::Types::People));
            ASSERT_NE(peopleTags, tags.end());

            auto peopleList = peopleTags->second.getList();
            peopleList.push_back(TagValue("person 987"));
            photo->setTag(TagNameInfo(Tag::Types::People), peopleList);

            // verify if original person wasn't touched, and if we have a new companion
            {
                const auto people = this->m_backend->peopleInformationAccessor().listPeople(ids[0]);
                ASSERT_EQ(people.size(), 2);

                EXPECT_TRUE(people[0].id.valid());
                EXPECT_EQ(people[0].p_id, p_id);
                EXPECT_EQ(people[0].ph_id, ids[0]);
                EXPECT_EQ(people[0].rect, p_r);

                EXPECT_TRUE(people[1].id.valid());
                EXPECT_NE(people[1].p_id, p_id);          // not as first person's id
                EXPECT_EQ(people[1].ph_id, ids[0]);
                EXPECT_FALSE(people[1].rect.isValid());   // no rect for second guy
            }
        });
    });
}
*/


TYPED_TEST(PeopleTest, alteringPersonData)
{
    // store 1 photo
    Photo::DataDelta pd1;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());

    const Photo::Id& ph_id = ids.front();

    // store person without rect
    const Person::Id p_id = this->m_backend->peopleInformationAccessor().store(PersonName("person 25"));
    const PersonInfo::Id pi_id = this->m_backend->peopleInformationAccessor().store(PersonInfo(p_id, ph_id, {}, QRect()));

    // update rect info
    const QRect pr(34, 56, 78, 90);
    const PersonInfo pi_full(pi_id, p_id, ph_id, {}, pr);
    const PersonInfo::Id pi_id_full = this->m_backend->peopleInformationAccessor().store(pi_full);

    EXPECT_EQ(pi_id, pi_id_full);

    // expect one person in db with full data
    {
        const auto ppl = this->m_backend->peopleInformationAccessor().listPeople();
        ASSERT_EQ(ppl.size(), 1);
        EXPECT_EQ(ppl[0].id(), p_id);
        EXPECT_EQ(ppl[0].name(), "person 25");

        const auto ph_ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
        ASSERT_EQ(ph_ppl.size(), 1);
        EXPECT_EQ(ph_ppl[0].id, pi_id);
        EXPECT_EQ(ph_ppl[0].p_id, p_id);
        EXPECT_EQ(ph_ppl[0].rect, pr);
    }

    // remove person name
    const PersonInfo pi_no_person(pi_id, Person::Id(), ph_id, {}, pr);
    this->m_backend->peopleInformationAccessor().store(pi_no_person);

    // person should not be removed from people list, but should not be assigned to photo anymore.
    // Rect with face should stay
    {
        const auto ppl = this->m_backend->peopleInformationAccessor().listPeople();
        ASSERT_EQ(ppl.size(), 1);
        EXPECT_EQ(ppl[0].id(), p_id);
        EXPECT_EQ(ppl[0].name(), "person 25");

        const auto ph_ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
        ASSERT_EQ(ph_ppl.size(), 1);
        EXPECT_EQ(ph_ppl[0].id, pi_id);
        EXPECT_FALSE(ph_ppl[0].p_id.valid());
        EXPECT_EQ(ph_ppl[0].rect, pr);
    }

    // remove rect
    const PersonInfo pi_no_person_no_rect(pi_id, Person::Id(), ph_id, {}, QRect());
    this->m_backend->peopleInformationAccessor().store(pi_no_person_no_rect);

    // person should not be removed from people list, but should be totally removed from photo
    {
        const auto ppl = this->m_backend->peopleInformationAccessor().listPeople();
        ASSERT_EQ(ppl.size(), 1);
        EXPECT_EQ(ppl[0].id(), p_id);
        EXPECT_EQ(ppl[0].name(), "person 25");

        const auto ph_ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
        EXPECT_TRUE(ph_ppl.empty());
    }
}



TYPED_TEST(PeopleTest, rectIsMoreImportantThanName)
{
    // store 1 photo
    Photo::DataDelta pd1;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());

    const Photo::Id& ph_id = ids.front();

    // store person with rect
    const Person::Id p_id = this->m_backend->peopleInformationAccessor().store(PersonName("person 25"));
    const PersonInfo::Id pi_id = this->m_backend->peopleInformationAccessor().store(PersonInfo(p_id, ph_id, {}, QRect(12, 34, 56, 78)));

    // store the same person with another rect
    const Person::Id p_id2 = this->m_backend->peopleInformationAccessor().store(PersonName("person 25"));
    const PersonInfo::Id pi_id2 = this->m_backend->peopleInformationAccessor().store(PersonInfo(p_id, ph_id, {}, QRect(23, 34, 45, 56)));

    EXPECT_EQ(p_id, p_id2);   // same person
    EXPECT_NE(pi_id, pi_id2); // different entries on photo
}


TYPED_TEST(PeopleTest, inteligentRectUpdate)
{
    // store 1 photo
    Photo::DataDelta pd1;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());

    const Photo::Id& ph_id = ids.front();

    // store people without rects
    const Person::Id p_id = this->m_backend->peopleInformationAccessor().store(PersonName("person 15"));
    const PersonInfo::Id pi_id = this->m_backend->peopleInformationAccessor().store(PersonInfo(p_id, ph_id, {}, QRect()));
    const Person::Id p_id2 = this->m_backend->peopleInformationAccessor().store(PersonName("person 25"));
    const PersonInfo::Id pi_id2 = this->m_backend->peopleInformationAccessor().store(PersonInfo(p_id2, ph_id, {}, QRect()));
    const Person::Id p_id3 = this->m_backend->peopleInformationAccessor().store(PersonName("person 35"));
    const PersonInfo::Id pi_id3 = this->m_backend->peopleInformationAccessor().store(PersonInfo(p_id3, ph_id, {}, QRect()));

    EXPECT_NE(pi_id, pi_id2);
    EXPECT_NE(pi_id2, pi_id3);

    // update rect info omitting pi_id (backend should guess which person needs update)
    const QRect pr(34, 56, 78, 90);
    const PersonInfo pi_full(p_id2, ph_id, {}, pr);
    const PersonInfo::Id pi_id_full = this->m_backend->peopleInformationAccessor().store(pi_full);

    EXPECT_EQ(pi_id2, pi_id_full);

    // expect one person in db with full data + two with missing rects
    {
        const auto ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
        ASSERT_EQ(ppl.size(), 3);

        EXPECT_FALSE(ppl[0].rect.isValid());
        EXPECT_EQ(ppl[1].rect, pr);
        EXPECT_FALSE(ppl[2].rect.isValid());
    }
}


TYPED_TEST(PeopleTest, inteligentNameUpdate)
{
    // store 1 photo
    Photo::DataDelta pd1;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());

    const Photo::Id& ph_id = ids.front();

    // store faces without names
    const QRect r1(12, 34, 56, 78);
    const QRect r2(23, 45, 67, 89);
    const QRect r3(34, 56, 78, 90);
    const PersonInfo::Id pi_id = this->m_backend->peopleInformationAccessor().store(PersonInfo(Person::Id(), ph_id, {}, r1));
    const PersonInfo::Id pi_id2 = this->m_backend->peopleInformationAccessor().store(PersonInfo(Person::Id(), ph_id, {}, r2));
    const PersonInfo::Id pi_id3 = this->m_backend->peopleInformationAccessor().store(PersonInfo(Person::Id(), ph_id, {}, r3));

    EXPECT_NE(pi_id, pi_id2);
    EXPECT_NE(pi_id2, pi_id3);

    // update name ommiting pi_id (backend should guess which person needs update)
    const Person::Id pn_id = this->m_backend->peopleInformationAccessor().store(PersonName("per 12345"));
    const PersonInfo pi_full(pn_id, ph_id, {}, r2);
    const PersonInfo::Id pi_id_full = this->m_backend->peopleInformationAccessor().store(pi_full);

    EXPECT_EQ(pi_id2, pi_id_full);

    // expect one person in db with full data + two with missing names
    {
        const auto ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
        ASSERT_EQ(ppl.size(), 3);

        EXPECT_FALSE(ppl[0].p_id.valid());
        EXPECT_EQ(ppl[1].p_id, pn_id);
        EXPECT_FALSE(ppl[2].p_id.valid());
    }
}


TYPED_TEST(PeopleTest, photoTagsWhenNoName)
{
    // store 1 photo
    Photo::DataDelta pd1;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());

    const Photo::Id& ph_id = ids.front();

    // store faces without names
    const QRect r1(12, 34, 56, 78);
    const QRect r2(23, 45, 67, 89);
    const QRect r3(34, 56, 78, 90);
    this->m_backend->peopleInformationAccessor().store(PersonInfo(Person::Id(), ph_id, {}, r1));
    this->m_backend->peopleInformationAccessor().store(PersonInfo(Person::Id(), ph_id, {}, r2));
    this->m_backend->peopleInformationAccessor().store(PersonInfo(Person::Id(), ph_id, {}, r3));

    const auto photo = this->m_backend->getPhoto(ph_id);

    EXPECT_TRUE(photo.tags.empty());
}


TYPED_TEST(PeopleTest, inteligentPersonNameRemoval)
{
    // store 1 photo
    Photo::DataDelta pd1;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());

    const Photo::Id& ph_id = ids.front();
    const Person::Id p_id = this->m_backend->peopleInformationAccessor().store(PersonName("person 25"));

    // store person with full data
    const QRect pr(34, 56, 78, 90);
    const PersonInfo pi_full(p_id, ph_id, {}, pr);
    const PersonInfo::Id pi_id = this->m_backend->peopleInformationAccessor().store(pi_full);

    // expect one person in db with full data
    {
        const auto ppl = this->m_backend->peopleInformationAccessor().listPeople();
        ASSERT_EQ(ppl.size(), 1);
        EXPECT_EQ(ppl[0].id(), p_id);
        EXPECT_EQ(ppl[0].name(), "person 25");

        const auto ph_ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
        ASSERT_EQ(ph_ppl.size(), 1);
        EXPECT_EQ(ph_ppl[0].id, pi_id);
        EXPECT_EQ(ph_ppl[0].p_id, p_id);
        EXPECT_EQ(ph_ppl[0].rect, pr);
    }

    // remove person name (Omit PersonInfo::Id)
    const PersonInfo pi_no_person(Person::Id(), ph_id, {}, pr);
    this->m_backend->peopleInformationAccessor().store(pi_no_person);

    // person should not be removed from people list, but should not be assigned to photo anymore.
    // Rect with face should stay
    {
        const auto ppl = this->m_backend->peopleInformationAccessor().listPeople();
        ASSERT_EQ(ppl.size(), 1);
        EXPECT_EQ(ppl[0].id(), p_id);
        EXPECT_EQ(ppl[0].name(), "person 25");

        const auto ph_ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
        ASSERT_EQ(ph_ppl.size(), 1);
        EXPECT_EQ(ph_ppl[0].id, pi_id);
        EXPECT_FALSE(ph_ppl[0].p_id.valid());
        EXPECT_EQ(ph_ppl[0].rect, pr);
    }
}


/*
TYPED_TEST(PeopleTest, removePersonWhenItsRemovedFromTags)
{
    for_all([](Database::IDatabase* db)
    {
        db->performCustomAction([](Database::IBackend* op)
        {
            // store 1 photo
            Photo::DataDelta pd1;
            pd1.insert<Photo::Field::Path>("photo1.jpeg");

            std::vector<Photo::Id> ids;
            std::vector<Photo::DataDelta> photos = { pd1 };
            this->m_backend->addPhotos(photos);

            ids.push_back(photos.front().getId());

            const Photo::Id& ph_id = ids.front();
            auto photo = this->m_backend->getPhotoFor(ph_id);

            {
                // store people
                const TagValue people({ TagValue("Per1"),
                                        TagValue("Per2"),
                                        TagValue("Per3")});

                photo->setTag(TagNameInfo(Tag::Types::People), people);

                // verify people count
                const auto ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
                ASSERT_EQ(ppl.size(), 3);
            }

            {
                // remove person from tags
                const TagValue people({ TagValue("Per1"),
                                        TagValue("Per3")});

                photo->setTag(TagNameInfo(Tag::Types::People), people);

                // verify people count
                const auto ppl = this->m_backend->peopleInformationAccessor().listPeople(ph_id);
                ASSERT_EQ(ppl.size(), 2);
            }
        });
    });
}
*/

TYPED_TEST(PeopleTest, readPeopleViaDataDelta)
{
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(PeopleDB::db);

    const auto photos = Database::getPhotoDelta<Photo::Field::People>(*this->m_backend);

    std::vector<PersonFullInfo> peopleInfo;

    for(const auto& photo: photos)
        for(const auto& person: photo.template get<Photo::Field::People>())
            peopleInfo.push_back(person);

    std::set<QString> peopleNames;
    std::ranges::transform(peopleInfo, std::inserter(peopleNames, peopleNames.end()), [](const auto& personInfo)
    {
        return personInfo.name.name();
    });

    EXPECT_THAT(peopleNames, UnorderedElementsAre("person 1", "person 2", "person 3", "person 4", "person 5"));
}
