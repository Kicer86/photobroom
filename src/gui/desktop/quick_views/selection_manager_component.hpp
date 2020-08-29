
#ifndef SELECTIONMANAGERCOMPONENT_HPP
#define SELECTIONMANAGERCOMPONENT_HPP


#include <QObject>
#include <QList>
#include <QSet>


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
        // TODO: https://doc.qt.io/qt-5/qtqml-cppintegration-data.html#sequence-type-to-javascript-array
        // register qset to pass it to qml
        QSet<int> m_selected;
};

#endif // SELECTIONMANAGERCOMPONENT_HPP
