
#ifndef SELECTIONMANAGERCOMPONENT_HPP
#define SELECTIONMANAGERCOMPONENT_HPP


#include <QObject>
#include <QList>
#include <set>


class SelectionManagerComponent: public QObject
{
        Q_OBJECT

    public:
        SelectionManagerComponent(QObject * = nullptr);

        Q_INVOKABLE void toggleIndexSelection(int);
        Q_INVOKABLE void clearSelection();
        Q_INVOKABLE bool isIndexSelected(int) const;
        Q_INVOKABLE QList<int> selected() const;

    private:
        std::set<int> m_selected;

        void calculateChange(const std::set<int> &, const std::set<int> &);

    signals:
        void selectionChanged(const QList<int>& unselected,
                              const QList<int>& selected);
};

#endif // SELECTIONMANAGERCOMPONENT_HPP
