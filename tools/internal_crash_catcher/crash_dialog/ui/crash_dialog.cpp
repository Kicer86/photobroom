/*
 * Crash dialog
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


#include "crash_dialog.hpp"

#include <functional>

#include "idebugger.hpp"
#include "ui_crash_dialog.h"


CrashDialog::CrashDialog(IDebugger* debugger): QDialog(), m_debugger(debugger)
{
    ui = new Ui::CrashDialog;
    ui->setupUi(this);

    using namespace std::placeholders;

    auto bt = std::bind(&CrashDialog::backtrace, this, _1);
    m_debugger->requestBacktrace(bt);
}


CrashDialog::~CrashDialog()
{
    delete ui;
}


void CrashDialog::backtrace(const std::vector<QString>& bt)
{

}
