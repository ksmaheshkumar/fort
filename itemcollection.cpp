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

#include "itemcollection.h"
#include <QtAlgorithms>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "environment.h"

/* Load item list from the filesystem.
 * This method is called after the data has been
 * decrypted.
 */
void ItemCollection::loadItems()
{
    _list.clear();

    QString path = Environment::ensurePath();
    QDir dir(path);
    QStringList filters;
    filters << "*.plain";

    QFileInfoList entries = dir.entryInfoList(filters,QDir::Files | QDir::NoDotAndDotDot);

    foreach(QFileInfo entryInfo,entries)
    {
        QString filePath = entryInfo.absoluteFilePath();
        QFile file(filePath);

        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            //In order: title,user,password,isFav,url,ID,notes
            //If, in the future, the file format changes notes must be the last thing
            //to be written into the file as it may contain multiple lines.
            QString title = in.readLine();
            QString user = in.readLine();
            QString password = in.readLine();
            bool fav = in.readLine().toInt();
            QString url = in.readLine();
            QString id = in.readLine();

            //Read the rest of the file to get the notes(if any)
            QString notes = in.readAll();

            Item item(title,user,password);
            item.setUrl(url);
            item.setID(id);
            item.setFavorite(fav);
            item.setNotes(notes);

            this->addItem(item);

            file.close();
        }
    }
}

/* Add an item to the internal item collection.
 * This method also adds the new item to the filesystem.
 */
void ItemCollection::addItem(Item &item)
{
    _list << item;
    if(item.getIsFavorite())
        this->setItemToTop(this->itemCount()-1);

    QString path = Environment::ensurePath();

    QFile file(path + item.getID() + ".plain");

    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QString isNumber = QString::number(item.getIsFavorite());
        QTextStream out(&file);

        out << item.getTitle() + '\n';
        out << item.getUser() + '\n';
        out << item.getPassword() + '\n';
        out << isNumber + '\n';
        out << item.getUrl() + '\n';
        out << item.getID() + '\n';
        out << item.getNotes();

        file.close();
    }
}

/* Return an item by index from the collection.
 */
Item ItemCollection::getItem(int index)
{
    return _list.at(index);
}

/* Return an object of an item by guid.
 * If an item with matching guid is not found
 * an empty item is returned.
 */
Item ItemCollection::getItemByGuid(QString guid)
{
    Item item;

    for(int i = 0; i < _list.count(); i++)
    {
        item = _list.at(i);

        if(item.getID().compare(guid) == 0)
            return item;
    }

    return item;
}

/* Return an item index by the item name.
 * Loops through the collection until an item
 * with matching name is found.
 *
 * If an item is not found -1 will be returned.
 */
int ItemCollection::getItemIndexByName(QString name)
{
    for(int i = 0; i < this->itemCount(); i++)
    {
        if(_list.at(i)._title == name)
            return i;
    }

    return -1;
}

/* Clear the collection.
 */
void ItemCollection::clearItems()
{
    _list.clear();
}

/* Replaces the item list with another list that
 * only hash items which match the search term.
 *
 * Original list is backed up.
 */
void ItemCollection::createSearchView(QString searchTerm)
{
    _searchSet.clear();
    _backupList = _list;

    for(int i = 0; i < this->itemCount(); i++)
    {
        Item item = this->getItem(i);

        if(item.getTitle().
                contains(searchTerm,Qt::CaseInsensitive))
        {
            if(!_searchSet.contains(item))
                _searchSet << item;
        }
    }

    _list = QList<Item>::fromSet(_searchSet);

    for(int i = 0; i < this->itemCount(); i++)
        if(this->getItem(i).getIsFavorite())
            this->setItemToTop(i);
}

/* Removes an item from the internal list
 * as well as from the filesystem by an index.
 */
void ItemCollection::removeItem(int index)
{
    QFile::remove(Environment::ensurePath() + _list[index].getID() + ".plain");
    _list.removeAt(index);
}

/* Get count of the items.
 */
int ItemCollection::itemCount()
{
    return _list.count();
}

/* Sort items alphabetically in the list.
 * From a to z.
 *
 * Method is not in use.
 */
void ItemCollection::sortItemsAscending()
{
    qSort(_list.begin(),_list.end());
}

/* Sort items from z to a.
 * Method is not in use.
 */
void ItemCollection::sortItemsDescending()
{
    qSort(_list.begin(),_list.end(),qGreater<Item>());
}

/* Move an item to be the first one in the list.
 * This method is used when an item is tagged as a favorite.
 * Favorite items are always on the top of the list.
 */
void ItemCollection::setItemToTop(int itemIndex)
{
    _list.move(itemIndex,0);
}
