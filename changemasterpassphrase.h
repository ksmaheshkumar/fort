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

#ifndef CHANGEMASTERPASSPHRASE_H
#define CHANGEMASTERPASSPHRASE_H

#include <QDialog>
#include <QString>
#include "security.h"

namespace Ui {
class changemasterpassphrase;
}

class changemasterpassphrase : public QDialog
{
    Q_OBJECT

public:
    explicit changemasterpassphrase(Security *sec, QWidget *parent = 0);
    ~changemasterpassphrase();
    QString getNewPassphraseHash();

private slots:
    void on_lineEditCurrentPass_textChanged(const QString &arg1);
    void on_lineEditPass_textEdited(const QString &arg1);
    void on_lineEditPassVerify_textChanged(const QString &arg1);
    void on_lineEditCurrentPass_cursorPositionChanged(int arg1, int arg2);
    void on_lineEditPassVerify_cursorPositionChanged(int arg1, int arg2);
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::changemasterpassphrase *ui;
    bool validatePassphrase();
    bool validateInput();
    QString _originalPassphraseStyle;
    QString _passphraseStyle;
    QString _passphraseVerifyStyle;
    QString _onErrorStyle;
    Security *_sec;
};

#endif // CHANGEMASTERPASSPHRASE_H
