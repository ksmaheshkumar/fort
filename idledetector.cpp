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

#include "idledetector.h"

IdleDetector::IdleDetector()
{
    _display = XOpenDisplay(NULL);
    _info = XScreenSaverAllocInfo();

    //Read wantedIdleValueSetting here
    _wantedIdleValueSetting = 180000;
}

/* Deconstructor.
 *
 * Close X Display if exists and free
 * XScreenSaveInfo structure.
 */
IdleDetector::~IdleDetector()
{
    if(_display)
        XCloseDisplay(_display);

    if(_info)
        XFree(_info);
}

/* Method return the system (X11) idle time in
 * milliseconds.
 *
 * On failure -1 is returned.
 */
long IdleDetector::getIdleTime()
{
    if(!_display)
        return -1;

    if(!_info)
        return -1;

    XScreenSaverQueryInfo(_display, DefaultRootWindow(_display), _info);
    _idleTime = _info->idle;

    return _idleTime;
}

/* Get user defined idle value.
 * Default value is 3 minutes.
 *
 * Value is defined in milliseconds.
 */
long IdleDetector::getWantedIdleValue()
{
    return _wantedIdleValueSetting;
}

void IdleDetector::setWantedIdleTime(long value)
{
    _wantedIdleValueSetting = value;
}
