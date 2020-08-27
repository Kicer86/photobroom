
#ifndef SELECTIONMANAGERCOMPONENT_HPP
#define SELECTIONMANAGERCOMPONENT_HPP


#include <QObject>
#include <unordered_set>


class SelectionManagerComponent: public QObject
{
        Q_OBJECT

    public:
        SelectionManagerComponent(QObject * = nullptr);

        Q_INVOKABLE void toggleIndexSelection(int);
        Q_INVOKABLE void clearSelection();
        Q_INVOKABLE bool isIndexSelected(int) const;

    private:
        std::unordered_set<int> m_selected;
};

#endif // SELECTIONMANAGERCOMPONENT_HPP
