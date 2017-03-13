/***************************************************************************
                          ledgerdelegate.cpp
                             -------------------
    begin                : Sat Aug 8 2015
    copyright            : (C) 2015 by Thomas Baumgart
    email                : Thomas Baumgart <tbaumgart@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ledgerdelegate.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QApplication>
#include <QScrollBar>
#include <QPainter>
#include <QDebug>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "ledgerview.h"
#include "models.h"
#include "accountsmodel.h"
#include "ledgermodel.h"
#include "kmymoneyaccountcombo.h"       // for definition of AccountNamesFilterProxyModel()
#include "newtransactioneditor.h"

static unsigned char attentionSign[] = {
  0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
  0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
  0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14,
  0x08, 0x06, 0x00, 0x00, 0x00, 0x8D, 0x89, 0x1D,
  0x0D, 0x00, 0x00, 0x00, 0x04, 0x73, 0x42, 0x49,
  0x54, 0x08, 0x08, 0x08, 0x08, 0x7C, 0x08, 0x64,
  0x88, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58,
  0x74, 0x53, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72,
  0x65, 0x00, 0x77, 0x77, 0x77, 0x2E, 0x69, 0x6E,
  0x6B, 0x73, 0x63, 0x61, 0x70, 0x65, 0x2E, 0x6F,
  0x72, 0x67, 0x9B, 0xEE, 0x3C, 0x1A, 0x00, 0x00,
  0x02, 0x05, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8D,
  0xAD, 0x95, 0xBF, 0x4B, 0x5B, 0x51, 0x14, 0xC7,
  0x3F, 0x2F, 0xBC, 0x97, 0x97, 0x97, 0x97, 0x77,
  0xF3, 0xF2, 0x1C, 0xA4, 0x54, 0x6B, 0x70, 0x10,
  0x44, 0x70, 0x2A, 0x91, 0x2E, 0x52, 0x02, 0x55,
  0x8A, 0xB5, 0xA3, 0xAB, 0x38, 0x08, 0x66, 0xCC,
  0xEE, 0xE0, 0xE2, 0x20, 0xB8, 0x38, 0xB8, 0xB8,
  0xF8, 0x1F, 0x38, 0x29, 0xA5, 0x29, 0x74, 0x90,
  0x0E, 0x0D, 0x0E, 0x22, 0x1D, 0x44, 0xA8, 0xD0,
  0xD4, 0xB4, 0x58, 0x4B, 0x09, 0xF9, 0xF1, 0x4A,
  0x3B, 0xD4, 0xD3, 0xE1, 0x55, 0xD3, 0x34, 0xAF,
  0x49, 0x6C, 0x3D, 0xF0, 0x85, 0x7B, 0xCF, 0xFD,
  0x9E, 0xEF, 0x3D, 0xE7, 0xFE, 0xD4, 0x44, 0x84,
  0xDB, 0xB4, 0x48, 0x2F, 0xA4, 0x94, 0xAB, 0xE5,
  0x52, 0xAE, 0x96, 0xEB, 0x49, 0x51, 0x44, 0x3A,
  0x02, 0x18, 0x88, 0xC7, 0xF1, 0xE3, 0x71, 0x7C,
  0x60, 0xA0, 0x1B, 0xBF, 0x6B, 0x86, 0x49, 0xC5,
  0x46, 0x3E, 0x47, 0x34, 0x9F, 0x23, 0x9A, 0x54,
  0x6C, 0xFC, 0x57, 0x86, 0x40, 0xC6, 0x4B, 0xE1,
  0x37, 0xCA, 0x48, 0xA3, 0x8C, 0x78, 0x29, 0x7C,
  0x20, 0xD3, 0x31, 0xA6, 0xD3, 0xA0, 0x52, 0x1C,
  0x6D, 0x6F, 0x72, 0xD9, 0x28, 0x23, 0xFE, 0x07,
  0x64, 0x7B, 0x93, 0x4B, 0xA5, 0x38, 0xFA, 0x27,
  0x41, 0x60, 0x6E, 0x74, 0x84, 0x7A, 0xE5, 0x1D,
  0x92, 0x54, 0x88, 0xE7, 0x22, 0xD5, 0x12, 0x32,
  0x3A, 0x42, 0x1D, 0x98, 0xBB, 0x91, 0x20, 0x60,
  0xDA, 0x36, 0x17, 0xFB, 0x7B, 0xC8, 0xC1, 0x4B,
  0x04, 0x02, 0xBC, 0x7E, 0x81, 0xEC, 0xEF, 0x21,
  0xB6, 0xCD, 0x05, 0x60, 0xF6, 0x2C, 0x68, 0x9A,
  0x2C, 0xCF, 0x4C, 0xE1, 0x4B, 0x05, 0x39, 0x3F,
  0x69, 0x0A, 0xBE, 0x7F, 0x83, 0x48, 0x05, 0x99,
  0x99, 0xC2, 0x37, 0x4D, 0x96, 0x7B, 0x12, 0x04,
  0xFA, 0x2D, 0x8B, 0xC6, 0xE9, 0x61, 0x10, 0x2C,
  0x15, 0xC4, 0x8A, 0x21, 0x86, 0x8E, 0xFC, 0xF8,
  0x12, 0xF4, 0x4F, 0x0F, 0x11, 0xCB, 0xA2, 0x01,
  0xF4, 0x77, 0x3D, 0x36, 0x4E, 0x82, 0xF5, 0xA5,
  0x05, 0x8C, 0xE1, 0x74, 0xD3, 0x37, 0x34, 0x18,
  0x20, 0xF2, 0x8B, 0x3D, 0x9C, 0x86, 0xA5, 0x05,
  0x0C, 0x27, 0xC1, 0x7A, 0xC7, 0x63, 0x03, 0x8C,
  0x2B, 0x07, 0xBF, 0x5A, 0x6A, 0x66, 0x27, 0x15,
  0x64, 0x3A, 0x8B, 0x3C, 0x7A, 0xD8, 0xEA, 0xAB,
  0x96, 0x10, 0xE5, 0xE0, 0x03, 0xE3, 0x7F, 0xCD,
  0x50, 0x39, 0x6C, 0xAD, 0xAD, 0x10, 0x53, 0xAA,
  0x75, 0xD2, 0xF4, 0xBD, 0x00, 0x2D, 0x5C, 0x05,
  0x6B, 0x2B, 0xC4, 0x94, 0xC3, 0xD6, 0xEF, 0xFE,
  0x6B, 0x41, 0x4D, 0xD3, 0x66, 0xFB, 0x3C, 0xC6,
  0x16, 0xE7, 0xDB, 0x97, 0x61, 0xE2, 0x3E, 0x3C,
  0xC8, 0xB4, 0x15, 0xC7, 0xE2, 0x3C, 0x91, 0x3E,
  0x8F, 0x31, 0x4D, 0xD3, 0x66, 0x5B, 0x4A, 0x06,
  0x8C, 0x84, 0xCD, 0x59, 0x61, 0xA7, 0xB5, 0xAC,
  0x2B, 0x9C, 0x1C, 0x04, 0x08, 0x1B, 0x2B, 0xEC,
  0x20, 0x09, 0x9B, 0x33, 0xC0, 0xB8, 0xDE, 0x65,
  0x43, 0x27, 0x9F, 0x9D, 0xA4, 0x1E, 0x16, 0xF0,
  0xF9, 0x6D, 0xB0, 0xC3, 0x86, 0x1E, 0xB4, 0xC3,
  0x38, 0xD9, 0x49, 0xEA, 0x86, 0x4E, 0xFE, 0xEA,
  0x29, 0xF4, 0x2C, 0x8B, 0xDA, 0x71, 0x31, 0x9C,
  0xFC, 0xF5, 0x23, 0x32, 0x34, 0x88, 0xDC, 0xBD,
  0x13, 0x5C, 0xBF, 0x30, 0xCE, 0x71, 0x11, 0xB1,
  0x2C, 0x6A, 0x80, 0xA7, 0xDB, 0x36, 0xAB, 0x4F,
  0xA6, 0x89, 0xBA, 0x49, 0x38, 0xFF, 0xD4, 0xBE,
  0x4E, 0x00, 0xAF, 0x9E, 0x81, 0x08, 0xD4, 0xEA,
  0x01, 0xFE, 0x34, 0x37, 0x09, 0x4F, 0x1F, 0x13,
  0xDD, 0x7D, 0xCE, 0xAA, 0x96, 0x72, 0x29, 0x7C,
  0xFB, 0xCE, 0x44, 0xB8, 0xD4, 0xCD, 0x2C, 0x66,
  0x52, 0xD4, 0x6E, 0xFB, 0x0B, 0xF8, 0x09, 0x63,
  0x63, 0x31, 0xE4, 0x85, 0x76, 0x2E, 0x0E, 0x00,
  0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE,
  0x42, 0x60, 0x82
};

QColor LedgerDelegate::m_erroneousColor = QColor(Qt::red);
QColor LedgerDelegate::m_importedColor = QColor(Qt::yellow);

class LedgerDelegate::Private
{
public:
  Private()
  : m_editor(0)
  , m_editorRow(-1)
  {}

  NewTransactionEditor*         m_editor;
  int                           m_editorRow;
};


LedgerDelegate::LedgerDelegate(QObject* parent)
  : QStyledItemDelegate(parent)
  , d(new Private)
{
}

LedgerDelegate::~LedgerDelegate()
{
  delete d;
}

void LedgerDelegate::setErroneousColor(const QColor& color)
{
  m_erroneousColor = color;
}

QWidget* LedgerDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  Q_UNUSED(option);

  if(index.isValid()) {
    Q_ASSERT(parent);
    LedgerView* view = qobject_cast<LedgerView*>(parent->parentWidget());
    Q_ASSERT(view != 0);

    if(view->selectionModel()->selectedRows().count() > 1) {
      qCDebug(LOG_KMYMONEY) << "Editing multiple transactions at once is not yet supported";

      /**
       * @todo replace the following three lines with the creation of a special
       * editor that can handle multiple transactions at once
       */
      d->m_editor = 0;
      LedgerDelegate* const that = const_cast<LedgerDelegate* const>(this);
      emit that->closeEditor(d->m_editor, NoHint);

    } else {
      d->m_editor = new NewTransactionEditor(parent, view->accountId());
    }

    if(d->m_editor) {
      d->m_editorRow = index.row();
      connect(d->m_editor, SIGNAL(done()), this, SLOT(endEdit()));
      emit sizeHintChanged(index);
    }

  } else {
    qFatal("LedgerDelegate::createEditor(): we should never end up here");
  }
  return d->m_editor;
}

