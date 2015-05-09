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

#include "dataexporter.h"
#include "item.h"
#include <QFile>
#include <QTextStream>

/* Constructor.
 */
DataExporter::DataExporter(ItemCollection *collection)
{
    _collection = collection;
}

/* Export all items as plain text to a file.
 * Method does not export item guid at all as the guid
 * is unnecessary information for the user.
 *
 * If the export path exists, it will be overwritten.
 *
 * Return true on success, false on failure.
 */
bool DataExporter::exportAll(QString filepath)
{
    bool retval = false;
    QFile file(filepath);

    if(file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        QTextStream outStream(&file);

        for(int i = 0; i < _collection->itemCount(); i++)
        {
            Item item = _collection->getItem(i);

            QString line = item.getTitle() + "\t" + item.getUser() + "\t" +
                    item.getPassword() + "\t" + item.getUrl() + "\t" + item.getNotes() + "\n";

            outStream << line;
        }

        file.close();
        retval = true;
    }

    return retval;
}

/* Export one item, selected by guid as plain text to
 * a file. If the export path exists, it will be overwritten.
 *
 * Method does not export guid of the item as user does not need
 * it.
 *
 * Returns true on success, false on failure.
 */
bool DataExporter::exportOneByGuid(QString guid, QString filepath)
{
    bool retval = false;

    Item item = _collection->getItemByGuid(guid);

    //We can safely assume that if the item is not empty
    //it has all the necessary data for the export.
    if(!item.isEmpty())
    {
        QFile file(filepath);

        if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QString line = item.getTitle() + "\t" + item.getUser() + "\t" +
                    item.getPassword() + "\t" + item.getUrl() + "\t" + item.getNotes() + "\n";

            QTextStream outStream(&file);
            outStream << line;

            file.close();
            retval = true;
        }
    }

    return retval;
}
