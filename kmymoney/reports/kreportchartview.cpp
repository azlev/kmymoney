/***************************************************************************
                          kreportchartview.cpp
                             -------------------
    begin                : Sun Aug 14 2005
    copyright            : (C) 2004-2005 by Ace Jones
    email                : <ace.j@hotpop.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kreportchartview.h"

// ----------------------------------------------------------------------------
// QT Includes
#include <QMouseEvent>
#include <QLabel>
#include <QFrame>
#include <QStandardItemModel>

// ----------------------------------------------------------------------------
// KDE Includes
#include <kcolorscheme.h>
#include <klocale.h>
#include <kglobalsettings.h>

// ----------------------------------------------------------------------------
// Project Includes
#include <KDChartBackgroundAttributes>
#include <KDChartDataValueAttributes>
#include <KDChartMarkerAttributes>
#include <KDChartGridAttributes>
#include <KDChartHeaderFooter>
#include <KDChartLegend>
#include <KDChartLineDiagram>
#include <KDChartBarDiagram>
#include <KDChartPieDiagram>
#include <KDChartRingDiagram>
#include <KDChartCartesianAxis>
#include <KDChartFrameAttributes>
#include "kmymoneyglobalsettings.h"

using namespace reports;

KReportChartView::KReportChartView(QWidget* parent): KDChart::Chart(parent)
{
  // ********************************************************************
  // Set KMyMoney's Chart Parameter Defaults
  // ********************************************************************

  //Set the background to white
  BackgroundAttributes backAttr = backgroundAttributes();
  KColorScheme colorScheme(QPalette::Normal);
  backAttr.setBrush(colorScheme.background());
  backAttr.setVisible(true);
  setBackgroundAttributes(backAttr);

  //Line diagram
  KDChart::LineDiagram* diagram = new KDChart::LineDiagram;
  diagram->setModel(&m_model);
  this->coordinatePlane()->replaceDiagram(diagram);
}

void KReportChartView::drawPivotChart(const PivotGrid &grid, const MyMoneyReport &config, int numberColumns, const QStringList& columnHeadings, const QList<ERowType>& rowTypeList, const QStringList& columnTypeHeaderList)
{
  ::timetrace("start drawing chart");
  //set the number of columns
  setNumColumns(numberColumns);

  //remove existing headers
  while (headerFooters().count() > 0) {
    HeaderFooter* delHeader = headerFooters().at(0);
    takeHeaderFooter(delHeader);
    delete delHeader;
  }

  //make sure the model is clear
  m_model.removeColumns(0, m_model.columnCount());
  m_model.removeRows(0, m_model.rowCount());

  //set the new header
  HeaderFooter* header = new HeaderFooter(this);
  header->setText(config.name());
  header->setType(HeaderFooter::Header);
  header->setPosition(Position::North);
  addHeaderFooter(header);

  // whether to limit the chart to use series totals only.  Used for reports which only
  // show one dimension (pie).
  setSeriesTotals(false);

  // whether series (rows) are accounts (true) or months (false). This causes a lot
  // of complexity in the charts.  The problem is that circular reports work best with
  // an account in a COLUMN, while line/bar prefer it in a ROW.
  setAccountSeries(true);

  //what values should be shown
  bool showBudget = config.hasBudget();
  bool showForecast = config.isIncludingForecast();
  bool showActual = false;
  if ((config.isIncludingBudgetActuals()) || (!showBudget && !showForecast))
    showActual = true;

  switch (config.chartType()) {
  case MyMoneyReport::eChartNone:
  case MyMoneyReport::eChartEnd:
  case MyMoneyReport::eChartLine: {
    KDChart::LineDiagram* diagram = new KDChart::LineDiagram;
    CartesianCoordinatePlane* cartesianPlane = new CartesianCoordinatePlane;
    replaceCoordinatePlane(cartesianPlane);
    coordinatePlane()->replaceDiagram(diagram);
    break;
  }
  case MyMoneyReport::eChartBar: {
    KDChart::BarDiagram* diagram = new KDChart::BarDiagram;
    CartesianCoordinatePlane* cartesianPlane = new CartesianCoordinatePlane;
    replaceCoordinatePlane(cartesianPlane);
    coordinatePlane()->replaceDiagram(diagram);
    break;
  }
  case MyMoneyReport::eChartStackedBar: {
    KDChart::BarDiagram* diagram = new KDChart::BarDiagram;
    CartesianCoordinatePlane* cartesianPlane = new CartesianCoordinatePlane;
    replaceCoordinatePlane(cartesianPlane);
    diagram->setType(BarDiagram::Stacked);
    coordinatePlane()->replaceDiagram(diagram);
    break;
  }
  case MyMoneyReport::eChartPie: {
    KDChart::PieDiagram* diagram = new KDChart::PieDiagram;
    PolarCoordinatePlane* polarPlane = new PolarCoordinatePlane;
    replaceCoordinatePlane(polarPlane);
    coordinatePlane()->replaceDiagram(diagram);
    setAccountSeries(false);
    setSeriesTotals(true);
    break;
  }
  case MyMoneyReport::eChartRing: {
    KDChart::RingDiagram* diagram = new KDChart::RingDiagram;
    PolarCoordinatePlane* polarPlane = new PolarCoordinatePlane;
    replaceCoordinatePlane(polarPlane);
    polarPlane->replaceDiagram(diagram);
    //chartView.params()->setRelativeRingThickness( true );
    setAccountSeries(false);
    break;
  }
  }
  //get the diagram for later use
  AbstractDiagram* planeDiagram = coordinatePlane()->diagram();

  //set grid attributes
  GridAttributes gridAttr(coordinatePlane()->globalGridAttributes());
  gridAttr.setGridVisible(config.isChartGridLines());
  coordinatePlane()->setGlobalGridAttributes(gridAttr);

  //set data value attributes
  DataValueAttributes valueAttr(planeDiagram->dataValueAttributes());
  valueAttr.setVisible(config.isChartDataLabels());
  planeDiagram->setDataValueAttributes(valueAttr);
  planeDiagram->setAllowOverlappingDataValueTexts(false);

  //Subdued colors - we set it here again because it is a property of the diagram
  planeDiagram->useSubduedColors();

  //the legend will be used later

  Legend* legend = new Legend(planeDiagram, this);
  legend->setTitleText(i18nc("Chart legend title", "Legend"));

  //set up the axes for cartesian diagrams
  if (config.chartType() == MyMoneyReport::eChartLine ||
      config.chartType() == MyMoneyReport::eChartBar) {
    //set x axis
    CartesianAxis *xAxis = new CartesianAxis();
    xAxis->setPosition(CartesianAxis::Bottom);
    xAxis->setTitleText(i18n("Time"));
    TextAttributes xAxisTextAttr(xAxis->titleTextAttributes());
    xAxisTextAttr.setMinimalFontSize(KGlobalSettings::generalFont().pointSize());
    xAxis->setTitleTextAttributes(xAxisTextAttr);

    // Set up X axis labels (ie "abscissa" to use the technical term)
    QStringList abscissaNames;
    if (accountSeries()) { // if not, we will set these up while putting in the chart values.
      int column = 1;
      while (column < numColumns()) {
        abscissaNames += QString(columnHeadings[column++]).replace("&nbsp;", " ");
      }
      xAxis->setLabels(abscissaNames);
    }

    //set y axis
    CartesianAxis *yAxis = new CartesianAxis();
    yAxis->setPosition(CartesianAxis::Left);
    yAxis->setTitleText(i18n("Balance"));
    TextAttributes yAxisTextAttr(yAxis->titleTextAttributes());
    yAxisTextAttr.setMinimalFontSize(KGlobalSettings::generalFont().pointSize());
    yAxis->setTitleTextAttributes(yAxisTextAttr);

    //add the axes to the corresponding diagram
    if (config.chartType() == MyMoneyReport::eChartLine) {
      KDChart::LineDiagram* lineDiagram = qobject_cast<LineDiagram*>(planeDiagram);

      //set line width
      /*QPen linePen(lineDiagram->pen());
      linePen.setWidth(config.chartLineWidth());
      lineDiagram->setPen(linePen);*/

      //remove all existing axes before inserting new ones
      while (lineDiagram->axes().count() > 0) {
        CartesianAxis *delAxis  = lineDiagram->axes().at(0);
        lineDiagram->takeAxis(delAxis);
        delete delAxis;
      }

      //add the new axes
      lineDiagram->addAxis(xAxis);
      lineDiagram->addAxis(yAxis);

    } else if (config.chartType() == MyMoneyReport::eChartBar) {
      KDChart::BarDiagram* barDiagram = qobject_cast<BarDiagram*>(planeDiagram);
      barDiagram->addAxis(xAxis);
      barDiagram->addAxis(yAxis);
    }
  }

  //line markers
  DataValueAttributes dataValueAttr(planeDiagram->dataValueAttributes());
  MarkerAttributes markerAttr(dataValueAttr.markerAttributes());
  markerAttr.setVisible(true);
  markerAttr.setMarkerStyle(MarkerAttributes::MarkerCircle);
  markerAttr.setMarkerSize(QSize(8, 8));
  dataValueAttr.setMarkerAttributes(markerAttr);
  planeDiagram->setDataValueAttributes(dataValueAttr);

  // For onMouseOver events, we want to activate mouse tracking
  setMouseTracking(true);

  ::timetrace("loading rows");
  switch (config.detailLevel()) {
  case MyMoneyReport::eDetailNone:
  case MyMoneyReport::eDetailEnd:
  case MyMoneyReport::eDetailAll: {
    int rowNum = 0;

    // iterate over outer groups
    PivotGrid::const_iterator it_outergroup = grid.begin();
    while (it_outergroup != grid.end()) {
      // iterate over inner groups
      PivotOuterGroup::const_iterator it_innergroup = (*it_outergroup).begin();
      while (it_innergroup != (*it_outergroup).end()) {
        //
        // Rows
        //
        QString innergroupdata;
        PivotInnerGroup::const_iterator it_row = (*it_innergroup).begin();
        while (it_row != (*it_innergroup).end()) {
          //Do not include investments accounts in the chart because they are merely container of stock and other accounts
          if (it_row.key().accountType() != MyMoneyAccount::Investment) {
            //iterate row types
            for (int i = 0; i < rowTypeList.size(); ++i) {
              //skip the budget difference rowset
              if (rowTypeList[i] != eBudgetDiff) {
                QString legendText;

                //only show the column type in the header if there is more than one type
                if (rowTypeList.size() > 1) {
                  legendText = QString(columnTypeHeaderList[i] + " - " + it_row.key().name());
                } else {
                  legendText = QString(it_row.key().name());
                }

                //set the cell value and tooltip
                rowNum = drawPivotRowSet(rowNum, it_row.value(), rowTypeList[i], legendText, 1, numColumns());

                //set the legend text
                legend->setText(rowNum - 1, legendText);
              }
            }
          }
          ++it_row;
        }
        ++it_innergroup;
      }
      ++it_outergroup;
    }
  }
  break;

  case MyMoneyReport::eDetailTop: {
    int rowNum = 0;

    // iterate over outer groups
    PivotGrid::const_iterator it_outergroup = grid.begin();
    while (it_outergroup != grid.end()) {

      // iterate over inner groups
      PivotOuterGroup::const_iterator it_innergroup = (*it_outergroup).begin();
      while (it_innergroup != (*it_outergroup).end()) {
        //iterate row types
        for (int i = 0; i < rowTypeList.size(); ++i) {
          //skip the budget difference rowset
          if (rowTypeList[i] != eBudgetDiff) {
            QString legendText;


            //only show the column type in the header if there is more than one type
            if (rowTypeList.size() > 1) {
              legendText = QString(columnTypeHeaderList[i] + " - " + it_innergroup.key());
            } else {
              legendText = QString(it_innergroup.key());
            }

            //set the cell value and tooltip
            rowNum = drawPivotRowSet(rowNum, (*it_innergroup).m_total, rowTypeList[i], legendText, 1, numColumns());

            //set the legend text
            legend->setText(rowNum - 1, legendText);
          }
        }
        ++it_innergroup;
      }
      ++it_outergroup;
    }
  }
  break;

  case MyMoneyReport::eDetailGroup: {
    int rowNum = 0;

    // iterate over outer groups
    PivotGrid::const_iterator it_outergroup = grid.begin();
    while (it_outergroup != grid.end()) {
      //iterate row types
      for (int i = 0; i < rowTypeList.size(); ++i) {
        //skip the budget difference rowset
        if (rowTypeList[i] != eBudgetDiff) {
          QString legendText;

          //only show the column type in the header if there is more than one type
          if (rowTypeList.size() > 1) {
            legendText = QString(columnTypeHeaderList[i] + " - " + it_outergroup.key());
          } else {
            legendText = QString(it_outergroup.key());
          }

          //set the cell value and tooltip
          rowNum = drawPivotRowSet(rowNum, (*it_outergroup).m_total, rowTypeList[i], legendText, 1, numColumns());

          //set the legend
          legend->setText(rowNum - 1, legendText);
        }
      }
      ++it_outergroup;
    }

    //if selected, show totals too
    if (config.isShowingRowTotals()) {
      //iterate row types
      for (int i = 0; i < rowTypeList.size(); ++i) {
        //skip the budget difference rowset
        if (rowTypeList[i] != eBudgetDiff) {
          QString legendText;

          //only show the column type in the header if there is more than one type
          if (rowTypeList.size() > 1) {
            legendText = QString(columnTypeHeaderList[i] + " - " + i18nc("Total balance", "Total"));
          } else {
            legendText = QString(i18nc("Total balance", "Total"));
          }

          //set the cell value
          rowNum = drawPivotRowSet(rowNum, grid.m_total, rowTypeList[i], legendText, 1, numColumns());

          //set the legend
          legend->setText(rowNum - 1, legendText);

        }
      }
    }
  }
  break;

  case MyMoneyReport::eDetailTotal: {
    int rowNum = 0;

    //iterate row types
    for (int i = 0; i < rowTypeList.size(); ++i) {
      //skip the budget difference rowset
      if (rowTypeList[i] != eBudgetDiff) {
        QString legendText;

        //only show the column type in the header if there is more than one type
        if (rowTypeList.size() > 1) {
          legendText = QString(columnTypeHeaderList[i] + " - " + i18nc("Total balance", "Total"));
        } else {
          legendText = QString(i18nc("Total balance", "Total"));
        }

        if (config.isMixedTime() && (rowTypeList[i] == eActual || rowTypeList[i] == eForecast)) {
          if (rowTypeList[i] == eActual) {
            rowNum = drawPivotRowSet(rowNum, grid.m_total, rowTypeList[i], legendText, 1, config.currentDateColumn());
          } else if (rowTypeList[i] == eForecast) {
            rowNum = drawPivotRowSet(rowNum, grid.m_total, rowTypeList[i], legendText, config.currentDateColumn(), numColumns());
          } else {
            rowNum = drawPivotRowSet(rowNum, grid.m_total, rowTypeList[i], legendText, 1, numColumns());
          }
        } else {
          //set cell value
          rowNum = drawPivotRowSet(rowNum, grid.m_total, rowTypeList[i], legendText, 1, numColumns());
        }

        //set legend text
        legend->setText(rowNum - 1, legendText);
      }
    }
  }
  break;
  }
  ::timetrace("rows loaded");

  //assign model to the diagram
  planeDiagram->setModel(&m_model);

  //set the legend basic attributes
  //this is done after adding the legend because the values are overridden when adding the legend to the chart
  if (legend->texts().count() <= KMyMoneyGlobalSettings::maximumLegendItems()) {
    legend->setPosition(Position::East);
    TextAttributes legendTextAttr(legend->textAttributes());
    legendTextAttr.setFontSize(KGlobalSettings::generalFont().pointSize() + 2);
    legendTextAttr.setAutoShrink(true);
    legend->setTextAttributes(legendTextAttr);

    TextAttributes legendTitleTextAttr(legend->titleTextAttributes());
    legendTitleTextAttr.setFontSize(KGlobalSettings::generalFont().pointSize() + 4);
    legendTitleTextAttr.setAutoShrink(true);
    legend->setTitleTextAttributes(legendTitleTextAttr);
    legend->setTitleText(i18nc("Chart lines legend", "Legend"));
    legend->setUseAutomaticMarkerSize(false);

    replaceLegend(legend);
  } else {
    //if it is over the limit delete the legend
    takeLegend(legend);
    delete legend;
    if (legends().size()) {
      Legend* initialLegend = legends().at(0);
      takeLegend(initialLegend);
      delete initialLegend;
    }

  }

  //this sets the line width only for line diagrams
  setLineWidth(config.chartLineWidth());
  ::timetrace("finished drawing chart");
  //make sure to show only the required number of fractional digits on the labels of the graph
  //chartView.params()->setDataValuesCalc(0, MyMoneyMoney::denomToPrec(MyMoneyFile::instance()->baseCurrency().smallestAccountFraction()));
}