int LedgerDelegate::editorRow() const
{
  return d->m_editorRow;
}

void LedgerDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);

  // never change the background of the cell the mouse is hovering over
  opt.state &= ~QStyle::State_MouseOver;

  // show the focus only on the detail column
  opt.state &= ~QStyle::State_HasFocus;
  if(index.column() == LedgerModel::DetailColumn) {
    QAbstractItemView* view = qobject_cast< QAbstractItemView* >(parent());
    if(view) {
      if(view->currentIndex().row() == index.row()) {
        opt.state |= QStyle::State_HasFocus;
      }
    }
  }

  painter->save();

  // Background
  QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

  // Do not paint text if the edit widget is shown
  const LedgerView *view = qobject_cast<const LedgerView *>(opt.widget);
  if (view && view->indexWidget(index)) {
    painter->restore();
    return;
  }

  const int margin = style->pixelMetric(QStyle::PM_FocusFrameHMargin);
  const QRect textArea = QRect(opt.rect.x() + margin, opt.rect.y() + margin, opt.rect.width() - 2 * margin, opt.rect.height() - 2 * margin);
  const bool selected = opt.state & QStyle::State_Selected;

  QStringList lines;
  if(index.column() == LedgerModel::DetailColumn) {
    lines << index.model()->data(index, LedgerRole::PayeeNameRole).toString();
    if(selected) {
      lines << index.model()->data(index, LedgerRole::CounterAccountRole).toString();
      lines << index.model()->data(index, LedgerRole::SingleLineMemoRole).toString();

    } else {
      if(lines.at(0).isEmpty()) {
        lines.clear();
        lines << index.model()->data(index, LedgerRole::SingleLineMemoRole).toString();
      }
      if(lines.at(0).isEmpty()) {
        lines << index.model()->data(index, LedgerRole::CounterAccountRole).toString();
      }
    }
    lines.removeAll(QString());
  }

  const bool erroneous = index.model()->data(index, LedgerRole::ErroneousRole).toBool();

  // draw the text items
  if(!opt.text.isEmpty() || !lines.isEmpty()) {

    // check if it is a scheduled transaction and display it as inactive
    if(!index.model()->data(index, LedgerRole::ScheduleIdRole).toString().isEmpty()) {
      opt.state &= ~QStyle::State_Enabled;
    }

    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled)
                              ? QPalette::Normal : QPalette::Disabled;

    if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active)) {
      cg = QPalette::Inactive;
    }
    if (opt.state & QStyle::State_Selected) {
      painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
    } else {
      painter->setPen(opt.palette.color(cg, QPalette::Text));
    }
    if (opt.state & QStyle::State_Editing) {
      painter->setPen(opt.palette.color(cg, QPalette::Text));
      painter->drawRect(textArea.adjusted(0, 0, -1, -1));
    }

    // Don't play with the color if it's selected
    // otherwise switch the color if the transaction has errors
    if(erroneous && !selected) {
      painter->setPen(m_erroneousColor);
    }

    // collect data for the various colums
    if(index.column() == LedgerModel::DetailColumn) {
      for(int i = 0; i < lines.count(); ++i) {
        painter->drawText(textArea.adjusted(0, (opt.fontMetrics.lineSpacing() + 2) * i, 0, 0), opt.displayAlignment, lines[i]);
      }

    } else {
      painter->drawText(textArea, opt.displayAlignment, opt.text);
    }
  }

  // draw the focus rect
  if(opt.state & QStyle::State_HasFocus) {
    QStyleOptionFocusRect o;
    o.QStyleOption::operator=(opt);
    o.rect = style->proxy()->subElementRect(QStyle::SE_ItemViewItemFocusRect, &opt, opt.widget);
    o.state |= QStyle::State_KeyboardFocusChange;
    o.state |= QStyle::State_Item;

    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled)
                              ? QPalette::Normal : QPalette::Disabled;
    o.backgroundColor = opt.palette.color(cg, (opt.state & QStyle::State_Selected)
                                             ? QPalette::Highlight : QPalette::Window);
    style->proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, opt.widget);
  }

  if((index.column() == LedgerModel::DetailColumn)
  && erroneous) {
    QPixmap attention;
    attention.loadFromData(attentionSign, sizeof(attentionSign), 0, 0);
    style->proxy()->drawItemPixmap(painter, option.rect, Qt::AlignRight | Qt::AlignTop, attention);
  }

  painter->restore();
