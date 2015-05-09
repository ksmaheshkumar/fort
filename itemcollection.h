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

#ifndef ITEMCOLLECTION_H
#define ITEMCOLLECTION_H

#include <QList>
#include <QSet>
#include "item.h"

class ItemCollection
{
public:
    ItemCollection(){}
    void addItem(Item &item);
    Item getItem(int index);
    Item getItemByGuid(QString guid);
    void removeItem(int index);
    int itemCount();
    void loadItems();
    void sortItemsAscending();
    void sortItemsDescending();
    void setItemToTop(int itemIndex);
    void createSearchView(QString searchTerm);
    QList<Item> _backupList;
    QList<Item> _list;
    int getItemIndexByName(QString name);
    void clearItems();
private:


    QSet<Item> _searchSet;
};

#endif // ITEMCOLLECTION_H
