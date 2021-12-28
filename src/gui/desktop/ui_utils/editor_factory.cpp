/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "editor_factory.hpp"

#include <cassert>

#include <QCompleter>
#include <QDateEdit>
#include <QHeaderView>
#include <QLineEdit>
#include <QTableWidget>
#include <QTimeEdit>
#include <QTimer>
#include <QDoubleSpinBox>

#include <kratingwidget.h>
#include <kcolorcombo.h>

#include <core/base_tags.hpp>
#include <core/down_cast.hpp>
#include <core/imodel_compositor_data_source.hpp>

#include "utils/model_index_utils.hpp"
#include "utils/svg_utils.hpp"
#include "widgets/tag_editor/helpers/tags_model.hpp"
#include "icompleter_factory.hpp"


namespace
{
    struct TimeEditor: QTimeEdit
    {
        explicit TimeEditor(QWidget* parent_widget = nullptr): QTimeEdit(parent_widget)
        {
            setDisplayFormat("hh:mm:ss");
        }
    };

    template<typename T>
    T* make_editor(ICompleterFactory* completerFactory, const Tag::Types& tagType, QWidget* parent)
    {
        T* editor = new T(parent);
        QCompleter* completer = completerFactory->createCompleter(tagType);
        completer->setParent(editor);
        editor->setCompleter(completer);

        return editor;
    }
}


EditorFactory::EditorFactory()
    : m_star()
    , m_completerFactory(nullptr)
{
    QImage star = SVGUtils::load(":/gui/star.svg", {32, 32});
    m_star = QPixmap::fromImage(star);
}


EditorFactory::~EditorFactory()
{

}


void EditorFactory::set(ICompleterFactory* completerFactory)
{
    m_completerFactory = completerFactory;
}


QWidget* EditorFactory::createEditor(const QModelIndex& index, QWidget* parent)
{
    const QVariant tagTypeRaw = index.data(TagsModel::TagTypeRole);
    const Tag::Types tagType = tagTypeRaw.value<Tag::Types>();

    return createEditor(tagType, parent);
}


QWidget* EditorFactory::createEditor(const Tag::Types& tagType, QWidget* parent)
{
    QWidget* result = nullptr;

    switch(tagType)
    {
        case Tag::Types::Event:
        case Tag::Types::Place:
            result = make_editor<QLineEdit>(m_completerFactory, tagType, parent);
            break;

        case Tag::Types::Date:
            result = new QDateEdit(parent);
            break;

        case Tag::Types::Time:
            result = new TimeEditor(parent);
            break;

        case Tag::Types::Rating:
        {
            KRatingWidget* ratingWidget = new KRatingWidget(parent);
            ratingWidget->setCustomPixmap(m_star);

            result = ratingWidget;
            break;
        }

        case Tag::Types::Category:
        {
            KColorCombo* combo = new KColorCombo(parent);
            IModelCompositorDataSource* model = m_completerFactory->accessModel(Tag::Types::Category);
            const QStringList& colorsList = model->data();

            QList<QColor> colors;
            for(const QString& colorStr: colorsList)
            {
                const QRgba64 rgba64 = QRgba64::fromRgba64(colorStr.toULongLong());
                const QColor color(rgba64);
                colors.push_back(color);
            }

            combo->setColors(colors);

            result = combo;
            break;
        }

        case Tag::Types::Invalid:
        default:
            assert(!"Unexpected call");
            break;
    }

    return result;
}


QByteArray EditorFactory::valuePropertyName(const Tag::Types& tagType) const
{
    QByteArray result;

    switch(tagType)
    {
        case Tag::Types::Event:
        case Tag::Types::Place:
            result = "text";
            break;

        case Tag::Types::Date:
            result = "date";
            break;

        case Tag::Types::Time:
            result = "time";
            break;

        case Tag::Types::Rating:
            result = "rating";
            break;

        case Tag::Types::Category:
            result = "color";
            break;

        case Tag::Types::Invalid:
        default:
            assert(!"Unexpected call");
            break;
    }

    return result;
}
