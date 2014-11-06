
#ifndef PROJECT_PICKER_HPP
#define PROJECT_PICKER_HPP

#include <QWidget>

namespace Ui
{
    class ProjectPicker;
}

class ProjectPicker: public QWidget
{
        Q_OBJECT

    public:
        explicit ProjectPicker(QWidget *parent = 0);
        ProjectPicker(const ProjectPicker &) = delete;
        ~ProjectPicker();

        ProjectPicker& operator=(const ProjectPicker &) = delete;

    private slots:
        void on_openButton_clicked();
        void on_newButton_clicked();

private:
        Ui::ProjectPicker *ui;
};

#endif // PROJECT_PICKER_HPP
 
