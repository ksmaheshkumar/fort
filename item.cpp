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

#include "item.h"
#include <QUuid>

/* An empty constructor mainly for creating an object
 * without filling all the data.
 */
Item::Item()
{
    _isEmpty = true;
}

/* Constructor. Set default variable values and
 * create a guid for the item.
 */
Item::Item(const QString &title,const QString &user,
           const QString &password)
{
    _title = title;
    _user = user;
    _password = password;

    //item unique ID
    _ID = QUuid::createUuid().toString();
    _isFavorite = false;
    _isEmpty = false;
}

/* Set item url
 */
void Item::setUrl(const QString &url)
{
    _url = url.trimmed();

    if(!_url.isEmpty())
    {
        if( (!_url.startsWith("http://")) && (!_url.startsWith("https://")))
            _url = "http://" + _url;
    }

    _isEmpty = false;
}

/* Allows to override current ID (guid)
 * This should be only used when loading item data from file.
 */
void Item::setID(QString id)
{
    _ID = id;
    _isEmpty = false;
}

/* Get username of the item as QString.
 */
QString Item::getUser()
{
    return _user;
}

/* Get notes of the item as QString.
 */
QString Item::getNotes()
{
    return _notes;
}

/* Set item notes.
 */
void Item::setNotes(QString text)
{
    _notes = text;
    _isEmpty = false;
}

/* Get title of the item as QString.
 */
QString Item::getTitle()
{
    return _title;
}

/* Get plain password of the item as QString.
 */
QString Item::getPassword()
{
    return _password;
}

/* Get url of the item as QString.
 */
QString Item::getUrl()
{
    return _url;
}

/* Get username of the item as QUrl.
 * Used to launch the url on systems default browser.
 */
QUrl Item::getUrlAsQUrl()
{
    QUrl url(_url);
    return url;
}

/* Get guid of the item as QString.
 */
QString Item::getID()
{
    return _ID;
}

/* Return either true of false depending
 * if the item has an url or not.
 */
bool Item::getHasUrl()
{
    if(_url.isEmpty() || _url.isNull())
        return false;

    return true;
}

/* Implement == operator for comparing items.
 */
bool Item::operator==(const Item& other) const
{
    return this->_title == other._title;
}

/* Implement < operator for comparing items.
 */
bool Item::operator<(const Item& other) const
{
    return this->_title < other._title;
}

/* Returns true or false depending if the item
 * has data or not.
 */
bool Item::isEmpty()
{
    return _isEmpty;
}

/* Return true or false depending if the item
 * is tagged as a favorite or not.
 */
bool Item::getIsFavorite()
{
    return _isFavorite;
}

/* Tag item favorite status.
 */
void Item::setFavorite(bool value)
{
    _isFavorite = value;
    _isEmpty = false;
}
