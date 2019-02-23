/***************************************************************************
                         effectivedatewizardpage  -  description
                            -------------------
   begin                : Sun Jul 4 2010
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

#ifndef EFFECTIVEDATEWIZARDPAGE_H
#define EFFECTIVEDATEWIZARDPAGE_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QWizardPage>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

namespace Ui { class EffectiveDateWizardPage; }

/**
 * This class implements the Effective Date page of the
 * @ref KNewLoanWizard.
 */

class EffectiveDateWizardPage : public QWizardPage
{
  Q_OBJECT
public:
  explicit EffectiveDateWizardPage(QWidget *parent = nullptr);
  ~EffectiveDateWizardPage();

  /**
   * Overload the isComplete function to control the Next button
   */
  bool isComplete() const final override;

  /**
   * Overload the initializePage function to set widgets based on
   * the inputs from previous pages.
   */
  void initializePage() final override;

  Ui::EffectiveDateWizardPage *ui;
};

#endif