unsigned KReportChartView::drawPivotRowSet(int rowNum, const PivotGridRowSet& rowSet, const ERowType rowType, const QString& legendText, int startColumn, int endColumn)
{
  //if endColumn is invalid, make it the same as numColumns
  if (endColumn == 0) {
    endColumn = numColumns();
  }

  // Columns
  if (seriesTotals()) {
    double value = rowSet[rowType].m_total.toDouble();

    //set the tooltip
    QString toolTip = QString("<h2>%1</h2><strong>%2</strong><br>")
                      .arg(legendText)
                      .arg(value, 0, 'f', 2);

    if (accountSeries()) {
      //set the cell value
      this->setDataCell(rowNum, 0, value);
      this->setCellTip(rowNum, 0, toolTip);
    } else {
      this->setDataCell(0, rowNum, value);
      this->setCellTip(0, rowNum, toolTip);
    }
  } else {
    int column = startColumn;
    while (column <= endColumn && column < numColumns()) {
      double value = rowSet[rowType][column].toDouble();
      QString toolTip = QString("<h2>%1</h2><strong>%2</strong><br>")
                        .arg(legendText)
                        .arg(value, 0, 'f', 2);

      if (accountSeries()) {
        this->setDataCell(column - 1, rowNum, value);
        this->setCellTip(column - 1, rowNum, toolTip);
      } else {
        this->setDataCell(rowNum, column - 1, value);
        this->setCellTip(rowNum, column - 1, toolTip);
      }
      ++column;
    }
  }
  return ++rowNum;
}

