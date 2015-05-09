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

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <iostream>
#include "mainwindow.h"
#include "masterpassphrasesetup.h"
#include "environment.h"
#include "logindialog.h"
#include "security.h"
#include "settingsparser.h"
#include "runguard.h"

/* Simple helper function to create the fort configuration
 * file if it does not exist.
 *
 * When the file is created, property firstrun is written to
 * the file and the property is set to true as this is the
 * first time Fort is being run on the system.
 */
static bool createInitialConfigurationFile()
{
    //Create configuration file if it does not exists
    //and set firstrun to true.
    SettingsParser parser;
    QFile file(parser.getSettingsPath());

    if(!file.exists())
        parser.setBoolean("firstrun",true);

    return true;
}

/* Program entry point */
int main(int argc, char *argv[])
{
     QApplication a(argc, argv);

    //Simple sha1 hash of "fortpasswordmanager"
    RunGuard guard("ececb360a80961cdd61ced9cd5d7418449ca09f7");

    if(!guard.tryToRun())
    {
        QMessageBox::information(NULL,"Fort Password Manager",
                                 "Another instance of Fort is already running.");
        return 0;
    }

    a.setStyleSheet("QListWidget:item{padding:5px;}");

    createInitialConfigurationFile();

    Security sec;
    bool firstRun = false;

    if(Environment::isFirstRun())
    {
        MasterPassphraseSetup setupDialog(&sec);
        if(setupDialog.exec() == QDialog::Accepted)
        {
            sec.setMasterPassphraseHash(setupDialog.getPasswordHash());
            firstRun = true;
            //Set firstrun to false in the configuration file.
            Environment::setFirstRunFalse();
        }
        else
            return 0;
    }

    //Some highly unlikely situation:
    //
    //If we just created the master password, no need to ask it again.
    //There should be no data to decrypt anyway, but make an additional check
    //if, for some reason there's data...and try to decrypt it with newly created
    //master passphrase. We can assume that if IV exists, there's data. If there's
    //no IV but some data still exists, it's broken anyway.
    if(!firstRun || Environment::hasIV())
    {
        LogInDialog loginDialog(&sec);

        if(loginDialog.exec() != QDialog::Accepted)
            return 0;
    }

    MainWindow w(&sec);
    w.show();

    return a.exec();
}