#if 0
  const QHeaderView* horizontalHeader = view->horizontalHeader();
  const QHeaderView* verticalHeader = view->verticalHeader();
  const QWidget* viewport = view->viewport();
  const bool showGrid = view->showGrid() && !view->indexWidget(index);
  const int gridSize = showGrid ? 1 : 0;
  const int gridHint = style->styleHint(QStyle::SH_Table_GridLineColor, &option, view);
  const QColor gridColor = static_cast<QRgb>(gridHint);
  const QPen gridPen = QPen(gridColor, 0, view->gridStyle());
  const bool rightToLeft = view->isRightToLeft();
  const int viewportOffset = horizontalHeader->offset();


  // QStyledItemDelegate::paint(painter, opt, index);

  if(!horizontalHeader->isSectionHidden(LedgerModel::DateColumn)) {
    QDate postDate = index.data(LedgerModel::PostDateRole).toDate();
    if(postDate.isValid()) {
      int ofs = horizontalHeader->sectionViewportPosition(LedgerModel::DateColumn) + viewportOffset;
      QRect oRect = opt.rect;
      opt.displayAlignment = Qt::AlignLeft | Qt::AlignTop;
      opt.rect.setLeft(opt.rect.left()+ofs);
      opt.rect.setTop(opt.rect.top()+margin);
      opt.rect.setWidth(horizontalHeader->sectionSize(LedgerModel::DateColumn));
      opt.text = KGlobal::locale()->formatDate(postDate, QLocale::ShortFormat);
      style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
      opt.rect = oRect;
    }
  }

  if(!horizontalHeader->isSectionHidden(LedgerModel::DetailColumn)) {
    QString payee = index.data(LedgerModel::PayeeRole).toString();
    QString counterAccount = index.data(LedgerModel::CounterAccountRole).toString();
    QString txt = payee;
    if(payee.length() > 0)
      txt += '\n';
    txt += counterAccount;
    int ofs = horizontalHeader->sectionViewportPosition(LedgerModel::DetailColumn) + viewportOffset;
    QRect oRect = opt.rect;
    opt.displayAlignment = Qt::AlignLeft | Qt::AlignTop;
    opt.rect.setLeft(opt.rect.left()+ofs);
    opt.rect.setTop(opt.rect.top()+margin);
    opt.rect.setWidth(horizontalHeader->sectionSize(LedgerModel::DetailColumn));
    opt.text = txt;
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
    opt.rect = oRect;

  }
