/*
 * This file is part of KMyMoney, A Personal Finance Manager by KDE
 * Copyright (C) 2014-2015 Christian Dávid <christian-david@web.de>
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

#ifndef ONLINEJOBMODEL_H
#define ONLINEJOBMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class MyMoneyObject;

namespace eMyMoney { namespace File { enum class Object; } }

class onlineJobModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  /**
   * @brief Item Data roles for onlineJobs
   * In addition to Qt::ItemDataRole
   */
  enum roles {
    OnlineJobId = Qt::UserRole, /**< QString of onlineJob.id() */
    OnlineJobRole /**< the real onlineJob */
  };

  enum columns {
    ColAccount,
    ColAction,
    ColDestination,
    ColValue
  };

  /** Only @ref Models should be able to construct this class */
  explicit onlineJobModel(QObject *parent = nullptr);
  friend class Models;

  int rowCount(const QModelIndex & parent = QModelIndex()) const final override;
  int columnCount(const QModelIndex & parent = QModelIndex()) const final override;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const final override;
  QVariant headerData(int section, Qt::Orientation orientation , int role = Qt::DisplayRole) const final override;
  /** @brief Remove onlineJob identified by row */
  bool removeRow(int row, const QModelIndex & parent = QModelIndex());
  /** @brief Remove onlineJobs identified by row and count */
  bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) final override;

Q_SIGNALS:

public Q_SLOTS:
  void reloadAll();

  void slotObjectAdded(eMyMoney::File::Object objType, const QString &id);
  void slotObjectModified(eMyMoney::File::Object objType, const QString &id);
  void slotObjectRemoved(eMyMoney::File::Object objType, const QString& id);

  /** @brief Load data from MyMoneyFile */
  void load();
  void unload();

private:
  QStringList m_jobIdList;

};

#endif // ONLINEJOBMODEL_H
