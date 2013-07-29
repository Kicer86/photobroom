
#ifndef PREPARING_PHOTOS_WIDGET_PRIVATE_HPP
#define PREPARING_PHOTOS_WIDGET_PRIVATE_HPP

#include <QWidget>

class QFileDialog;
class QPushButton;
class QLineEdit;
class QPushButton;
class QtExtChooseFile;

struct BrowseLine: public QWidget
{
    Q_OBJECT

    public:
        BrowseLine(QWidget *);
        ~BrowseLine();

        BrowseLine(const BrowseLine &) = delete;
        void operator=(const BrowseLine &) = delete;

    signals:
        void addPath(QString);

    private:
        QFileDialog *m_dialog;
        QPushButton *m_button;
        QLineEdit   *m_line;
        QPushButton *m_addButton;
        QtExtChooseFile *m_chooser;

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
};

#endif
