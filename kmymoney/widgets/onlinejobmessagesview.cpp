/*
 * Copyright 2015       Christian Dávid <christian-david@web.de>
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

#include "onlinejobmessagesview.h"

#include "ui_onlinejobmessagesview.h"

onlineJobMessagesView::onlineJobMessagesView(QWidget* parent)
    : QWidget(parent),
    ui(new Ui::onlineJobMessageView)
{
  ui->setupUi(this);
  connect(ui->closeButton, &QAbstractButton::pressed, this, &QWidget::close);
}

void onlineJobMessagesView::setModel(QAbstractItemModel* model)
{
  ui->tableView->setModel(model);
  // Enlarge the description column
  ui->tableView->setColumnWidth(2, 4*ui->tableView->columnWidth(2));
}

onlineJobMessagesView::~onlineJobMessagesView()
{
  delete ui;
}
