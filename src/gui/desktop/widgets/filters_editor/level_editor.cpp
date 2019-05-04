/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "level_editor.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QPainter>
#include <QDebug>


namespace
{
    struct WidgetsOrganizer
    {
        WidgetsOrganizer(std::vector<QLabel *>* widgets): m_widgets(widgets) {}

        void organize()
        {
            int pos = 0;

            for (QWidget* widget: *m_widgets)
            {
                widget->move(pos, 0);

                pos += widget->width();
            }
        }

        void addAt(const QPoint& /*point*/, QLabel* /*label*/)
        {

        }

        std::vector<QLabel *>* m_widgets;
    };


    struct ChoosenLevelsWidget: QFrame
    {
        ChoosenLevelsWidget();
        ChoosenLevelsWidget(const ChoosenLevelsWidget &) = delete;
        virtual ~ChoosenLevelsWidget();

        ChoosenLevelsWidget& operator=(const ChoosenLevelsWidget &) = delete;

        virtual void dragEnterEvent(QDragEnterEvent *) override;
        virtual void dragMoveEvent(QDragMoveEvent *) override;
        virtual void dropEvent(QDropEvent *) override;
        virtual void paintEvent(QPaintEvent *) override;

        QPoint calcDropPosition(const QPoint &);

        std::vector<QLabel *> m_levels;
        WidgetsOrganizer m_organizer;

        //drag data:
        QLabel* m_dd_label;
        QPoint  m_dd_offset;
        QPoint  m_dd_dropPosition;
    };

    template<unsigned int> struct PtrSize {};
    template<> struct PtrSize<4> { typedef quint32 ptr_size; };
    template<> struct PtrSize<8> { typedef quint64 ptr_size; };
    typedef PtrSize<sizeof(void *)>::ptr_size ptr_size;

    ChoosenLevelsWidget::ChoosenLevelsWidget(): QFrame(), m_levels(), m_organizer(&m_levels), m_dd_label(nullptr), m_dd_offset(), m_dd_dropPosition()
    {
        setAcceptDrops(true);
    }


    ChoosenLevelsWidget::~ChoosenLevelsWidget()
    {

    }


    void ChoosenLevelsWidget::dragEnterEvent(QDragEnterEvent *_event)
    {
        if (_event->mimeData()->hasFormat("application/x-dnditemdata"))
        {
            _event->acceptProposedAction();

            //collect drag data
            QByteArray itemData = _event->mimeData()->data("application/x-dnditemdata");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);

            ptr_size childAddr;
            QPoint offset;
            dataStream >> childAddr >> offset;

            m_dd_label = reinterpret_cast<QLabel *>(childAddr);
            m_dd_offset = offset;
        }
        else
            _event->ignore();
    }


    void ChoosenLevelsWidget::dragMoveEvent(QDragMoveEvent *_event)
    {
        const QPoint mouse_pos = _event->pos() - m_dd_offset;
        m_dd_dropPosition = calcDropPosition(mouse_pos);

        qDebug() << m_dd_dropPosition;

        update();
    }


    void ChoosenLevelsWidget::dropEvent(QDropEvent *_event)
    {
        const QPoint mouse_pos = _event->pos() - m_dd_offset;
        const QPoint dropPosition = calcDropPosition(mouse_pos);

        m_dd_label->setParent(this);
        m_dd_label->move(dropPosition);
        m_dd_label->show();

        m_levels.push_back(m_dd_label);

        _event->acceptProposedAction();

        m_dd_label = nullptr;
        m_dd_offset = QPoint();
    }


    void ChoosenLevelsWidget::paintEvent(QPaintEvent* _event)
    {
        QFrame::paintEvent(_event);

        if (m_dd_label != nullptr)
        {
            QPainter painter(this);

            QPen pen(Qt::blue, 4);
            painter.setPen(pen);

            const QSize widget_size = this->size();
            painter.drawLine(m_dd_dropPosition.x(), 0,
                             m_dd_dropPosition.x(), widget_size.height() - 1);
        }
    }


    QPoint ChoosenLevelsWidget::calcDropPosition(const QPoint& point)
    {
        int result = 0;

        for(QWidget* child: m_levels)
        {
            const QSize child_size = child->size();
            const QPoint child_pos = child->pos();

            const int left = child_pos.x();
            const int right = child_pos.x() + child_size.width();
            const int m = (left + right) / 2;  //central point

            if (point.x() >= left && point.x() <= m)   //mouse over left part of child? Return left side of child as result
            {
                result = left;
                break;
            }
            else if (point.x() > m)                    //mouse on the right side? (Right side of current child or farer)
                result = right;                        //by default set to right side. This will be the final result when we are over last child from list.
                                                       //It there are more children, algorithm will continue.
        }

        return QPoint(result, 0);
    }

}


LevelEditor::LevelEditor(QWidget* _parent): QWidget(_parent), m_notUsedItemsLayout(nullptr), m_levels()
{
    QFrame* usedLevelsArea = new ChoosenLevelsWidget;
    usedLevelsArea->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    usedLevelsArea->setLineWidth(0);
    usedLevelsArea->setMidLineWidth(0);
    usedLevelsArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout* levelsDefinitionLayout = new QHBoxLayout;
    levelsDefinitionLayout->addWidget(new QLabel(tr("Grouping:")));
    levelsDefinitionLayout->addWidget(usedLevelsArea);

    m_notUsedItemsLayout = new QHBoxLayout;

    QBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(levelsDefinitionLayout);
    mainLayout->addLayout(m_notUsedItemsLayout);
}


LevelEditor::~LevelEditor()
{

}


void LevelEditor::setLevelNames(const std::vector<QString>& names)
{
    //TODO: perform diff and remove/add widgets in smart way

    for (const QString& name: names)
    {
        QLabel* label = new QLabel(name, this);
        label->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
        label->setLineWidth(0);
        label->setMidLineWidth(0);
        label->setAttribute(Qt::WA_ContentsPropagated);

        m_levels.push_back(label);
        m_notUsedItemsLayout->addWidget(label);
    }

    m_notUsedItemsLayout->addStretch();
}


void LevelEditor::mousePressEvent(QMouseEvent *_event)
{
    QLabel *child = dynamic_cast<QLabel*>(childAt(_event->pos()));
    if (child == nullptr)
        return;

    QPixmap pixmap(child->size());
    child->render(&pixmap);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << reinterpret_cast<ptr_size>(child) << (_event->pos() - child->pos());

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(_event->pos() - child->pos());

    const QString originalColors = child->styleSheet();
    child->setStyleSheet("QLabel { color : gray; }");

    drag->exec();
    child->setStyleSheet(originalColors);
}
