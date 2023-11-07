
#include "apeople_information_accessor.hpp"

namespace Database
{
    PersonInfo::Id APeopleInformationAccessor::store(const PersonInfo& fd)
    {
        assert(fd.ph_id.valid());
        assert(fd.rect.isValid() || fd.p_id.valid() || fd.id.valid());  // if rect is invalid and person is invalid then at least id must be valid (removal operation)

        PersonInfo::Id result = fd.id;

        if (fd.id.valid() && fd.rect.isValid() == false && fd.p_id.valid() == false)
            dropPersonInfo(fd.id);
        else
        {
            PersonInfo to_store = fd;

            if (fd.id.valid() == false)
            {
                // determine if it is a new person, or we want to update existing one
                const auto existing_people = listPeople(fd.ph_id);

                for(const auto& person: existing_people)
                {
                    if (fd.rect.isValid() && person.rect == fd.rect)    // same, valid rect
                    {
                        to_store.id = person.id;
                        break;
                    }
                    else if (person.p_id.valid()    &&
                             person.p_id == fd.p_id &&
                             person.rect.isValid() == false)            // same, valid person but no rect in db
                    {
                        to_store.id = person.id;
                        break;
                    }
                }
            }

            if (to_store.id.valid() && to_store.rect.isValid() == false && to_store.p_id.valid() == false)
                dropPersonInfo(fd.id);
            else
                result = storePerson(to_store);
        }

        return result;
    }
}
