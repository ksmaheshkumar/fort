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

#include "security.h"
#include <botan/auto_rng.h>
#include <botan/sha2_32.h>
#include <botan/pipe.h>
#include <botan/botan.h>
#include <botan/base64.h>
#include <botan/bcrypt.h>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include "environment.h"
#include <iostream>

using namespace Botan;

Security::Security()
{

}

/* Encrypt each item file using AES with 128 bit initialization vector and 256 bit key.
 *
 * Encrypted data is encoded with base64.
 *
 * Initialization vector is preserved to a file for decryption.
 * Function returns true on success and false on failure.
 * On failure _lastErrorMessage is set. It can be accessed via
 * Security::getLastErrorMessage()
 */
bool Security::encryptAll()
{
    AutoSeeded_RNG rng;
    InitializationVector iv(rng,16); //128bits
    SymmetricKey key = this->getSymmetricKeyFromHash(this->_currentPassphraseHash);

    QString path = Environment::ensurePath();
    QDir dir(path);
    QStringList filters;
    filters << "*.plain";

    QFileInfoList entries = dir.entryInfoList(filters,QDir::Files | QDir::NoDotAndDotDot);

    try
    {
        foreach(QFileInfo entryInfo,entries)
        {
            QString filePath = entryInfo.absoluteFilePath();
            QFile file(filePath);

            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                //Read all data from the file and encrypt it
                QTextStream in(&file);
                QString plainData = in.readAll();

                file.close();

                Pipe pipe(get_cipher("AES-256/CBC/PKCS7",key,iv,ENCRYPTION), new Base64_Encoder);
                pipe.process_msg(plainData.toStdString().c_str());
                QString encryptedData = QString::fromStdString(pipe.read_all_as_string(0));

                //Remove the file containing plain data and create a new one with the encrypted data.
                QFile::remove(filePath);
                filePath = filePath + ".enc";
                QFile fileOut(filePath);

                if(fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
                {
                    QTextStream out(&fileOut);
                    out << encryptedData;
                    fileOut.close();
                }
            }
        }

        //Write initialization vector to a file
        //If an old one exists, it will be overwritten
        QString ivPath = path + FORT_IV_FILE;
        QFile ivFile(ivPath);

        if(ivFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&ivFile);
            out << QString::fromStdString(iv.as_string());
            ivFile.close();
        }
    }
    catch(...)
    {
        _lastErrorMessage = "Something went wrong. Corrupted data or permission error.";
        return false;
    }

    return true;
}

/* Decrypt each encrypted (*.enc) item file.
 * Uses preserved initialization vector from a file.
 *
 * After successful decryption preserved IV is removed. On failure
 * an exception is thrown and function returns false.
 */