#if 0
  opt.features |= QStyleOptionViewItemV2::HasDisplay;
  QString txt = QString("%1").arg(index.isValid() ? "true" : "false");
  if(index.isValid())
    txt += QString(" %1 - %2").arg(index.row()).arg(view->verticalHeader()->sectionViewportPosition(index.row()));
  opt.text = displayText(txt, opt.locale);

  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
#endif

  // paint grid
  if(showGrid) {
    painter->save();
    QPen old = painter->pen();
    painter->setPen(gridPen);

    // qCDebug(LOG_KMYMONEY) << "Paint grid for" << index.row() << "in" << opt.rect;
    for(int i=0; i < horizontalHeader->count(); ++i) {
      if(!horizontalHeader->isSectionHidden(i)) {
        int ofs = horizontalHeader->sectionViewportPosition(i) + viewportOffset;
        if(!rightToLeft) {
          ofs += horizontalHeader->sectionSize(i) - gridSize;
        }
        if(ofs-viewportOffset < viewport->width()) {
          // I have no idea, why I need to paint the grid for the selected row and the one below
          // but it was the only way to get this working correctly. Otherwise the grid was missing
          // while moving the mouse over the view from bottom to top.
          painter->drawLine(opt.rect.x()+ofs, opt.rect.y(), opt.rect.x()+ofs, opt.rect.height());
          painter->drawLine(opt.rect.x()+ofs, opt.rect.y()+verticalHeader->sectionSize(index.row()), opt.rect.x()+ofs, opt.rect.height());
        }
      }
    }
    painter->setPen(old);
    painter->restore();
  }
