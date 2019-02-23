/*
 * Copyright 2014-2016  Christian Dávid <christian-david@web.de>
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

#ifndef KSETTINGSKMYMONEY_H
#define KSETTINGSKMYMONEY_H

#include <KConfigDialog>

/**
 * @brief The general settings dialog
 */
class KSettingsKMyMoney : public KConfigDialog
{
public:
  explicit KSettingsKMyMoney(QWidget *parent, const QString &name, KCoreConfigSkeleton *config);

Q_SIGNALS:
  void pluginsChanged();

private Q_SLOTS:
  void slotPluginsChanged(bool changed);
};


#endif /* KSETTINGSKMYMONEY_H */
