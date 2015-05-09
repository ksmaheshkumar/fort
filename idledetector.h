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

#ifndef IDLEDETECTOR_H
#define IDLEDETECTOR_H

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>

class IdleDetector
{
public:
    IdleDetector();
    ~IdleDetector();
    long getIdleTime();
    long getWantedIdleValue();
    void setWantedIdleTime(long value);
private:
    unsigned long _idleTime;
    long _wantedIdleValueSetting;
    Display *_display;
    XScreenSaverInfo *_info;
};

#endif // IDLEDETECTOR_H
