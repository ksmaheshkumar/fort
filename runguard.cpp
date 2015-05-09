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

/* This class was originally copied from stackoverflow
 * See http://stackoverflow.com/questions/5006547/qt-best-practice-for-a-single-instance-app-protection
 */

#include "runguard.h"
#include <QCryptographicHash>

namespace
{

    QString generateKeyHash( const QString& key, const QString& salt )
    {
        QByteArray data;

        data.append( key.toUtf8() );
        data.append( salt.toUtf8() );
        data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();

        return data;
    }

}

RunGuard::RunGuard( const QString& key )
    : _key( key )
    , _memLockKey( generateKeyHash( key, "_memLockKey" ) )
    , _sharedmemKey( generateKeyHash( key, "_sharedmemKey" ) )
    , _sharedMem( _sharedmemKey )
    , _memLock( _memLockKey, 1 )
{
        QSharedMemory fix( _sharedmemKey );//Fix for *nix: http://habrahabr.ru/post/173281/
        fix.attach();
}

RunGuard::~RunGuard()
{
    release();
}

bool RunGuard::isAnotherRunning()
{
    if ( _sharedMem.isAttached() )
        return false;

    _memLock.acquire();
    const bool isRunning = _sharedMem.attach();

    if ( isRunning )
        _sharedMem.detach();

    _memLock.release();

    return isRunning;
}

bool RunGuard::tryToRun()
{
    if ( isAnotherRunning() )// Extra check
        return false;

    _memLock.acquire();

    const bool result = _sharedMem.create( sizeof( quint64 ) );
    _memLock.release();

    if ( !result )
    {
        release();
        return false;
    }

    return true;
}

void RunGuard::release()
{
    _memLock.acquire();

    if ( _sharedMem.isAttached() )
        _sharedMem.detach();

    _memLock.release();
}
