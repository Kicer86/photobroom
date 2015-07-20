/*
 * Dialog for photos adding.
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


#ifndef PHOTOSADDDIALOG_HPP
#define PHOTOSADDDIALOG_HPP

#include <QMainWindow>

class IConfiguration;

namespace Ui
{
    class PhotosAddDialog;
}

class PhotosAddDialog: public QMainWindow
{
        Q_OBJECT

    public:
        explicit PhotosAddDialog(IConfiguration *, QWidget * = 0);
        PhotosAddDialog(const PhotosAddDialog &) = delete;

        ~PhotosAddDialog();

        PhotosAddDialog& operator=(const PhotosAddDialog &) = delete;

    private:
        Ui::PhotosAddDialog* ui;
        IConfiguration* m_config;

        void closeEvent(QCloseEvent *) override;

    signals:
        void closing();
};

#endif // PHOTOSADDDIALOG_HPP
