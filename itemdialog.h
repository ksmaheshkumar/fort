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

#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ItemDialog;
}

class ItemDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ItemDialog(QWidget *parent = 0);
    ~ItemDialog();
    QString getTitle();
    QString getUser();
    QString getPassword();
    QString getUrl();
    QString getNotes();
    void setData(const QString &title,const QString &user,const QString &password,
                 const QString &url, const QString &notes, bool isFavorite);
    bool getIsFavorite();
private slots:
    void on_lineTitle_2_textChanged(const QString &arg1);
    void on_lineUser_textChanged(const QString &arg1);
    void on_linePassword_textChanged(const QString &arg1);
    void on_lineEditUrl_textChanged(const QString &arg1);
    void on_checkBox_toggled(bool checked);
    void on_textEdit_textChanged();
private:
    Ui::ItemDialog *ui;
    QString _title;
    QString _user;
    QString _password;
    QString _url;
    QString _notes;
    void checkUserInput();
};

#endif // ITEMDIALOG_H
