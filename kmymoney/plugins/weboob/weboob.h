/*
 * This file is part of KMyMoney, A Personal Finance Manager by KDE
 * Copyright (C) 2014-2015 Romain Bignon <romain@symlink.me>
 * Copyright (C) 2014-2015 Florent Fourcot <weboob@flo.fourcot.fr>
 * (C) 2017 by Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WEBOOB_H
#define WEBOOB_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "kmymoneyplugin.h"

class MyMoneyAccount;
class MyMoneyKeyValueContainer;

class WeboobPrivate;
class Weboob : public KMyMoneyPlugin::Plugin, public KMyMoneyPlugin::OnlinePlugin
{
  Q_OBJECT
  Q_INTERFACES(KMyMoneyPlugin::OnlinePlugin)

public:
  explicit Weboob(QObject *parent, const QVariantList &args);
  ~Weboob() override;

  void plug() override;
  void unplug() override;

  void protocols(QStringList& protocolList) const override;

  QWidget* accountConfigTab(const MyMoneyAccount& account, QString& tabName) override;

  MyMoneyKeyValueContainer onlineBankingSettings(const MyMoneyKeyValueContainer& current) override;

  bool mapAccount(const MyMoneyAccount& acc, MyMoneyKeyValueContainer& onlineBankingSettings) override;

  bool updateAccount(const MyMoneyAccount& acc, bool moreAccounts = false) override;

private:
  Q_DECLARE_PRIVATE(Weboob)
  WeboobPrivate * const d_ptr;

private Q_SLOTS:
  void gotAccount();
};

#endif
