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

#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

/* Constructor.
 * Setup ui and default variables.
 */
LogInDialog::LogInDialog(Security *sec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogInDialog)
{
    ui->setupUi(this);

    //Set a nice looking style for the "header" of the dialog.
    //This should probably be in the separate file...

    ui->frame->setStyleSheet("#label{border:none;color:white;}#frame{border: none;background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #a6a6a6, stop: 0.08 #7f7f7f,stop: 0.39999 #717171, stop: 0.4 #626262,stop: 0.9 #4c4c4c, stop: 1 #333333);}");
    _sec = sec;

    //As passphrase input field theme is modified on error, take a copy of
    //the original style of the widget.
    _lineEditPassStyle = ui->linePassword->styleSheet();
}

/* Deconstructor.
 * Delete ui.
 */
LogInDialog::~LogInDialog()
{
    delete ui;
}

/* Checkbox state. So password when toggled to true.
 */
void LogInDialog::on_checkBox_toggled(bool checked)
{
    if(checked)
        ui->linePassword->setEchoMode(QLineEdit::Normal);
    else
        ui->linePassword->setEchoMode(QLineEdit::Password);
}

/* Cancel and close the dialog.
 */
void LogInDialog::on_pushButtonCancel_clicked()
{
    this->close();
    this->setResult(QDialog::Rejected);
}

/* Called when OK button is clicked.
 * Perform decryption of the data.
 *
 * On decryption failure passphrase field css style is modified
 * to indicate an error. Last error message is also displayed.
 * On failure the dialog does not close.
 */
void LogInDialog::on_pushButtonOK_clicked()
{
    this->setCursor(Qt::WaitCursor);

    //First validate the passphrase.
    if(_sec->validateLogin(ui->linePassword->text().trimmed()))
    {
        //When login dialog is active the data is always encrypted
        //This method will decrypt the data
        QString hash = Security::createHashFromString(ui->linePassword->text().trimmed());
        _sec->setMasterPassphraseHash(hash);

        //If working return Accepted result
        if(_sec->decryptAll())
        {
            this->close();
            this->setResult(QDialog::Accepted);
        }
        else
        {
            //Clear password field and set an error color if passphrase is invalid
            ui->linePassword->setText("");
            ui->linePassword->setStyleSheet("QLineEdit{border:2px solid red;}");

            QMessageBox::information(this,"Fort Password Manager",_sec->getLastErrorMessage());
        }
    }
    else
    {
        QMessageBox::information(this,"Fort Password Manager",_sec->getLastErrorMessage());
        ui->linePassword->setText("");
        ui->linePassword->setStyleSheet("QLineEdit{border:2px solid red;}");
    }

    this->setCursor(Qt::ArrowCursor);
}

/* If an error occurred on decryption, passphrase field css style
 * is modified. Once user starts typing the password again this
 * method will reset the passphrase field to the original one.
 */
void LogInDialog::on_linePassword_cursorPositionChanged(int /* Unused param */, int /* Unused param */)
{
     ui->linePassword->setStyleSheet(_lineEditPassStyle);
}

/* Allow user to quit the application without
 * typing the passphrase.
 */
void LogInDialog::on_pushButtonExit_clicked()
{
    QApplication::instance()->quit();
}
