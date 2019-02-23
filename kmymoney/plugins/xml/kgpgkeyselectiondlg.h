/***************************************************************************
                          kgpgkeyselectiondlg.h
                             -------------------
    copyright            : (C) 2008 by Thomas Baumgart
    email                : ipwizard@users.sourceforge.net
                           (C) 2017 by Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KGPGKEYSELECTIONDLG_H
#define KGPGKEYSELECTIONDLG_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QDialog>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

/**
  * @author Thomas Baumgart
  */
class KGpgKeySelectionDlgPrivate;
class KGpgKeySelectionDlg : public QDialog
{
  Q_OBJECT
  Q_DISABLE_COPY(KGpgKeySelectionDlg)

public:

  explicit KGpgKeySelectionDlg(QWidget* parent = nullptr);
  ~KGpgKeySelectionDlg();

  /**
   * preset the key selector with the keys contained in @a keyList.
   * The key contained in @a defaultKey is made the current selection.
   */
  void setSecretKeys(const QStringList& keyList, const QString& defaultKey);

  /**
   * preset the additional key list with the given key ids in @a list
   */
  void setAdditionalKeys(const QStringList& list);

  /**
   * Returns the selected secret key. In case "No encryption" is selected,
   * the string is empty.
   */
  QString secretKey() const;

  /**
   * Returns the list of keys currently listed in the KEditListWidget
   */
  QStringList additionalKeys() const;

protected Q_SLOTS:
  void slotIdChanged();
  void slotKeyListChanged();

private:
  KGpgKeySelectionDlgPrivate * const d_ptr;
  Q_DECLARE_PRIVATE(KGpgKeySelectionDlg)
};

#endif
