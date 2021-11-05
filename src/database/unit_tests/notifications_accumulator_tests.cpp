
#include <gmock/gmock.h>
#include <QSignalSpy>

#include "notifications_accumulator.hpp"

using testing::Eq;


TEST(NotificationsAccumulatorTest, dropAllNotificationsWhenIgnoreIsCalled)
{
    Database::NotificationsAccumulator notifications;
    Photo::Id id1(1), id2(2), id3(3), id4(4);

    QSignalSpy addSpy(&notifications, &Database::NotificationsAccumulator::photosAddedSignal);
    QSignalSpy modSpy(&notifications, &Database::NotificationsAccumulator::photosModifiedSignal);
    QSignalSpy delSpy(&notifications, &Database::NotificationsAccumulator::photosRemovedSignal);
    notifications.photosAdded( {id1, id2} );
    notifications.photosModified( {id1, id2} );
    notifications.photosRemoved( {id3, id4} );

    notifications.ignoreChanges();
    notifications.fireChanges();

    EXPECT_THAT(addSpy.size(), Eq(0));
    EXPECT_THAT(modSpy.size(), Eq(0));
    EXPECT_THAT(delSpy.size(), Eq(0));
}


TEST(NotificationsAccumulatorTest, doNotFireTwice)
{
    Database::NotificationsAccumulator notifications;
    Photo::Id id1(1), id2(2), id3(3), id4(4);

    QSignalSpy addSpy(&notifications, &Database::NotificationsAccumulator::photosAddedSignal);
    QSignalSpy modSpy(&notifications, &Database::NotificationsAccumulator::photosModifiedSignal);
    QSignalSpy delSpy(&notifications, &Database::NotificationsAccumulator::photosRemovedSignal);
    notifications.photosAdded( {id1, id2} );
    notifications.photosModified( {id1, id2} );
    notifications.photosRemoved( {id3, id4} );

    notifications.fireChanges();
    notifications.fireChanges();

    EXPECT_THAT(addSpy.size(), Eq(1));
    EXPECT_THAT(modSpy.size(), Eq(1));
    EXPECT_THAT(delSpy.size(), Eq(1));
}


TEST(NotificationsAccumulatorTest, exclusiveAddition)
{
    Database::NotificationsAccumulator notifications;
    Photo::Id id1(1), id2(2);

    QSignalSpy addSpy(&notifications, &Database::NotificationsAccumulator::photosAddedSignal);
    QSignalSpy modSpy(&notifications, &Database::NotificationsAccumulator::photosModifiedSignal);
    QSignalSpy delSpy(&notifications, &Database::NotificationsAccumulator::photosRemovedSignal);
    notifications.photosAdded( {id1, id2} );

    notifications.fireChanges();

    EXPECT_THAT(addSpy.size(), Eq(1));
    EXPECT_THAT(modSpy.size(), Eq(0));
    EXPECT_THAT(delSpy.size(), Eq(0));
}


TEST(NotificationsAccumulatorTest, exclusiveModification)
{
    Database::NotificationsAccumulator notifications;
    Photo::Id id1(1), id2(2);

    QSignalSpy addSpy(&notifications, &Database::NotificationsAccumulator::photosAddedSignal);
    QSignalSpy modSpy(&notifications, &Database::NotificationsAccumulator::photosModifiedSignal);
    QSignalSpy delSpy(&notifications, &Database::NotificationsAccumulator::photosRemovedSignal);
    notifications.photosModified( {id1, id2} );

    notifications.fireChanges();

    EXPECT_THAT(addSpy.size(), Eq(0));
    EXPECT_THAT(modSpy.size(), Eq(1));
    EXPECT_THAT(delSpy.size(), Eq(0));
}


TEST(NotificationsAccumulatorTest, exclusiveDeletion)
{
    Database::NotificationsAccumulator notifications;
    Photo::Id id1(1), id2(2);

    QSignalSpy addSpy(&notifications, &Database::NotificationsAccumulator::photosAddedSignal);
    QSignalSpy modSpy(&notifications, &Database::NotificationsAccumulator::photosModifiedSignal);
    QSignalSpy delSpy(&notifications, &Database::NotificationsAccumulator::photosRemovedSignal);
    notifications.photosRemoved( {id1, id2} );

    notifications.fireChanges();

    EXPECT_THAT(addSpy.size(), Eq(0));
    EXPECT_THAT(modSpy.size(), Eq(0));
    EXPECT_THAT(delSpy.size(), Eq(1));
}


TEST(NotificationsAccumulatorTest, deletionWins)
{
    Database::NotificationsAccumulator notifications;
    Photo::Id id1(1), id2(2);

    QSignalSpy addSpy(&notifications, &Database::NotificationsAccumulator::photosAddedSignal);
    QSignalSpy modSpy(&notifications, &Database::NotificationsAccumulator::photosModifiedSignal);
    QSignalSpy delSpy(&notifications, &Database::NotificationsAccumulator::photosRemovedSignal);

    notifications.photosAdded( {id1, id2} );
    notifications.photosRemoved( {id1, id2} );
    notifications.photosModified( {id1, id2} );

    notifications.fireChanges();

    EXPECT_THAT(addSpy.size(), Eq(0));
    EXPECT_THAT(modSpy.size(), Eq(0));
    EXPECT_THAT(delSpy.size(), Eq(1));
}
