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

#include "kcm_csvimporter.h"
#include <config-kmymoney-version.h>

// KDE includes
#include <KPluginFactory>
#include <KAboutData>
#include "pluginsettings.h"

PluginSettingsWidget::PluginSettingsWidget(QWidget* parent) :
    QWidget(parent)
{
  setupUi(this);
}

KCMCSVImporter::KCMCSVImporter(QWidget *parent, const QVariantList& args)
  : KCModule(parent, args)
{
  PluginSettingsWidget* w = new PluginSettingsWidget(this);
  addConfig(PluginSettings::self(), w);
  QVBoxLayout *layout = new QVBoxLayout;
  setLayout(layout);
  layout->addWidget(w);
  setButtons(NoAdditionalButton);
  load();
}

KCMCSVImporter::~KCMCSVImporter()
{
}

K_PLUGIN_FACTORY_WITH_JSON(KCMCSVImporterFactory, "kcm_csvimporter.json", registerPlugin<KCMCSVImporter>();)

#include "kcm_csvimporter.moc"
