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

#include "masterpassphrasesetup.h"
#include "ui_masterpassphrasesetup.h"
#include <QPushButton>
#include <QMessageBox>
#include "security.h"

/* Dialog implementation for setting the master passphrase.
 * This constructor disables OK button and sets text fields to password mode.
 */
MasterPassphraseSetup::MasterPassphraseSetup(Security *sec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MasterPassphraseSetup)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->lineEditPass->setEchoMode(QLineEdit::Password);
    ui->lineEditPassVerify->setEchoMode(QLineEdit::Password);

    _sec = sec;
}

/* Deconstructor to delete ui.
 */
MasterPassphraseSetup::~MasterPassphraseSetup()
{
    delete ui;
}

/* This method simply checks that given passphrases are
 * equal.
 */
bool MasterPassphraseSetup::verifyPassword(QString pass)
{
    QString password = ui->lineEditPass->text().trimmed();

    if(password.compare(pass,Qt::CaseSensitive) != 0)
        return false;

    return true;
}

/* Called each type textfield is changed.
 * Checks that given passphrases are equal.
 * On equal input OK-button is enabled and password is stored.
 */
void MasterPassphraseSetup::on_lineEditPassVerify_textChanged(const QString &arg1)
{
    if(verifyPassword(arg1.trimmed()))
         ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    else
         ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    _password = ui->lineEditPass->text().trimmed();
}

/* Method is used to get SHA256 hex encoded hash
 * of the passphrase.
 */
QString MasterPassphraseSetup::getPasswordHash()
{
    QString hash = Security::createHashFromString(_password);
    return hash;
}

/* Allow accepted only if we can successfully preserve the passphrase
 * as bcrypted hash.
 */
void MasterPassphraseSetup::on_buttonBox_accepted()
{
    if(_sec->preservePassphraseBcrypt(_password))
    {
        this->close();
        this->setResult(QDialog::Accepted);
    }
    else
    {
        QMessageBox::information(this,"Fort Password Manager",_sec->getLastErrorMessage());
        this->close();
        this->setResult(QDialog::Rejected);
    }
}
