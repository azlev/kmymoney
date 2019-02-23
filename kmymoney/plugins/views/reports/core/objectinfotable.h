/*
 * Copyright 2004-2005  Ace Jones <acejones@users.sourceforge.net>
 * Copyright 2008-2010  Alvaro Soliverez <asoliverez@gmail.com>
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

#ifndef OBJECTINFOTABLE_H
#define OBJECTINFOTABLE_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "listtable.h"

class MyMoneyReport;

namespace reports
{

class ReportAccount;

/**
  * Calculates a query of information about the transaction database.
  *
  * This is a middle-layer class, between the UI and the engine.  The
  * MyMoneyReport class holds only the CONFIGURATION parameters.  This
  * class actually does the work of retrieving the data from the engine
  * and formatting it for the user.
  *
  * @author Ace Jones
  *
  * @short
**/

class ObjectInfoTable : public ListTable
{
public:
  explicit ObjectInfoTable(const MyMoneyReport&);
  void init();

protected:
  void constructScheduleTable();
  void constructAccountTable();
  void constructAccountLoanTable();

  bool linkEntries() const final override { return false; }

private:
  /**
    * @param acc the investment account
    * @return the balance in the currency of the investment account
    */
  MyMoneyMoney investmentBalance(const MyMoneyAccount& acc);
};

}

#endif // QUERYREPORT_H
