/***************************************************************************
                          kmmviewinterface.h
                             -------------------
    begin                : Wed Jan 5 2005
    copyright            : (C) 2005 Thomas Baumgart
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

#ifndef KMMVIEWINTERFACE_H
#define KMMVIEWINTERFACE_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

class KMyMoneyView;

// ----------------------------------------------------------------------------
// Project Includes

#include "viewinterface.h"

namespace KMyMoneyPlugin
{

/**
  * This class represents the implementation of the
  * ViewInterface.
  */
class KMMViewInterface : public ViewInterface
{
  Q_OBJECT

public:
  KMMViewInterface(KMyMoneyView* view, QObject* parent, const char* name = 0);
  ~KMMViewInterface() {}

  /**
    * This method returns a pointer to a newly created view
    * with title @p item and icon @p pixmap.
    *
    * @param item Name of view
    * @param icon name for the icon to be used for the view
    *
    * @return pointer to KMyMoneyViewBase object
    */
//  KMyMoneyViewBase* addPage(const QString& item, const QString& icon);

  /**
    * This method allows to add a widget to the view
    * created with addPage()
    *
    * @param view pointer to view object
    * @param w pointer to widget
    */
//  void addWidget(KMyMoneyViewBase* view, QWidget* w);

  /**
    * Brings up a dialog to change the list(s) settings and saves them into the
    * class KMyMoneySettings (a singleton).
    *
    * @see KListSettingsDlg
    * Refreshes all views. Used e.g. after settings have been changed or
    * data has been loaded from external sources (QIF import).
    **/
  void slotRefreshViews() override;

  void addView(KMyMoneyViewBase* view, const QString& name, View idView) override;
  void removeView(View idView) override;
private:
  KMyMoneyView* m_view;
};

} // namespace
#endif
