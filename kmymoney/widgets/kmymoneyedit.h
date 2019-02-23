/*
 * Copyright 2000-2002  Michael Edwardes <mte@users.sourceforge.net>
 * Copyright 2002-2017  Thomas Baumgart <tbaumgart@kde.org>
 * Copyright 2017-2018  Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
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

#ifndef KMYMONEYEDIT_H
#define KMYMONEYEDIT_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QWidget>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "mymoneymoney.h"
#include "kmm_widgets_export.h"

class QWidget;
class KLineEdit;
class MyMoneySecurity;
class QValidator;

/**
  * This class represents a widget to enter monetary values.
  * It has an edit field and a button to select a popup
  * calculator. The result of the calculator (if used) is
  * stored in the edit field.
  *
  * @author Michael Edwardes, Thomas Baumgart
  */
class KMyMoneyEditPrivate;
class KMM_WIDGETS_EXPORT KMyMoneyEdit : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(KMyMoneyEdit)
  Q_PROPERTY(bool calculatorButtonVisibility READ isCalculatorButtonVisible WRITE setCalculatorButtonVisible)
  Q_PROPERTY(bool resetButtonVisibility READ isResetButtonVisible WRITE setResetButtonVisible)
  Q_PROPERTY(bool allowEmpty READ isEmptyAllowed WRITE setAllowEmpty)
  Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
  Q_PROPERTY(MyMoneyMoney value READ value WRITE setValue DESIGNABLE false STORED false USER true)
  Q_PROPERTY(bool valid READ isValid DESIGNABLE false STORED false)

public:
  explicit KMyMoneyEdit(QWidget* parent = nullptr, const int prec = -2);
  explicit KMyMoneyEdit(const MyMoneySecurity& eq, QWidget* parent = nullptr);
  ~KMyMoneyEdit();

  MyMoneyMoney value() const;

  void setValue(const MyMoneyMoney& value);

  bool isValid() const;

  virtual bool eventFilter(QObject *watched, QEvent *event) override;

  /**
    * This method returns the value of the edit field in "numerator/denominator" format.
    * If you want to get the text of the edit field, use lineedit()->text() instead.
    */
  QString text() const;
  void setMinimumWidth(int w);

  /**
    * Set the number of fractional digits that should be shown
    *
    * @param prec number of fractional digits.
    *
    * @note should be used prior to calling loadText()
    * @sa precision
    */
  void setPrecision(const int prec);

  /**
    * return the number of fractional digits
    * @sa setPrecision
    */
  int precision() const;

  QWidget* focusWidget() const;

  /**
    * This method allows to modify the behavior of the widget
    * such that it accepts an empty value (all blank) or not.
    * The default is to not accept an empty input and to
    * convert an empty field into 0.00 upon loss of focus.
    *
    * @param allowed if @a true, empty input is allowed, if @a false
    *                empty input will be converted to 0.00
    */
  void setAllowEmpty(bool allowed = true);

  /** Overloaded for internal reasons. The API is not affected. */
  void setValidator(const QValidator* v);

  bool isCalculatorButtonVisible() const;

  bool isResetButtonVisible() const;

  bool isEmptyAllowed() const;

  KLineEdit* lineedit() const;

  void setPlaceholderText(const QString& hint) const;

  bool isReadOnly() const;

  /**
   * This allows to setup the standard precision (number of decimal places)
   * to be used when no other information is available. @a prec must be in
   * the range of 0..19. If never set, the default precision is 2.
   */
  static void setStandardPrecision(int prec);

public Q_SLOTS:
  void loadText(const QString& text);
  void resetText();
  void clearText();

  void setText(const QString& txt);

  /**
    * This method allows to show/hide the calculator button of the widget.
    * The parameter @p show controls the behavior. Default is to show the
    * button.
    *
    * @param show if true, button is shown, if false it is hidden
    */
  void setCalculatorButtonVisible(const bool show);

  void setResetButtonVisible(const bool show);

  void setReadOnly(bool readOnly);

Q_SIGNALS: // Signals
  /**
    * This signal is sent, when the focus leaves this widget and
    * the amount has been changed by user during this session.
    */
  void valueChanged(const QString& text);

  void textChanged(const QString& text);

protected Q_SLOTS:
  void theTextChanged(const QString & text);
  void slotCalculatorResult();
  void slotCalculatorOpen();

private:
  KMyMoneyEditPrivate * const d_ptr;
  Q_DECLARE_PRIVATE(KMyMoneyEdit)
};

#endif
