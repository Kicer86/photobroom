
#ifndef TAG_DEFINITION_HPP 
#define TAG_DEFINITION_HPP 

#include <QWidget>

class QPushButton;
class QComboBox;

class TagDefinition: public QWidget
{
    public:
        explicit TagDefinition(QWidget* parent = 0, Qt::WindowFlags f = 0);
        virtual ~TagDefinition();
        
    private:
        QComboBox* m_comboBox;
        QPushButton* m_button;
};

#endif
