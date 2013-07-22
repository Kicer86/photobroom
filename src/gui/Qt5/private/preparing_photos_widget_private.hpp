
#ifndef PREPARING_PHOTOS_WIDGET_PRIVATE_HPP
#define PREPARING_PHOTOS_WIDGET_PRIVATE_HPP

#include <QWidget>

class QFileDialog;
class QPushButton;
class QLineEdit;
class QPushButton;

struct BrowseLine: public QWidget
{
    Q_OBJECT

    public:
        BrowseLine(QWidget *);
        ~BrowseLine();

    signals:
        void addPath(QString);

    private:
        QFileDialog *m_dialog;
        QPushButton *m_button;
        QLineEdit   *m_line;
        QPushButton *m_addButton;

    private slots:
        void browseButtonClicked();
        void addButtonClicked();
};


struct BrowseList: public QWidget
{
        Q_OBJECT

    public:
        BrowseList(QWidget *);
        ~BrowseList();

    signals:
        void addPath(QString);

    private slots:
        void addPathToAnalyze(QString);
};

#endif
