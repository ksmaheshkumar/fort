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

#ifndef SECURITY_H
#define SECURITY_H

#include <QString>
#include <botan/symkey.h>

class Security
{
public:
    Security();
    bool encryptAll();
    bool decryptAll();
    void setMasterPassphraseHash(QString hash);
    void clearMasterPassphraseHashFromMemory();
    static QString createHashFromString(QString str);
    Botan::SymmetricKey getSymmetricKeyFromHash(QString hash);
    QString getLastErrorMessage();
    bool comparePassphraseHash(QString hash);
    bool preservePassphraseBcrypt(QString plain);
    bool validateLogin(QString plain);

private:
    QString _currentPassphraseHash;
    QString _lastErrorMessage;
};

#endif // SECURITY_H
