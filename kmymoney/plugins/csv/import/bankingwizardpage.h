/*
 * Copyright 2011-2017  Allan Anderson <agander93@gmail.com>
 * Copyright 2016-2018  Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BANKINGWIZARDPAGE_H
#define BANKINGWIZARDPAGE_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "csvwizardpage.h"

// ----------------------------------------------------------------------------

class BankingProfile;
class MyMoneyStatement;

namespace Ui
{
class BankingPage;
}

class BankingPage : public CSVWizardPage
{
  Q_OBJECT

public:
  explicit BankingPage(CSVWizard *dlg, CSVImporterCore *imp);
  ~BankingPage();

  bool validateCreditDebit();
  /**
  * This method fills QIF file with bank/credit card data
  */
  void                makeQIF(const MyMoneyStatement &st, const QString &outFileName);

private:
  void initializePage() final override;
  bool isComplete() const final override;
  int nextId() const final override;

  bool                validateMemoComboBox();
  void                resetComboBox(const Column comboBox);
  bool                validateSelectedColumn(const int col, const Column type);

  BankingProfile       *m_profile;
  Ui::BankingPage      *ui;

private Q_SLOTS:
  void                memoColSelected(int col);
  void                categoryColSelected(int col);
  void                numberColSelected(int col);
  void                payeeColSelected(int col);
  void                dateColSelected(int col);
  void                debitColSelected(int col);
  void                creditColSelected(int col);
  void                amountColSelected(int col);
  void                amountToggled(bool checked);
  void                debitCreditToggled(bool checked);
  void                oppositeSignsClicked(bool checked);
  void                clearColumns();
};

#endif // BANKINGWIZARDPAGE_H
