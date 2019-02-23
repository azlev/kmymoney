/*
 * Copyright 2009-2015  Cristian Oneț <onet.cristian@gmail.com>
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

#ifndef MODELS_H
#define MODELS_H

#include <config-kmymoney.h>
#include "kmm_models_export.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QObject>
#include <QModelIndex>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

/**
  * Forward declarations for the returned models.
  */
class AccountsModel;
class InstitutionsModel;
#ifdef ENABLE_UNFINISHEDFEATURES
class LedgerModel;
#endif
class CostCenterModel;
class PayeesModel;
class EquitiesModel;
class SecuritiesModel;

/**
  * This object is the owner and maintainer of all the core models of KMyMoney.
  * It's a singleton so the instance should be accessed in the following way:
  *
  * @code
  * Models *models = Models::instance();
  * AccountsModel *accountsModel = models->accountsModel();
  * @endcode
  *
  * In order for the data synchronization between the @ref MyMoneyFile and the
  * models managed by this object to work, the @ref MyMoneyFile::dataChanged
  * signal must be connected to this object's @ref fileClosed slot.
  *
  * @author Cristian Onet 2010
  *
  */
class KMM_MODELS_EXPORT Models : public QObject
{
  Q_OBJECT

public:
  Models();
  ~Models();

  /**
    * This is the function to access the Models object.
    * It returns a pointer to the single instance of the object.
    */
  static Models* instance();

  AccountsModel* accountsModel();
  InstitutionsModel* institutionsModel();
#ifdef ENABLE_UNFINISHEDFEATURES
  LedgerModel* ledgerModel();
#endif
  CostCenterModel* costCenterModel();
  PayeesModel* payeesModel();
  EquitiesModel* equitiesModel();
  SecuritiesModel* securitiesModel();

  /**
   * returns the index of an item the @a model based on the @a id of role @a role.
   */
  static QModelIndex indexById(QAbstractItemModel* model, int role, const QString& id);

public Q_SLOTS:
  /**
    * This slot is used to notify the models that the data has been loaded and ready to use.
    * @ref MyMoneyFile.
    */
  void fileOpened();

  /**
    * This slot is used to notify the models that the data has been unloaded.
    * @ref MyMoneyFile.
    */
  void fileClosed();

Q_SIGNALS:
  void modelsLoaded();

private:

  /**
    * This class defines a singleton.
    */
  Models(const Models&);
  /**
    * This class defines a singleton.
    */
  Models& operator=(Models&);

private:
  struct Private;
  Private* const d;
};

#endif // MODELS_H

