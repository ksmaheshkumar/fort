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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include "environment.h"

/* Constructor. Read settings from the configuration file
 * and set widgets to match.
 */
PreferencesDialog::PreferencesDialog(SettingsParser *settingsParser,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    _settingsParser = settingsParser;
    _settingsApplied = false;
    ui->checkBoxMinimizeOnClose->setChecked(_settingsParser->getBoolean("minimizeonclose"));

    //ensurePath gives us the old path, either the default path
    //or a value from the configuration file.
    ui->lineEditDataLocation->setText(Environment::ensurePath());
    ui->spinBoxIdleInternal->setValue(_settingsParser->getIntIdleTime("idleinterval"));
}

/* Deconstructor. Delete ui.
 */
PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

/* Select new datapath.
 */
void PreferencesDialog::on_toolButtonSelectDataLocation_clicked()
{
    QFileDialog dialog;

    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    if(dialog.exec() == QFileDialog::Accepted)
    {
        QString directory = dialog.selectedFiles()[0];
        ui->lineEditDataLocation->setText(directory);
        _settingsApplied = false;
    }
}

/* Some of the dialog buttons is being clicked.
 * Look for the button and do an action.
 */
void PreferencesDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch(ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Ok:
        if(!_settingsApplied)
            saveSettings();
        close();
        setResult(QDialog::Accepted);
        break;
    case QDialogButtonBox::Cancel:
        close();
        setResult(QDialog::Rejected);
        break;
    case QDialogButtonBox::Apply:
        if(!_settingsApplied)
            saveSettings();
        break;
    default:
        break;
    }
}

/* Save settings from the dialog widgets to the configuration file.
 * Data is also moved from the old datapath to the new one.
 * If, for some reason, user selects the same path as the old one is
 * file moving will fail silently. (rename() function)
 */
void PreferencesDialog::saveSettings()
{
    bool minimizeOnClose = ui->checkBoxMinimizeOnClose->isChecked();
    QString dataPath = ui->lineEditDataLocation->text();
    int idleInterval = ui->spinBoxIdleInternal->value();

    //Set the datapath
    //ensurePath gives us the old path, either the default path
    //or a value from the configuration file.
    QString oldpath = Environment::ensurePath();

    if(!_settingsParser->setString("datapath", dataPath))
    {
         QMessageBox::information(this,"Fort Password Manager",
                                  "Error writing configuration property.");
    }
    else
    {
        //move all files from old dir to the new path (except fortrc)
        //loop files in the old datapath and use qfile::rename to move them
        QDir dir(oldpath);
        QStringList filters;

        filters << "*.plain";
        filters << "*.iv";
        filters << "*.pph";

        QFileInfoList entries = dir.entryInfoList(filters,
                                                  QDir::Files | QDir::NoDotAndDotDot);

        foreach(QFileInfo entryInfo, entries)
        {
            QString filePath = entryInfo.absoluteFilePath();
            QFile file(filePath);

            file.rename(dataPath + "/" + entryInfo.fileName());
            file.close();
        }
    }

    //Set minimizeOnClose
    _settingsParser->setBoolean("minimizeonclose", minimizeOnClose);

    //Set idle interval
    _settingsParser->setIntIdleTime("idleinterval", idleInterval);

    _settingsApplied = true;
}

/* Mark settings as dirty.
 */
void PreferencesDialog::on_checkBoxMinimizeOnClose_toggled(bool /* not in use */)
{
    _settingsApplied = false;
}

/* Mark settings as dirty.
 */
void PreferencesDialog::on_spinBoxIdleInternal_valueChanged(int /* not in use */)
{
    _settingsApplied = false;
}
