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

#include "itemdialog.h"
#include "ui_itemdialog.h"
#include <QPushButton>

/* Default constructor.
 */
ItemDialog::ItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemDialog)
{
    ui->setupUi(this);
    checkUserInput();
}

/*Deconstructor. Delete ui.
 */
ItemDialog::~ItemDialog()
{
    delete ui;
}

/* Keep OK button as disabled state until
 * username, password and title has been filled.
 * It's necessary to have at least that data to construct an item.
 */
void ItemDialog::checkUserInput()
{
    if(ui->lineTitle_2->text().isEmpty() || ui->lineUser->text().isEmpty() ||
            ui->linePassword->text().isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

/* Returns the title of an item as QString.
 */
QString ItemDialog::getTitle()
{
    return _title;
}

/* Returns the username of an item as QString.
 */
QString ItemDialog::getUser()
{
    return _user;
}

/* Returns the notes of an item as QString.
 */
QString ItemDialog::getNotes()
{
    return _notes;
}

bool ItemDialog::getIsFavorite()
{
    return ui->checkBoxTagAsFavorite->isChecked();
}

/* Returns the password of an item as QString.
 */
QString ItemDialog::getPassword()
{
    return _password;
}

/* Returns the url of an item as QString.
 */
QString ItemDialog::getUrl()
{
    return _url;
}

/*Called when title field is edited.
 */
void ItemDialog::on_lineTitle_2_textChanged(const QString &arg1)
{
    _title = arg1;
    checkUserInput();
}

/* Set data for the text fields.
 * This method is called when an item is edited.
 */
void ItemDialog::setData(const QString &title,const QString &user,
                         const QString &password,
                         const QString &url, const QString &notes, bool isFavorite)
{
    ui->lineTitle_2->setText(title);
    ui->lineUser->setText(user);
    ui->linePassword->setText(password);
    ui->lineEditUrl->setText(url);
    ui->textEdit->setText(notes);
    ui->checkBoxTagAsFavorite->setChecked(isFavorite);
}

/*Called when user field is edited.
 */
void ItemDialog::on_lineUser_textChanged(const QString &arg1)
{
    _user = arg1;
    checkUserInput();
}

/*Called when password field is edited.
 */
void ItemDialog::on_linePassword_textChanged(const QString &arg1)
{
    _password = arg1;
    checkUserInput();
}

/*Called when url field is edited.
 */
void ItemDialog::on_lineEditUrl_textChanged(const QString &arg1)
{
    _url = arg1;
}

/* Called when checkbox state is changed.
 *
 * If toggled set password field to show the password,
 * otherwise hide the password.
 */
void ItemDialog::on_checkBox_toggled(bool checked)
{
    if(checked)
        ui->linePassword->setEchoMode(QLineEdit::Normal);
    else
        ui->linePassword->setEchoMode(QLineEdit::Password);
}

/*Called when notes field is edited.
 */
void ItemDialog::on_textEdit_textChanged()
{
    _notes = ui->textEdit->toPlainText();
}


