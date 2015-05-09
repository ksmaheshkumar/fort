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

#include "settingsparser.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDir>
#include <QMessageBox>
#include "environment.h"

/* Constructor.
 * Fort settings file always lives in /home/<user>/.fort/
 * directory.
 */
SettingsParser::SettingsParser()
{
    char *path = getenv("HOME");
    QString qpath(path);

    qpath = qpath + "/.fort/";

    if(!QDir(qpath).exists())
    {
        if(!QDir(qpath).mkdir(qpath)) {
            //If this fails something is badly wrong in the user system...
             QMessageBox::information(0,"Fort Password Manager",
                                      "Can't create directory " + qpath + ". Fatal.");
        }
    }

    qpath = qpath + "/" + FORT_CONFIG_FILE;

    _settingsPath = qpath;
}

/* Methods reads boolean value from the Fort's configuration file
 * for the the wanted property.
 *
 * Returns the boolean value. If configuration value does not
 * exist false is always returned.
 */
bool SettingsParser::getBoolean(QString property)
{
    QFile file(_settingsPath);
    bool foundValue = false;

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);

        while(!inStream.atEnd())
        {
            QString line = inStream.readLine();
            QStringList tokens = line.split("=");

            if(tokens.length() > 0)
            {
                if(tokens[0].compare(property) == 0)
                {
                    QString value = tokens[1];

                    if(value.compare("true") == 0)
                        foundValue = true;

                    break;
                }
            }
        }

        file.close();
    }

    return foundValue;
}

/* Write new configuration boolean property to the Fort's
 * configuration file.
 *
 * Returns true on success, false on failure.
 */
bool SettingsParser::setBoolean(QString property, bool value)
{
    bool retval = false;
    QList<QString> *lines = readAllConfigurationLines();

    QString valueStr;

    if(value)
        valueStr = "true";
    else
        valueStr = "false";

    QString setting = property + "=" + valueStr;
    QFile file(_settingsPath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream outStream(&file);
        bool found = false;

        for(int i = 0; i < lines->count(); i++)
        {
            QString line = lines->at(i);
            QStringList tokens = line.split("=");

            if(tokens.length() > 0)
            {
                if(tokens[0].compare(setting.split("=")[0]) != 0)
                    outStream << line + "\n";
                else if(tokens[0].compare(setting.split("=")[0]) == 0)
                {
                    outStream << setting + "\n";
                    found = true;
                }
            }
        }

        if(!found)
            outStream << setting + "\n";

        file.close();

        retval = true;
    }

    delete lines;

    return retval;
}

/* Write new configuration string property to the Fort's
 * configuration file.
 *
 * Returns true on success, false on failure.
 */
bool SettingsParser::setString(QString property, QString value)
{
    bool retval = false;
    QList<QString> *lines = readAllConfigurationLines();

    QString setting = property + "=" + value;
    QFile file(_settingsPath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream outStream(&file);
        bool found = false;

        //Loop through all the existing lines and
        //look for a matching property. If found, replace it with the new
        //one. Otherwise write original line back to the file.
        for(int i = 0; i < lines->count(); i++)
        {
            QString line = lines->at(i);
            QStringList tokens = line.split("=");

            if(tokens.length() > 0)
            {
                if(tokens[0].compare(setting.split("=")[0]) != 0)
                    outStream << line + "\n";
                else if(tokens[0].compare(setting.split("=")[0]) == 0)
                {
                    outStream << setting + "\n";
                    found = true;
                }
            }
        }

        //Property did not exist, add it.
        if(!found)
            outStream << setting + "\n";

        file.close();

        retval = true;
    }

    delete lines;

    return retval;
}

/* Methods reads QString value from the Fort's configuration file
 * for the the wanted property.
 *
 * Returns the string value. If configuration value does not
 * exist an empty QString is returned.
 */
QString SettingsParser::getString(QString property)
{
    QFile file(_settingsPath);
    QString foundValue = "";

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);

        while(!inStream.atEnd())
        {
            QString line = inStream.readLine();
            QStringList tokens = line.split("=");

            if(tokens.length() > 0)
            {
                if(tokens[0].compare(property) == 0)
                {
                    foundValue = tokens[1];
                    break;
                }
            }
        }

        file.close();
    }

    return foundValue;
}

/* Get the fullpath of the configuration file.
 */
QString SettingsParser::getSettingsPath()
{
    return _settingsPath;
}

/* Get user defined idle value when Fort will be automatically
 * locked.
 *
 * Returns the value from the configuration file, or default value 3
 * minutes.
 */
int SettingsParser::getIntIdleTime(QString property)
{
    QFile file(_settingsPath);
    int foundValue = 3; //default value

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);

        while(!inStream.atEnd())
        {
            QString line = inStream.readLine();
            QStringList tokens = line.split("=");

            if(tokens.length() > 0)
            {
                if(tokens[0].compare(property) == 0)
                {
                    foundValue = tokens[1].toInt();
                    break;
                }
            }
        }

        file.close();
    }

    return foundValue;
}

/* Set idle time value in minutes.
 * Returns true on success, false on failure.
 */
bool SettingsParser::setIntIdleTime(QString property, int value)
{
    bool retval = false;
    QList<QString> *lines = readAllConfigurationLines();

    QString setting = property + "=" + QString::number(value);
    QFile file(_settingsPath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream outStream(&file);
        bool found = false;

        //Loop through all the existing lines and
        //look for a matching property. If found, replace it with the new
        //one. Otherwise write original line back to the file.
        for(int i = 0; i < lines->count(); i++)
        {
            QString line = lines->at(i);
            QStringList tokens = line.split("=");

            if(tokens.length() > 0)
            {
                if(tokens[0].compare(setting.split("=")[0]) != 0)
                    outStream << line + "\n";
                else if(tokens[0].compare(setting.split("=")[0]) == 0)
                {
                    outStream << setting + "\n";
                    found = true;
                }
            }
        }

        //Property did not exist, add it.
        if(!found)
            outStream << setting + "\n";

        file.close();

        retval = true;
    }

    delete lines;

    return retval;
}

/* Read all configuration file lines into a QList.
 * Return value must be deleted after use.
 */
QList<QString> *SettingsParser::readAllConfigurationLines()
{
    QFile file(_settingsPath);
    QList<QString> *lines = new QList<QString>();

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream inStream(&file);

        while(!inStream.atEnd())
        {
            QString line = inStream.readLine();
            lines->append(line);
        }

        file.close();
    }

    return lines;
}
