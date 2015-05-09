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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>

#define FORT_CONFIG_FILE "fortrc"
#define FORT_IV_FILE "fort.iv"
#define FORT_KEY_FILE "fort.pph"

class Environment
{
public:
    Environment(){}
    static QString ensurePath();
    static bool hasIV();
    static bool isFirstRun();
    static void setFirstRunFalse();
};

#endif // ENVIRONMENT_H
