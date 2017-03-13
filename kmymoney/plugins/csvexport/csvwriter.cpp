/***************************************************************************
                        csvwriter.cpp  -  description
                             --------------------
    begin                : Wed Mar 20 2013
    copyright            : (C) 2013-03-20 by Allan Anderson
    email                : Allan Anderson agander93@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "csvwriter.h"
#include "csvexportdlg.h"

// ----------------------------------------------------------------------------
// QT Headers

#include <QFile>
#include <QList>
#include <QDebug>
#include <QStringBuilder>

// ----------------------------------------------------------------------------
// KDE Headers

#include <kmessagebox.h>
#include <KConfigGroup>
#include <KLocalizedString>

// ----------------------------------------------------------------------------
// Project Headers

#include "mymoneyfile.h"

CsvWriter::CsvWriter()
{
}

CsvWriter::~CsvWriter()
{
}

void CsvWriter::write(const QString& filename,
                      const QString& accountId, const bool accountData,
                      const bool categoryData,
                      const QDate& startDate, const QDate& endDate,
                      const QString& separator)
{
  m_separator = separator;
  QFile csvFile(filename);
  if (csvFile.open(QIODevice::WriteOnly)) {
    QTextStream s(&csvFile);
    s.setCodec("UTF-8");

    m_plugin->exporterDialog()->show();
    try {
      if (categoryData) {
        writeCategoryEntries(s);
      }

      if (accountData) {
        writeAccountEntry(s, accountId, startDate, endDate);
      }
      emit signalProgress(-1, -1);

    } catch (const MyMoneyException &e) {
      QString errMsg = i18n("Unexpected exception '%1' thrown in %2, line %3 "
                            "caught in MyMoneyCsvWriter::write()", e.what(), e.file(), e.line());

      KMessageBox::error(0, errMsg);
    }

    csvFile.close();
    qCDebug(LOG_KMYMONEY) << i18n("Export completed.\n");
    delete m_plugin->exporterDialog();  //  Can now delete as export finished
  } else {
    KMessageBox::error(0, i18n("Unable to open file '%1' for writing", filename));
  }
}

void CsvWriter::writeAccountEntry(QTextStream& stream, const QString& accountId, const QDate& startDate, const QDate& endDate)
{
  MyMoneyFile* file = MyMoneyFile::instance();
  MyMoneyAccount account;
  QString data;

  account = file->account(accountId);
  MyMoneyTransactionFilter filter(accountId);

  QString type = account.accountTypeToString(account.accountType());
  data = QString(i18n("Account Type:"));

  if (type == QLatin1String("Investment")) {
    data += QString("%1\n\n").arg(type);
    m_headerLine << QString(i18n("Date")) << QString(i18n("Security")) << QString(i18n("Action/Type")) << QString(i18n("Amount")) << QString(i18n("Quantity")) << QString(i18n("Price")) << QString(i18n("Interest")) << QString(i18n("Fees")) << QString(i18n("Account")) << QString(i18n("Memo")) << QString(i18n("Status"));
    data += m_headerLine.join(m_separator);
    extractInvestmentEntries(accountId, startDate, endDate);
  } else {
    data += QString("%1\n\n").arg(type);
    m_headerLine << QString(i18n("Date")) << QString(i18n("Payee")) << QString(i18n("Amount")) << QString(i18n("Account/Cat")) << QString(i18n("Memo")) << QString(i18n("Status")) << QString(i18n("Number"));
    filter.setDateFilter(startDate, endDate);

    QList<MyMoneyTransaction> trList = file->transactionList(filter);
    QList<MyMoneyTransaction>::ConstIterator it;
    signalProgress(0, trList.count());
    int count = 0;
    m_highestSplitCount = 0;
    for (it = trList.constBegin(); it != trList.constEnd(); ++it) {
      writeTransactionEntry(*it, accountId, ++count);
      if (m_noError)
        signalProgress(count, 0);
    }
    data += m_headerLine.join(m_separator);
  }

  QString result;
  QMap<QString, QString>::const_iterator it_map = m_map.constBegin();
  while (it_map != m_map.constEnd()) {
    result += it_map.value();
    ++it_map;
  }

  stream << data << result << QLatin1Char('\n');
}

void CsvWriter::writeCategoryEntries(QTextStream &s)
{
  MyMoneyFile* file = MyMoneyFile::instance();
  MyMoneyAccount income;
  MyMoneyAccount expense;

  income = file->income();
  expense = file->expense();

  QStringList list = income.accountList() + expense.accountList();
  emit signalProgress(0, list.count());
  QStringList::Iterator it_catList;
  int count = 0;
  for (it_catList = list.begin(); it_catList != list.end(); ++it_catList) {
    writeCategoryEntry(s, *it_catList, "");
    emit signalProgress(++count, 0);
  }
}

void CsvWriter::writeCategoryEntry(QTextStream &s, const QString& accountId, const QString& leadIn)
{
  MyMoneyAccount acc = MyMoneyFile::instance()->account(accountId);
  QString name = acc.name();

  s << leadIn << name << m_separator;
  s << (acc.accountGroup() == MyMoneyAccount::Expense ? QLatin1Char('E') : QLatin1Char('I'));
  s << endl;

  QStringList list = acc.accountList();
  QStringList::Iterator it_catList;

  name += m_separator;
  for (it_catList = list.begin(); it_catList != list.end(); ++it_catList) {
    writeCategoryEntry(s, *it_catList, name);
  }
}


void CsvWriter::writeTransactionEntry(const MyMoneyTransaction& t, const QString& accountId, const int count)
{
  m_firstSplit = true;
  m_noError = true;
  MyMoneyFile* file = MyMoneyFile::instance();
  MyMoneySplit split = t.splitByAccount(accountId);
  QList<MyMoneySplit> splits = t.splits();
  if (splits.count() < 2) {
    KMessageBox::sorry(0, i18n("Transaction number '%1' is missing an account assignment.\n"
                               "Date '%2', Payee '%3'.\nTransaction dropped.\n", count, t.postDate().toString(Qt::ISODate), file->payee(split.payeeId()).name()),
                       i18n("Invalid transaction"));
    m_noError = false;
    return;
  }

  QString str;
  str += QLatin1Char('\n');

  str += QString("%1" + m_separator).arg(t.postDate().toString(Qt::ISODate));
  MyMoneyPayee payee = file->payee(split.payeeId());
  str += QString("%1" + m_separator).arg(payee.name());

  QString txt = split.value().formatMoney("", 2, false);
  str += QString("%1" + m_separator).arg(txt);

  if (splits.count() > 1) {
    MyMoneySplit sp = t.splitByAccount(accountId, false);
    QString tmp = QString("%1").arg(file->accountToCategory(sp.accountId()));
    str += tmp + m_separator;
  }
  QString memo = split.memo();
  memo.replace('\n', '~').remove('\'');
  QString localeThousands = QLocale().groupSeparator();  //  In case of clash with field separator
  if (m_separator == localeThousands) {
    memo.replace(localeThousands, QString());
  }

  str += QString("%1" + m_separator).arg(memo);

  switch (split.reconcileFlag()) {
    case MyMoneySplit::Cleared:
      str += QLatin1String("C") + m_separator;
      break;

    case MyMoneySplit::Reconciled:
    case MyMoneySplit::Frozen:
      str += QLatin1String("R") + m_separator;
      break;

    default:
      str += m_separator;
      break;
  }

  str += split.number();

  if (splits.count() > 2) {
    QList<MyMoneySplit>::ConstIterator it;
    for (it = splits.constBegin(); it != splits.constEnd(); ++it) {
      if (!((*it) == split)) {
        writeSplitEntry(str, *it, splits.count() - 1);
      }
    }
  }
  QString date = t.postDate().toString(Qt::ISODate);
  m_map.insertMulti(date, str);
}

void CsvWriter::writeSplitEntry(QString &str, const MyMoneySplit& split, const int splitCount)
{
  if (m_firstSplit) {
    m_firstSplit = false;
    str += m_separator;
  }
  MyMoneyFile* file = MyMoneyFile::instance();
  QString splt = QString("%1").arg(file->accountToCategory(split.accountId()));
  str += splt + m_separator;

  if (splitCount > m_highestSplitCount) {
    m_highestSplitCount++;
    m_headerLine << QString(i18n("splitCategory")) << QString(i18n("splitMemo")) << QString(i18n("splitAmount"));
    m_headerLine.join(m_separator);
  }
  QString m = split.memo();
  m.replace(QLatin1Char('\n'), QLatin1Char('~'));
  QString localeThousands = QLocale().groupSeparator();  //  In case of clash with field separator
  if (m_separator == localeThousands) {
    m.replace(localeThousands, QString());
  }
  str += QString("%1" + m_separator).arg(m);

  QString txt = QString("%1" + m_separator).arg(split.value().formatMoney("", 2, false));
  str += txt;
}

void CsvWriter::extractInvestmentEntries(const QString& accountId, const QDate& startDate, const QDate& endDate)
{
  MyMoneyFile* file = MyMoneyFile::instance();
  QList<QString> accList = file->account(accountId).accountList();
  QList<QString>::ConstIterator itAcc;

  for (itAcc = accList.constBegin(); itAcc != accList.constEnd(); ++itAcc) {
    MyMoneyTransactionFilter filter((*itAcc));
    filter.setDateFilter(startDate, endDate);
    QList<MyMoneyTransaction> list = file->transactionList(filter);
    QList<MyMoneyTransaction>::ConstIterator itList;
    signalProgress(0, list.count());
    int count = 0;
    for (itList = list.constBegin(); itList != list.constEnd(); ++itList) {
      writeInvestmentEntry(*itList, ++count);
      signalProgress(count, 0);
    }
  }
}

void CsvWriter::writeInvestmentEntry(const MyMoneyTransaction& t, const int count)
{
  QString strQuantity;
  QString strAmount;
  QString strPrice;
  QString strAccName;
  QString strCheckingAccountName;
  QString strMemo;
  QString strAction;
  QString strStatus;
  QString strInterest;
  QString strFees;
  MyMoneyFile* file = MyMoneyFile::instance();
  QString chkAccnt;
  QList<MyMoneySplit> lst = t.splits();
  QList<MyMoneySplit>::Iterator itSplit;
  MyMoneyAccount::_accountTypeE typ;
  QString chkAccntId;
  MyMoneyMoney qty;
  MyMoneyMoney value;
  QMap<MyMoneyAccount::_accountTypeE, QString> map;

  for (int i = 0; i < lst.count(); i++) {
    MyMoneyAccount acc = file->account(lst[i].accountId());
    QString accName = acc.name();
    typ = acc.accountType();
    map.insert(typ, lst[i].accountId());

    if (typ == MyMoneyAccount::Stock) {
      switch (lst[i].reconcileFlag()) {
        case MyMoneySplit::Cleared:
          strStatus =  QLatin1Char('C');
          break;

        case MyMoneySplit::Reconciled:
        case MyMoneySplit::Frozen:
          strStatus =  QLatin1Char('R');
          break;

        default:
          strStatus.clear();
          break;
      }
      strStatus += m_separator;
    }
  }
  //
  //  Add date.
  //
  QString str = QString("\n%1" + m_separator).arg(t.postDate().toString(Qt::ISODate));
  QString localeThousands = QLocale().groupSeparator();  //  In case of clash with field separator
  for (itSplit = lst.begin(); itSplit != lst.end(); ++itSplit) {
    MyMoneyAccount acc = file->account((*itSplit).accountId());
    //
    //  MyMoneyAccount::Checkings.
    //
    if ((acc.accountType() == MyMoneyAccount::Checkings) || (acc.accountType() == MyMoneyAccount::Cash) || (acc.accountType() == MyMoneyAccount::Savings)) {
      chkAccntId = (*itSplit).accountId();
      chkAccnt = file->account(chkAccntId).name();
      strCheckingAccountName = file->accountToCategory(chkAccntId) + m_separator;
      strAmount = (*itSplit).value().formatMoney("", 2).remove(localeThousands) + m_separator;
    } else if (acc.accountType() == MyMoneyAccount::Income) {
      //
      //  MyMoneyAccount::Income.
      //
      qty = (*itSplit).shares();
      value = (*itSplit).value();
      strInterest = value.formatMoney("", 2, false) + m_separator;
    } else if (acc.accountType() == MyMoneyAccount::Expense) {
      //
      //  MyMoneyAccount::Expense.
      //
      qty = (*itSplit).shares();
      value = (*itSplit).value();
      strFees = value.formatMoney("", 2, false) + m_separator;
    }  else if (acc.accountType() == MyMoneyAccount::Stock) {
      //
      //  MyMoneyAccount::Stock.
      //
      strMemo = QString("%1" + m_separator).arg((*itSplit).memo());
      strMemo.replace(QLatin1Char('\n'), QLatin1Char('~')).remove('\'');
      //
      //  Actions.
      //
      if ((*itSplit).action() == QLatin1String("Dividend")) {
        strAction = QLatin1String("DivX");
      } else if ((*itSplit).action() == QLatin1String("IntIncome")) {
        strAction = QLatin1String("IntIncX");
      }
      if ((strAction == QLatin1String("DivX")) || (strAction == QLatin1String("IntIncX"))) {
        if ((map.value(MyMoneyAccount::Checkings).isEmpty()) && (map.value(MyMoneyAccount::Cash).isEmpty())) {
          KMessageBox::sorry(0, i18n("Transaction number '%1' is missing an account assignment.\n"
                                     "Date '%2', Amount '%3'.\nTransaction dropped.\n", count, t.postDate().toString(Qt::ISODate), strAmount),
                             i18n("Invalid transaction"));
          return;
        }
      } else if ((*itSplit).action() == QLatin1String("Buy")) {
        qty = (*itSplit).shares();
        if (qty.isNegative()) {
          strAction = QLatin1String("Sell");
        } else {
          strAction = QLatin1String("Buy");
        }
      } else if ((*itSplit).action() == QLatin1String("Add")) {
        qty = (*itSplit).shares();
        if (qty.isNegative()) {
          strAction = QLatin1String("Shrsout");
        } else {
          strAction = QLatin1String("Shrsin");
        }
      } else if ((*itSplit).action() == QLatin1String("Reinvest")) {
        qty = (*itSplit).shares();
        strAmount = (*itSplit).value().formatMoney("", 2).remove(localeThousands) + m_separator;
        strAction = QLatin1String("ReinvDiv");
      } else {
        strAction = (*itSplit).action();
      }
      //
      //  Add action.
      //
      if ((strAction == QLatin1String("Buy")) || (strAction == QLatin1String("Sell")) || (strAction == QLatin1String("ReinvDiv"))) {
        //
        //  Add total.
        //
        if (strAction == QLatin1String("Sell")) {
          value = -value;
        }
        //
        //  Add price.
        //
        strPrice = (*itSplit).price().formatMoney("", 6, false);
        if (!qty.isZero()) {
          //
          //  Add quantity.
          //
          if (strAction == QLatin1String("Sell")) {
            qty = -qty;
          }
          strQuantity = qty.formatMoney("", 2, false);
        }
      } else if ((strAction == QLatin1String("Shrsin")) || (strAction == QLatin1String("Shrsout"))) {
        //
        //  Add quantity for "Shrsin" || "Shrsout".
        //
        if (strAction == QLatin1String("Shrsout")) {
          qty = -qty;
        }
        strQuantity = qty.formatMoney("", 2, false);
      }
      strAccName = acc.name();
      strAccName += m_separator;
      strAction += m_separator;
      strQuantity += m_separator;
      strPrice += m_separator;
    }
    if (strCheckingAccountName.isEmpty()) {
      strCheckingAccountName = m_separator;
    }
    if (strInterest.isEmpty()) {
      strInterest = m_separator;
    }
    if (strFees.isEmpty()) {
      strFees = m_separator;
    }
  }  //  end of itSplit loop
  str += strAccName + strAction + strAmount + strQuantity + strPrice + strInterest + strFees + strCheckingAccountName + strMemo + strStatus;
  QString date = t.postDate().toString(Qt::ISODate);
  m_map.insertMulti(date, str);
}
