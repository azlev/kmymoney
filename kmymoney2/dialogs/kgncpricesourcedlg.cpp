/***************************************************************************
                          kgncpricesourcedlg.cpp
                             -------------------
    copyright            : (C) 2005 by Tony Bloomfield <tonybloom@users.sourceforge.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// ----------------------------------------------------------------------------
// QT Includes
#include <QLabel>
#include <q3listbox.h>
#include <QLineEdit>
#include <QLayout>
#include <QApplication>
#include <QButtonGroup>

// ----------------------------------------------------------------------------
// KDE Includes
#include <kapplication.h>
#include <kurlrequester.h>
#include <ktextbrowser.h>
#include <klocale.h>
#include <ktoolinvocation.h>

// ----------------------------------------------------------------------------
// Project Includes
#include "kgncpricesourcedlg.h"
#include "webpricequote.h"

KGncPriceSourceDlg::KGncPriceSourceDlg(QWidget *parent){}

KGncPriceSourceDlg::KGncPriceSourceDlg(const QString &stockName, const QString &gncSource,QWidget * parent)
{
  setButtons(Ok | Help);
  m_widget = new KGncPriceSourceDlgDecl();
  setMainWidget(m_widget);
  // signals and slots connections
  connect( m_widget->buttonGroup5, SIGNAL(buttonClicked(int) ), this, SLOT( buttonPressed(int) ) );
  connect( this, SIGNAL(helpClicked()), this, SLOT( slotHelp() ) );
  // initialize data fields
  m_widget->textStockName->setText (i18n ("Investment: %1",stockName));
  m_widget->textGncSource->setText (i18n ("Quote source: %1",gncSource));
  m_widget->listKnownSource->insertStringList (WebPriceQuote::quoteSources());
  m_widget->lineUserSource->setText (gncSource);
  m_widget->checkAlwaysUse->setChecked(true);
  m_widget->buttonGroup5->setId(m_widget->buttonNoSource, 0);
  m_widget->buttonGroup5->setId(m_widget->buttonSelectSource, 1);
  m_widget->buttonGroup5->setId(m_widget->buttonUserSource, 2);
  m_widget->buttonGroup5->button(0)->setChecked(true);
  buttonPressed (0);
  return;
}

KGncPriceSourceDlg::~KGncPriceSourceDlg()
{
}

enum ButtonIds {NOSOURCE = 0, KMMSOURCE, USERSOURCE};

void KGncPriceSourceDlg::buttonPressed (int buttonId) {
  m_currentButton = buttonId;
  switch (m_currentButton) {
    case NOSOURCE:
      m_widget->listKnownSource->clearSelection();
      m_widget->listKnownSource->setEnabled (false);
      m_widget->lineUserSource->deselect();
      m_widget->lineUserSource->setEnabled (false);
      break;
    case KMMSOURCE:
      m_widget->lineUserSource->deselect ();
      m_widget->lineUserSource->setEnabled (false);
      m_widget->listKnownSource->setEnabled (true);
      m_widget->listKnownSource->setFocus();
      m_widget->listKnownSource->setSelected (0, true);
      break;
    case USERSOURCE:
      m_widget->listKnownSource->clearSelection();
      m_widget->listKnownSource->setEnabled (false);
      m_widget->lineUserSource->setEnabled (true);
      m_widget->lineUserSource->selectAll();
      m_widget->lineUserSource->setFocus ();
      break;
  }
}

QString KGncPriceSourceDlg::selectedSource() const {
  QString s;
  switch (m_currentButton) {
    case NOSOURCE: s = ""; break;
    case KMMSOURCE: s = m_widget->listKnownSource->currentText(); break;
    case USERSOURCE: s = m_widget->lineUserSource->text(); break;
  }
  return (s);
}

void KGncPriceSourceDlg::slotHelp(void)
{
  KToolInvocation::invokeHelp ("details.impexp.gncquotes");
}
