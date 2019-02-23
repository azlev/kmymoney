/***************************************************************************
                         previouspostponewizardpage.cpp  -  description
                            -------------------
   begin                : Sun Jul 18 2010
   copyright            : (C) 2010 by Fernando Vilas
   email                : kmymoney-devel@kde.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "previouspostponewizardpage.h"

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "ui_previouspostponewizardpage.h"

PreviousPostponeWizardPage::PreviousPostponeWizardPage(QWidget *parent) :
  QWizardPage(parent),
  ui(new Ui::PreviousPostponeWizardPage)
{
  ui->setupUi(this);
  // Register the fields with the QWizard and connect the
  // appropriate signals to update the "Next" button correctly

}

PreviousPostponeWizardPage::~PreviousPostponeWizardPage()
{
  delete ui;
}