void KReportChartView::setDataCell(int row, int column, const double data)
{
  if (! coordinatePlane()->diagram()->datasetDimension() == 1)
    return;

  justifyModelSize(row + 1, column + 1);

  const QModelIndex index = m_model.index(row, column);
  m_model.setData(index, QVariant(data), Qt::DisplayRole);
}

void KReportChartView::setCellTip(int row, int column, QString tip)
{
  if (! coordinatePlane()->diagram()->datasetDimension() == 1)
    return;

  justifyModelSize(row + 1, column + 1);

  const QModelIndex index = m_model.index(row, column);
  m_model.setData(index, QVariant(tip), Qt::ToolTipRole);
}

/**
 * Justifies the model, so that the given rows and columns fit into it.
 */
void KReportChartView::justifyModelSize(int rows, int columns)
{
  const int currentRows = m_model.rowCount();
  const int currentCols = m_model.columnCount();

  if (currentCols < columns)
    if (! m_model.insertColumns(currentCols, columns - currentCols))
      qDebug() << "justifyModelSize: could not increase model size.";
  if (currentRows < rows)
    if (! m_model.insertRows(currentRows, rows - currentRows))
      qDebug() << "justifyModelSize: could not increase model size.";

  Q_ASSERT(m_model.rowCount() >= rows);
  Q_ASSERT(m_model.columnCount() >= columns);
}

bool KReportChartView::event(QEvent* event)
{
  return Chart::event(event);
}

void KReportChartView::setLineWidth(const int lineWidth)
{
  qDebug("line width: %d", lineWidth);
  if (qobject_cast<LineDiagram*>(coordinatePlane()->diagram())) {
    LineDiagram* lineDiagram = qobject_cast<LineDiagram*>(coordinatePlane()->diagram());
    const int currentCols = m_model.columnCount();
    const int currentRows = m_model.rowCount();
    for (int col = 0; col < currentCols; ++col) {
        qDebug("column: %d", col);
        for (int row = 0; row < currentRows; ++row) {
        QPen pen(lineDiagram->pen(m_model.index(row, col, QModelIndex())));
        pen.setWidth(lineWidth);
        lineDiagram->setPen(m_model.index(row, col, QModelIndex()), pen);
      }
    }
  }
}

void KReportChartView::drawLimitLine(const double limit)
{
  //we get the current number of rows and we add one after that
  int row = m_model.rowCount();

  for (int col = 0; col < m_numColumns; ++col) {
    setDataCell(col, row, limit);
  }
//TODO: add format to the line
}

void KReportChartView::removeLegend(void)
{
  Legend* chartLegend = Chart::legend();
  delete chartLegend;
}
