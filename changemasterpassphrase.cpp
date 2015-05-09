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

#include "changemasterpassphrase.h"
#include "ui_changemasterpassphrase.h"
#include <QMessageBox>
#include <QPushButton>

/* Default constructor.
 */
changemasterpassphrase::changemasterpassphrase(Security *sec, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changemasterpassphrase)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    _sec = sec;
    //Take a copy of the original password field widget styles.
     _passphraseStyle = ui->lineEditPass->styleSheet();
     _originalPassphraseStyle = ui->lineEditCurrentPass->styleSheet();
     _passphraseVerifyStyle = ui->lineEditPassVerify->styleSheet();

     //Edit widget stylesheet for an error
     _onErrorStyle = "QLineEdit{border:2px solid red;}";

     ui->lineEditPass->setEchoMode(QLineEdit::Password);
     ui->lineEditPassVerify->setEchoMode(QLineEdit::Password);
     ui->lineEditCurrentPass->setEchoMode(QLineEdit::Password);
}

/* deconstructor to delete ui.
 */
changemasterpassphrase::~changemasterpassphrase()
{
    delete ui;
}

/* Called when current password field is changed.
 */
void changemasterpassphrase::on_lineEditCurrentPass_textChanged(const QString &/* Not in use */)
{
    validateInput();
}

/* Called when password field is changed.
 */
void changemasterpassphrase::on_lineEditPass_textEdited(const QString &/* Not in use */)
{
    validateInput();
}

/* Called when password verify field is changed.
 */
void changemasterpassphrase::on_lineEditPassVerify_textChanged(const QString &/* Not in use */)
{
    validateInput();
}

/* Validates current passphrase as well as the new one.
 * Returns true if all of them are valid.
 *
 * Method is called when user presses OK-button.
 */
bool changemasterpassphrase::validatePassphrase()
{
    QString pwd = ui->lineEditCurrentPass->text().trimmed();

    if(!_sec->validateLogin(pwd))
    {
        ui->lineEditCurrentPass->setText("");
        ui->lineEditCurrentPass->setStyleSheet(_onErrorStyle);
        QMessageBox::information(this,"Fort Password Manager",
                                 "Current passphrase does not match.");
        return false;
    }

    QString p1 = ui->lineEditPass->text().trimmed();
    QString p2 = ui->lineEditPassVerify->text().trimmed();

    if (p1.compare(p2) == 0)
        return true;

    ui->lineEditPassVerify->setText("");
    ui->lineEditPassVerify->setStyleSheet(_onErrorStyle);
    QMessageBox::information(this,"Fort Password Manager",
                             "Passphrases do not match.");
    return false;
}

/* Returns new passphrase hash as QString.
 */
QString changemasterpassphrase::getNewPassphraseHash()
{
    QString passphrase = ui->lineEditPassVerify->text().trimmed();
    QString hash = Security::createHashFromString(passphrase);

    return hash;
}

/* Simply checks if all the input fields has some content.
 * If there is data, OK-button is enabled.
 */
bool changemasterpassphrase::validateInput()
{
    if(ui->lineEditCurrentPass->text().length() > 0 &&
            ui->lineEditPass->text().length() > 0 &&
            ui->lineEditPassVerify->text().length() > 0)
    {
         ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
         return true;
    }
    else
    {
         ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }

    return false;
}

/* When current passphrase field is edited, reset the original theme
 * of the widget.
 */
void changemasterpassphrase::on_lineEditCurrentPass_cursorPositionChanged
(int /* not in use */, int /* not in use */)
{
    ui->lineEditCurrentPass->setStyleSheet(_originalPassphraseStyle);
}

/* When verify passphrase field is edited, reset the original theme
 * of the widget.
 */
void changemasterpassphrase::on_lineEditPassVerify_cursorPositionChanged
(int /* not in use */, int /* not in use */)
{
    ui->lineEditPassVerify->setStyleSheet(_passphraseVerifyStyle);
}

/* When OK-button is clicked.
 *
 * Validates passphrases and sends Accepted result if
 * they match closing the dialog.
 */
void changemasterpassphrase::on_buttonBox_accepted()
{
    this->setCursor(Qt::WaitCursor);

    if(validatePassphrase())
    {
        _sec->preservePassphraseBcrypt(ui->lineEditPassVerify->text().trimmed());
        this->close();
        this->setResult(QDialog::Accepted);
    }

    this->setCursor(Qt::ArrowCursor);
}

void changemasterpassphrase::on_buttonBox_rejected()
{
    this->close();
    this->setResult(QDialog::Rejected);
}
