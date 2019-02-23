/*
 * Copyright 2007-2011  Thomas Baumgart <tbaumgart@kde.org>
 * Copyright 2017       Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
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

#ifndef KBALANCECHARTDLG_H
#define KBALANCECHARTDLG_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QDialog>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes
namespace reports {
  class KReportChartView;
}
class MyMoneyAccount;

/**
 * @author Thomas Baumgart <ipwizard@users.sourceforge.net>
 *  This dialog displays a chart with the account balance for the last 90 days.
 *  It also draws the account limit if the account has any.
 */
class KBalanceChartDlg : public QDialog
{
  Q_OBJECT
public:
  explicit KBalanceChartDlg(const MyMoneyAccount& account, QWidget* parent = nullptr);
  ~KBalanceChartDlg();

protected:
  /**
    * Draw the chart and calculate and draw the account limits if any
    */
  reports::KReportChartView* drawChart(const MyMoneyAccount& account);

};

#endif
