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

#include "environment.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "settingsparser.h"

/* Static method.
 *
 * Returns the filesystem path Fort is using to store
 * the items. Default data path is /home/<user/.fort/
 *
 * If the path does not exist, it will be created.
 */
QString Environment::ensurePath()
{
    SettingsParser settingsParser;

    QString path = settingsParser.getString("datapath");

    if(path.isEmpty()) {
        char *envpath = getenv("HOME");
        QString qpath(envpath);
        path = qpath + "/.fort/";
    }
    else
    {
        //Prevent multiple trailing slashes.
        if(!path.endsWith("/"))
            path = path + "/";
    }

    if(!QDir(path).exists())
        QDir(path).mkdir(path);

    return path;
}

/* Static method.
 *
 * Returns true if there is initialization vector preserved,
 * false if not.
 */
bool Environment::hasIV()
{
    QString path = ensurePath() + FORT_IV_FILE;
    QFile file(path);

    return file.exists();
}

/* Static method.
 *
 * Checks if Fort is running for the first time on the system.
 * Method simply checks if fortrc file exists and reads "firstrun"
 * property from the file. If the file does not exist at all
 * true is returned.
 */
bool Environment::isFirstRun()
{
    SettingsParser parser;
    return parser.getBoolean("firstrun");
}

/* Set firstrun property to false in the fortrc file.
 */
void Environment::setFirstRunFalse()
{
    SettingsParser parser;
    parser.setBoolean("firstrun", false);
}





