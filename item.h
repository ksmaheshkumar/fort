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

#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <QUrl>
#include <QIcon>
#include <QHash>

class Item
{
public:
    Item();
    Item(const QString &title,const QString &user,const QString &password);
    void setUrl(const QString &url);
    QString getTitle();
    QString getUser();
    QString getPassword();
    QString getUrl();
    QUrl getUrlAsQUrl();
    QString getNotes();
    QString getID();
    void setID(QString id);
    void setNotes(QString text);
    bool getHasUrl();
    bool getIsFavorite();
    void setFavorite(bool value);
    bool operator==(const Item &other) const;
    bool operator<(const Item &other) const;
    QString _title;
    QString _ID;
    bool isEmpty();

private:
    QString _user;
    QString _password;
    QString _url;
    bool _isFavorite;
    QString _notes;
    bool _isEmpty;

};

inline uint qHash(const Item &other)
{
    return qHash(other._ID) ^ 0x9e3779b9;
}

#endif // ITEM_H
