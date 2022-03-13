
#ifndef SELECTIONMANAGERCOMPONENT_HPP
#define SELECTIONMANAGERCOMPONENT_HPP


#include <QQmlEngine>
#include <QObject>
#include <QList>
#include <set>


class SelectionManagerComponent: public QObject
{
        Q_OBJECT

        QML_NAMED_ELEMENT(SelectionManager)

    public:
        explicit SelectionManagerComponent(QObject * = nullptr);

        Q_PROPERTY(QList<int> selected READ selected NOTIFY selectedChanged)

        Q_INVOKABLE void clearAndToggleIndexSelection(int);
        Q_INVOKABLE void toggleIndexSelection(int);
        Q_INVOKABLE void clearSelection();
        Q_INVOKABLE void selectTo(int);
        Q_INVOKABLE bool isIndexSelected(int) const;
        Q_INVOKABLE QList<int> selected() const;

    private:
        std::set<int> m_selected;
        int m_previouslySelected;

        void calculateChange(const std::set<int> &, const std::set<int> &);
        void toggle(int);
        void clear();

    signals:
        void selectionChanged(const QList<int>& unselected,
                              const QList<int>& selected) const;

        void selectedChanged(const QList<int> &) const;
};

#endif // SELECTIONMANAGERCOMPONENT_HPP
