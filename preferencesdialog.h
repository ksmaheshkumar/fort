#ifndef PREFERENCESDIALOG_H
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

#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "settingsparser.h"

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(SettingsParser *settingsParser, QWidget *parent = 0);
    ~PreferencesDialog();

private slots:
    void on_toolButtonSelectDataLocation_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_checkBoxMinimizeOnClose_toggled(bool checked);

    void on_spinBoxIdleInternal_valueChanged(int arg1);

private:
    Ui::PreferencesDialog *ui;
    SettingsParser *_settingsParser;
    void saveSettings();
    bool _settingsApplied;
};

#endif // PREFERENCESDIALOG_H
