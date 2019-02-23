/*
 * Copyright 2016-2017  Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
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

#ifndef KCM_CSVIMPORTER_H
#define KCM_CSVIMPORTER_H

#include <KCModule>
#include <QWidget>
#include "ui_pluginsettingsdecl.h"

class PluginSettingsWidget : public QWidget, public Ui::PluginSettingsDecl
{
  Q_OBJECT
public:
  explicit PluginSettingsWidget(QWidget* parent = 0);
};

class KCMCSVImporter : public KCModule
{
public:
  explicit KCMCSVImporter(QWidget* parent, const QVariantList& args);
  ~KCMCSVImporter();
};

#endif // KCM_CSVIMPORT_H