#endif
}

QSize LedgerDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  bool fullDisplay = false;
  LedgerView* view = qobject_cast<LedgerView*>(parent());
  if(view) {
    QModelIndex currentIndex = view->currentIndex();
    if(currentIndex.isValid()) {
      QString currentId = currentIndex.model()->data(currentIndex, LedgerRole::TransactionSplitIdRole).toString();
      QString myId = index.model()->data(index, LedgerRole::TransactionSplitIdRole).toString();
      fullDisplay = (currentId == myId);
    }
  }

  QSize size;
  QStyleOptionViewItem opt = option;
  if(index.isValid()) {
    // check if we are showing the edit widget
    const QAbstractItemView *view = qobject_cast<const QAbstractItemView *>(opt.widget);
    if (view) {
      QModelIndex editIndex = view->model()->index(index.row(), 0);
      if(editIndex.isValid()) {
        QWidget* editor = view->indexWidget(editIndex);
        if(editor) {
          size = editor->minimumSizeHint();
          return size;
        }
      }
    }
  }

  int rows = 1;
  initStyleOption(&opt, index);
  size = QSize(100, (opt.fontMetrics.lineSpacing()));
  if(fullDisplay) {
    QString payee = index.data(LedgerRole::PayeeNameRole).toString();
    QString counterAccount = index.data(LedgerRole::CounterAccountRole).toString();
    QString memo = index.data(LedgerRole::SingleLineMemoRole).toString();

    rows = (payee.length() > 0 ? 1 : 0) + (counterAccount.length() > 0 ? 1 : 0) + (memo.length() > 0 ? 1 : 0);
    // make sure we show at least one row
    if(!rows) {
      rows = 1;
    }
    // leave a few pixels as margin for each row
    size.setHeight((size.height() + 5) * rows);
  }

  return size;
}

void LedgerDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  Q_UNUSED(index);
  QStyleOptionViewItem opt = option;
  int ofs = 8;
  const LedgerView* view = qobject_cast<const LedgerView*>(opt.widget);
  if(view) {
    if(view->verticalScrollBar()->isVisible()) {
      ofs += view->verticalScrollBar()->width();
    }
  }

  QRect r(opt.rect);
  r.setWidth(opt.widget->width() - ofs);
  editor->setGeometry(r);
  editor->update();
  // int flags = editor->windowFlags();
  // qCDebug(LOG_KMYMONEY) << "updateEditorGeometry" << r << QString("%1").arg(flags, 8, 16, QChar('0'));
}

void LedgerDelegate::endEdit()
{
  if(d->m_editor) {
    if(d->m_editor->accepted()) {
      emit commitData(d->m_editor);
    }
    emit closeEditor(d->m_editor, NoHint);
    d->m_editorRow = -1;
  }
}

/**
 * This eventfilter seems to do nothing but it prevents that selecting a
 * different row with the mouse closes the editor
 */
bool LedgerDelegate::eventFilter(QObject* o, QEvent* event)
{
  return QAbstractItemDelegate::eventFilter(o, event);
}

void LedgerDelegate::setEditorData(QWidget* editWidget, const QModelIndex& index) const
{
  NewTransactionEditor* editor = qobject_cast<NewTransactionEditor*>(editWidget);
  if(editor) {
    editor->loadTransaction(index.model()->data(index, LedgerRole::TransactionSplitIdRole).toString());
  }
}

void LedgerDelegate::setModelData(QWidget* editWidget, QAbstractItemModel* model, const QModelIndex& index) const
{
  Q_UNUSED(model)
  Q_UNUSED(index)

  NewTransactionEditor* editor = qobject_cast<NewTransactionEditor*>(editWidget);
  if(editor) {
    editor->saveTransaction();
  }
}
