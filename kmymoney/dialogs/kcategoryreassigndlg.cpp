/*
 * Copyright 2007-2008  Thomas Baumgart <tbaumgart@kde.org>
 * Copyright 2017-2018  Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
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

#include "kcategoryreassigndlg.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QPushButton>

// ----------------------------------------------------------------------------
// KDE Includes

#include <KMessageBox>
#include <kguiutils.h>
#include <KLocalizedString>

// ----------------------------------------------------------------------------
// Project Includes

#include "ui_kcategoryreassigndlg.h"

#include "mymoneyfile.h"
#include "mymoneyaccount.h"
#include "kmymoneycategory.h"
#include "kmymoneyaccountselector.h"
#include "mymoneyenums.h"

KCategoryReassignDlg::KCategoryReassignDlg(QWidget* parent) :
  QDialog(parent),
  ui(new Ui::KCategoryReassignDlg)
{
  ui->setupUi(this);
  auto mandatory = new KMandatoryFieldGroup(this);
  mandatory->add(ui->m_category);
  mandatory->setOkButton(ui->buttonBox->button(QDialogButtonBox::Ok));
}

KCategoryReassignDlg::~KCategoryReassignDlg()
{
  delete ui;
}

QString KCategoryReassignDlg::show(const MyMoneyAccount& category)
{
  if (category.id().isEmpty())
    return QString(); // no payee available? nothing can be selected...

  AccountSet set;
  set.addAccountGroup(eMyMoney::Account::Type::Income);
  set.addAccountGroup(eMyMoney::Account::Type::Expense);
  set.load(ui->m_category->selector());

  // remove the category we are about to delete
  ui->m_category->selector()->removeItem(category.id());

  // make sure the available categories have the same currency
  QStringList list;
  QStringList::const_iterator it_a;
  ui->m_category->selector()->itemList(list);
  for (it_a = list.constBegin(); it_a != list.constEnd(); ++it_a) {
    MyMoneyAccount acc = MyMoneyFile::instance()->account(*it_a);
    if (acc.currencyId() != category.currencyId())
      ui->m_category->selector()->removeItem(*it_a);
  }

  // reload the list
  ui->m_category->selector()->itemList(list);

  // if there is no category for reassignment left, we bail out
  if (list.isEmpty()) {
    KMessageBox::sorry(this, QString("<qt>") + i18n("At least one transaction/schedule still references the category <b>%1</b>.  However, at least one category with the same currency must exist so that the transactions/schedules can be reassigned.", category.name()) + QString("</qt>"));
    return QString();
  }

  // execute dialog and if aborted, return empty string
  if (this->exec() == QDialog::Rejected)
    return QString();

  // otherwise return index of selected payee
  return ui->m_category->selectedItem();
}


void KCategoryReassignDlg::accept()
{
  // force update of payeeCombo
  ui->buttonBox->button(QDialogButtonBox::Ok)->setFocus();

  if (ui->m_category->selectedItem().isEmpty())
    KMessageBox::information(this, i18n("This dialog does not allow new categories to be created. Please pick a category from the list."), i18n("Category creation"));
  else
    QDialog::accept();
}
