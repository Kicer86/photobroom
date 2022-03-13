
#include <gmock/gmock.h>
#include <QSignalSpy>

#include <desktop/quick_items/selection_manager_component.hpp>

using testing::ElementsAre;
using testing::IsEmpty;


TEST(SelectionManagerComponentTest, simpleSelections)
{
    SelectionManagerComponent selectionManager;

    QSignalSpy sp(&selectionManager, &SelectionManagerComponent::selectionChanged);

    selectionManager.toggleIndexSelection(3);
    EXPECT_THAT(selectionManager.selected(), ElementsAre(3));

    selectionManager.toggleIndexSelection(5);
    EXPECT_THAT(selectionManager.selected(), ElementsAre(3, 5));

    selectionManager.toggleIndexSelection(3);
    EXPECT_THAT(selectionManager.selected(), ElementsAre(5));

    ASSERT_EQ(sp.count(), 3);
    EXPECT_THAT(sp.at(0).at(0).value<QList<int>>(), IsEmpty());          // nothing was unselected
    EXPECT_THAT(sp.at(0).at(1).value<QList<int>>(), ElementsAre(3));     // item #3 was selected

    EXPECT_THAT(sp.at(1).at(0).value<QList<int>>(), IsEmpty());          // nothing was unselected
    EXPECT_THAT(sp.at(1).at(1).value<QList<int>>(), ElementsAre(5));     // item #5 was selected

    EXPECT_THAT(sp.at(2).at(0).value<QList<int>>(), ElementsAre(3));     // item #3 was unselected
    EXPECT_THAT(sp.at(2).at(1).value<QList<int>>(), IsEmpty());          // nothing was selected
}


TEST(SelectionManagerComponentTest, range)
{
    SelectionManagerComponent selectionManager;

    selectionManager.toggleIndexSelection(3);

    QSignalSpy sp(&selectionManager, &SelectionManagerComponent::selectionChanged);
    selectionManager.selectTo(5);
    EXPECT_THAT(selectionManager.selected(), ElementsAre(3, 4, 5));

    selectionManager.selectTo(0);
    EXPECT_THAT(selectionManager.selected(), ElementsAre(0, 1, 2, 3));

    ASSERT_EQ(sp.count(), 2);
    EXPECT_THAT(sp.at(0).at(0).value<QList<int>>(), IsEmpty());             // nothing was unselected
    EXPECT_THAT(sp.at(0).at(1).value<QList<int>>(), ElementsAre(4, 5));     // items #4 and #5 were selected

    EXPECT_THAT(sp.at(1).at(0).value<QList<int>>(), ElementsAre(4, 5));     // item #4 and #5 were unselected
    EXPECT_THAT(sp.at(1).at(1).value<QList<int>>(), ElementsAre(0, 1, 2));  // items #0, #1 and #2 were selected
}
