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
    struct ChoosenLevelsWidget: QFrame
    {
        ChoosenLevelsWidget();
        virtual ~ChoosenLevelsWidget();

        virtual void dragEnterEvent(QDragEnterEvent *event) override;
        virtual void dragMoveEvent(QDragMoveEvent *) override;
        virtual void dropEvent(QDropEvent *) override;

        std::deque<QLabel *> m_levels;
    };

    template<unsigned int> struct PtrSize {};
    template<> struct PtrSize<4> { typedef quint32 ptr_size; };
    template<> struct PtrSize<8> { typedef quint64 ptr_size; };
    typedef PtrSize<sizeof(void *)>::ptr_size ptr_size;

    ChoosenLevelsWidget::ChoosenLevelsWidget(): QFrame()
    {
        setAcceptDrops(true);
    }


    ChoosenLevelsWidget::~ChoosenLevelsWidget()
    {

    }


    void ChoosenLevelsWidget::dragEnterEvent(QDragEnterEvent *_event)
    {
        if (_event->mimeData()->hasFormat("application/x-dnditemdata"))
            _event->acceptProposedAction();
        else
            _event->ignore();
    }


    void ChoosenLevelsWidget::dragMoveEvent(QDragMoveEvent *_event)
    {
        qDebug() << _event->pos();
    }


    void ChoosenLevelsWidget::dropEvent(QDropEvent *_event)
    {
        QByteArray itemData = _event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        ptr_size childAddr;
        QPoint offset;
        dataStream >> childAddr >> offset;

        const QPoint pos = _event->pos() - offset;

        QLabel* label = reinterpret_cast<QLabel *>(childAddr);
        label->setParent(this);
        label->move(pos);
        label->show();

        m_levels.push_back(label);

        _event->acceptProposedAction();
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


void LevelEditor::setLevelNames(const std::deque<QString>& names)
{
    //TODO: perform diff and remove/add widgets in smart way

    for (const QString& name: names)
    {
        QLabel* label = new QLabel(name, this);
        label->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
        label->setLineWidth(0);
        label->setMidLineWidth(0);

        m_levels.push_back(label);
        m_notUsedItemsLayout->addWidget(label);
    }
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
    dataStream << reinterpret_cast<ptr_size>(child) << QPoint(_event->pos() - child->pos());

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
