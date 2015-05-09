/*
 * This file is part of Fort.
 *
 * Fort is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fort is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Fort.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2015 Niko Rosvall <niko@ideabyte.net>
 *
 */

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "security.h"

namespace Ui {
class LogInDialog;
}

class LogInDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LogInDialog(Security *sec, QWidget *parent = 0);
    ~LogInDialog();
    
private slots:
    void on_checkBox_toggled(bool checked);
    void on_pushButtonCancel_clicked();
    void on_pushButtonOK_clicked();

    void on_linePassword_cursorPositionChanged(int arg1, int arg2);

    void on_pushButtonExit_clicked();

private:
    Ui::LogInDialog *ui;
    Security *_sec;
    QString _lineEditPassStyle;
};

#endif // LOGINDIALOG_H
