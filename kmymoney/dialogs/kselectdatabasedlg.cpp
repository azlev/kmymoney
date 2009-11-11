/***************************************************************************
                          kselectdatabasedlg.cpp
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
#include "kselectdatabasedlg.h"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// ----------------------------------------------------------------------------
// QT Includes

#include <QLayout>
#include <QPushButton>
#include <QApplication>
#include <QSqlDatabase>
#include <QStatusBar>
#include <QCheckBox>
#include <QColor>
#include <QTextStream>

// ----------------------------------------------------------------------------
// KDE Includes

#include <kapplication.h>
#include <kurlrequester.h>
#include <ktextbrowser.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>
#include <kfiledialog.h>
// ----------------------------------------------------------------------------
// Project Includes

#define buttonSQL KDialog::User1

KSelectDatabaseDlg::KSelectDatabaseDlg(int openMode, KUrl openURL, QWidget *) {
  m_widget = new KSelectDatabaseDlgDecl();
  setMainWidget(m_widget);
  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help | buttonSQL);
  setButtonText (buttonSQL, i18n("Generate SQL"));
  enableButton(buttonSQL, false);
  connect (this, SIGNAL(user1Clicked()), this, SLOT(slotGenerateSQL()));
  connect (this, SIGNAL(helpClicked()), this, SLOT(slotHelp()));
  m_requiredFields = 0;
  m_url = openURL;
  m_mode = openMode;
  m_widget->checkPreLoad->setEnabled (openMode == QIODevice::ReadWrite);
}

KSelectDatabaseDlg::~KSelectDatabaseDlg() {
  if (m_requiredFields != 0) delete m_requiredFields;
}

bool KSelectDatabaseDlg::checkDrivers() {
  // list drivers supported by KMM
  QMap<QString, QString> map = m_map.driverMap();
  // list drivers installed on system
  QStringList list = QSqlDatabase::drivers();
  // join the two
  QStringList::Iterator it = list.begin();
  while(it != list.end()) {
    QString dname = *it;
    if (map.keys().contains(dname)) { // only keep if driver is supported
      dname = dname + " - " + map[dname];
      m_supportedDrivers.append(dname);
    }
    it  ++;
  }
  if (m_url != KUrl()) {
    QString driverName = m_url.queryItem("driver");
    if (!list.contains(driverName)) {
      KMessageBox::error (0, i18n("Qt SQL driver %1 is no longer installed on your system", driverName),
        "");
      return (false);
    }
  }
  if (m_supportedDrivers.count() == 0) {
      // why does KMessageBox not have a standard dialog with Help button?
    if ((KMessageBox::questionYesNo(this,
         i18n("In order to use a database, you need to install some additional software. Click Help for more information"),
         i18n("No Qt SQL Drivers"),
         KStandardGuiItem::help(), KStandardGuiItem::cancel()))
        == KMessageBox::Yes) { // Yes stands in for help here
      KToolInvocation::invokeHelp("details.database.usage");
    }
    return(false);
  }
  return (true);
}

int KSelectDatabaseDlg::exec() {
  m_widget->listDrivers->clear();
  if (m_url == KUrl()) {
    m_widget->listDrivers->addItems(m_supportedDrivers);
    m_widget->textDbName->setText ("KMyMoney");
    m_widget->textHostName->setText ("localhost");
    m_widget->textUserName->setText("");
    struct passwd * pwd = getpwuid(geteuid());
    if (pwd != 0)
      m_widget->textUserName->setText (QString(pwd->pw_name));
    m_widget->textPassword->setText ("");
    m_requiredFields = new kMandatoryFieldGroup(this);
    m_requiredFields->setOkButton(button(KDialog::Ok));
    m_requiredFields->add(m_widget->listDrivers);
    m_requiredFields->add(m_widget->textDbName);
    connect (m_widget->listDrivers, SIGNAL(itemClicked(QListWidgetItem *)),
           this, SLOT(slotDriverSelected(QListWidgetItem *)));
    m_widget->checkPreLoad->setChecked(false);
    enableButton(buttonSQL, false);
    // ensure a driver gets selected; pre-select if only one
    if (m_widget->listDrivers->count() == 1) {
      m_widget->listDrivers->setCurrentItem(m_widget->listDrivers->item(0));
      slotDriverSelected(m_widget->listDrivers->item(0));
    }
  } else {
    // fill in the fixed data from the URL
    QString driverName = m_url.queryItem("driver");
    m_widget->listDrivers->addItem (QString(driverName + " - " + m_map.driverMap()[driverName]));
    m_widget->listDrivers->setCurrentItem(m_widget->listDrivers->item(0));
    QString dbName = m_url.path().right(m_url.path().length() - 1); // remove separator slash
    m_widget->textDbName->setText (dbName);
    m_widget->textHostName->setText (m_url.host());
    m_widget->textUserName->setText(m_url.user());
    // disable all but the password field, coz that's why we're here
    m_widget->textDbName->setEnabled(false);
    m_widget->listDrivers->setEnabled(false);
    m_widget->textHostName->setEnabled(false);
    m_widget->textUserName->setEnabled(false);
    m_widget->textPassword->setEnabled(true);
    m_widget->textPassword->setFocus();
    enableButton(buttonSQL, true);
    // set password required
    m_requiredFields = new kMandatoryFieldGroup(this);
    m_requiredFields->add(m_widget->textPassword);
    m_requiredFields->setOkButton(button(KDialog::Ok));

    m_widget->checkPreLoad->setChecked(false);
  }

  return (QDialog::exec());
}

const KUrl KSelectDatabaseDlg::selectedURL() {
  KUrl url;
  url.setProtocol("sql");
  url.setUser(m_widget->textUserName->text());
  url.setPass(m_widget->textPassword->text());
  url.setHost(m_widget->textHostName->text());
  url.setPath('/' + m_widget->textDbName->text());
  QString qs = QString("driver=%1")
      .arg(m_widget->listDrivers->currentItem()->text().section (' ', 0, 0));
  if (m_widget->checkPreLoad->isChecked()) qs.append("&options=loadAll");
  if (!m_widget->textPassword->text().isEmpty()) qs.append("&secure=yes");
  url.setQuery(qs);
  return (url);
}

void KSelectDatabaseDlg::slotDriverSelected (QListWidgetItem *driver) {
  databaseTypeE dbType = m_map.driverToType(driver->text().section(' ', 0, 0));
  if (!m_map.isTested(dbType)) {
    int rc = KMessageBox::warningContinueCancel (0,
       i18n("Database type %1 has not been fully tested in a KMyMoney environment.\n"
            "Please make sure you have adequate backups of your data.\n"
            "Please report any problems to the developer mailing list at "
            "kmymoney-devel@kde.org", driver->text()),
        "");
    if (rc == KMessageBox::Cancel) {
      m_widget->listDrivers->clearSelection();
      return;
    }
  }

  enableButton(buttonSQL, true);

  if (m_map.driverToType(driver->text().section(' ', 0, 0)) == Sqlite3){
    QString dbName;
    if (m_mode == QIODevice::WriteOnly)
      dbName = KFileDialog::getSaveFileName(
      KUrl(),
      i18n("*.sql *.*|SQLite files (*.sql)| All files (*.*)"),
      this,
      i18n("Select SQLite file"));
    else
      dbName = KFileDialog::getOpenFileName(
      KUrl(),
      i18n("*.sql *.*|SQLite files (*.sql)| All files (*.*)"),
      this,
      i18n("Select SQLite file"));
    if (dbName.isNull())
      return;

    m_widget->textDbName->setText(dbName);
    // sqlite databases do not react to host/user/password;
    // file system permissions must be used
    m_widget->textHostName->setEnabled (false);
    m_widget->textUserName->setEnabled (false);
    m_widget->textPassword->setEnabled(false);
  } else {                         // not sqlite3
    m_widget->textUserName->setEnabled (true);
    m_widget->textHostName->setEnabled (true);
    m_widget->textPassword->setEnabled(true);
  }
}

// Generate SQL button processing
void KSelectDatabaseDlg::slotGenerateSQL() {
  QString fileName = KFileDialog::getSaveFileName(
      KUrl(),
      i18n("All files (*.*)"),
      this,
      i18n("Select output file"));
  if (fileName == "") return;
  QFile out(fileName);
  if (!out.open(QIODevice::WriteOnly)) return;
  QTextStream s(&out);
  MyMoneyDbDef db;
  s << db.generateSQL(m_widget->listDrivers->currentItem()->text().section (' ', 0, 0));
  out.close();
}

void KSelectDatabaseDlg::slotHelp(void) {
  KToolInvocation::invokeHelp("details.database.selectdatabase");
}