bool Security::decryptAll()
{

    QString path = Environment::ensurePath();
    QString ivPath = path + FORT_IV_FILE;
    QFile ivFile(ivPath);

    try
    {
        if(ivFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
              QTextStream in(&ivFile);
              QString plainIV = in.readAll();
              OctetString iv(plainIV.toStdString());

              ivFile.close();

              QDir dir(path);
              QStringList filters;
              filters << "*.enc";
              QFileInfoList entries = dir.entryInfoList(filters,QDir::Files | QDir::NoDotAndDotDot);

              SymmetricKey key = this->getSymmetricKeyFromHash(this->_currentPassphraseHash);

              //Loop through encrypted files and decrypt them
              foreach(QFileInfo entryInfo,entries)
              {
                  QString filePath = entryInfo.absoluteFilePath();
                  QFile file(filePath);

                  if(file.open(QIODevice::ReadOnly | QIODevice::Text))
                  {
                      QTextStream in(&file);
                      QString encryptedData;

                      in >> encryptedData;

                      file.close();

                      Pipe base64dec(new Base64_Decoder);
                      base64dec.process_msg(encryptedData.toStdString());

                      Pipe pipe(get_cipher("AES-256/CBC/PKCS7",key,iv,DECRYPTION));
                      pipe.process_msg(base64dec.read_all_as_string(0));

                      QString plainData = QString::fromStdString(pipe.read_all_as_string());

                      QFile::remove(filePath);
                      filePath = path + entryInfo.completeBaseName(); //Remove ".enc" extension from the file name
                      QFile fileOut(filePath);

                      if(fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
                      {
                          QTextStream out(&fileOut);
                          out << plainData;
                          fileOut.close();
                      }
                  }
              }
        }
        else
        {
            std::cout << "Missing initialization vector. Can't decrypt." << std::endl;
        }
    }
    catch(...)
    {
        _lastErrorMessage = "Something went wrong. Invalid passphrase or corrupted data.";
        return false;
    }

    //Remove the preserved initialization vector.
    if(Environment::hasIV())
        QFile::remove(ivPath);

    return true;
}

/* Set passphrase hash to use in encryption / decryption.
 * Hash algorithm is SHA256.
 */
void Security::setMasterPassphraseHash(QString hash)
{
    _currentPassphraseHash = hash;
}

/* Remove current passphrase hash. This implementation
 * is probably not safe. Passphrase hash should be stored
 * in a safe string implementation.
 */
void Security::clearMasterPassphraseHashFromMemory()
{
    //Fix this, not safe.
    _currentPassphraseHash = "";
}

/* Static method.
 *
 * Creates a hex encoded hash (using SHA256) from a string.
 * This method is used to create a hash from user passphrase.
 *
 * Returns the hash as QString.
 */
QString Security::createHashFromString(QString str)
{
    std::string password = str.toStdString();
    Pipe pipe(new Chain(new Hash_Filter(new SHA_256),new Hex_Encoder));

    pipe.process_msg(password);
    std::string hash =  pipe.read_all_as_string(0);

    return QString::fromStdString(hash);
}

/* Get symmetric key from the hash.
 * Returned key is 256bits.
 */
SymmetricKey Security::getSymmetricKeyFromHash(QString hash)
{
    SymmetricKey key(reinterpret_cast<const unsigned char*>(hash.toStdString().c_str()), 32); //256bits

    return key;
}

/* encryptAll and decryptAll methods will set _lastErrorMessage on failure.
 * This method is used to access that message.
 */
QString Security::getLastErrorMessage()
{
    return _lastErrorMessage;
}

/* Compare two QString instances. Returns true if they are equal.
 */
bool Security::comparePassphraseHash(QString hash)
{
    if(_currentPassphraseHash.compare(hash) == 0)
        return true;

    return false;
}

/* Preserve bcrypted passphrase to a file for validation purposes.
 * Bcrypt algorithm is used with work factor 12.
 *
 * This method is called by setup master passphrase dialog as well as
 * change master passphrase dialog.
 */
bool Security::preservePassphraseBcrypt(QString plain)
{
    AutoSeeded_RNG rng;
    std::string bhash = generate_bcrypt(plain.toStdString(), rng, 12); //Work factor 12 is secure enough.
    QString hash = QString::fromStdString(bhash);
    QString path = Environment::ensurePath();

    path = path + FORT_KEY_FILE;
    QFile file(path);

    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream out(&file);
        out << hash;
        file.close();

        return true;
    }

    _lastErrorMessage = "Unable to preserve passphrase hash.";

    return false;
}

/* Method validates user inputted login passphrase with
 * the preserved bcrypted one in order to check if the passphrase
 * is valid.
 *
 * Method is called from the login dialog as well as in the change
 * master password dialog.
 */
bool Security::validateLogin(QString plain)
{
    QString path = Environment::ensurePath();

    path = path + FORT_KEY_FILE;
    QFile file(path);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString hash = in.readLine();

        file.close();

        if(hash.length() != 60)
        {
            _lastErrorMessage = "Invalid hash length.";
            return false;
        }

        bool valid = check_bcrypt(plain.toStdString(), hash.toStdString());

        if(!valid)
        {
            _lastErrorMessage = "Invalid passphrase or corrupted data.";
            return false;
        }

        return true;
    }

    _lastErrorMessage = "Unable to validate passphrase.";

    return false;
}
