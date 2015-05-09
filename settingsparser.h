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

#ifndef SETTINGSPARSER_H
#define SETTINGSPARSER_H

#include <QString>
#include <QList>


class SettingsParser
{
public:
    SettingsParser();
    bool getBoolean(QString property);
    bool setBoolean(QString property, bool value);
    bool setString(QString property, QString value);
    QString getString(QString property);
    QString getSettingsPath();
    int getIntIdleTime(QString property);
    bool setIntIdleTime(QString property, int value);
private:
    QString _settingsPath;
    QList<QString> *readAllConfigurationLines();

};

#endif // SETTINGSPARSER_H
