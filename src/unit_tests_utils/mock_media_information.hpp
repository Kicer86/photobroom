
#include <gmock/gmock.h>

#include <core/imedia_information.hpp>

class MediaInformationMock: public IMediaInformation
{
public:
    MOCK_METHOD(FileInformation, getInformation, (const QString &), (const, override));
};
