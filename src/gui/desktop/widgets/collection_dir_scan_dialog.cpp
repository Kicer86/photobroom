/*
 * Dialog for collection scanning
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>


#include "collection_dir_scan_dialog.hpp"


CollectionDirScanDialog::CollectionDirScanDialog(QWidget* p):
    QDialog(p),
    m_info(nullptr),
    m_button(nullptr),
    m_close(false)
{
    m_info = new QLabel(tr("Collection scan in progress"), this);
    m_button = new QPushButton(tr("Cancel"), this);

    connect(m_button, &QPushButton::clicked, this, &CollectionDirScanDialog::buttonPressed);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(m_info);
    l->addWidget(m_button);
}


CollectionDirScanDialog::~CollectionDirScanDialog()
{

}


void CollectionDirScanDialog::buttonPressed()
{
    if (m_close)
        accept();
    else
    {
        m_info->setText(tr("Collection scan canceled"));
        m_button->setText(tr("Close"));
        m_close = true;
    }
}


void CollectionDirScanDialog::scan()
{

}
