/**
 * Copyright (c) 2009, Benjamin C. Meyer <ben@meyerhome.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "adblocksubscription.h"
#include "adblockmanager.h"
#include "adblocksearchtree.h"
#include "followredirectreply.h"
#include "mainapplication.h"
#include "networkmanager.h"
#include "common.h"

#include <QFile>
#include <QTimer>
#include <QNetworkReply>
#include <QDebug>
#include <QWebPage>

AdBlockSubscription::AdBlockSubscription(const QString &title, QObject* parent)
  : QObject(parent)
  , m_reply(0)
  , m_title(title)
  , m_updated(false)
{
}

QString AdBlockSubscription::title() const
{
  return m_title;
}

void AdBlockSubscription::setTitle(const QString &title)
{
  m_title = title;
}

QString AdBlockSubscription::filePath() const
{
  return m_filePath;
}

void AdBlockSubscription::setFilePath(const QString &path)
{
  m_filePath = path;
}

QUrl AdBlockSubscription::url() const
{
  return m_url;
}

void AdBlockSubscription::setUrl(const QUrl &url)
{
  m_url = url;
}

void AdBlockSubscription::loadSubscription(const QStringList &disabledRules)
{
  QFile file(m_filePath);

  if (!file.exists()) {
    QTimer::singleShot(0, this, SLOT(updateSubscription()));
    return;
  }

  if (!file.open(QFile::ReadOnly)) {
    qWarning() << "AdBlockSubscription::" << __FUNCTION__ << "Unable to open adblock file for reading" << m_filePath;
    QTimer::singleShot(0, this, SLOT(updateSubscription()));
    return;
  }

  QTextStream textStream(&file);
  textStream.setCodec("UTF-8");
  // Header is on 3rd line
  textStream.readLine(1024);
  textStream.readLine(1024);
  QString header = textStream.readLine(1024);

  if (!header.startsWith(QLatin1String("[Adblock")) || m_title.isEmpty()) {
    qWarning() << "AdBlockSubscription::" << __FUNCTION__ << "invalid format of adblock file" << m_filePath;
    QTimer::singleShot(0, this, SLOT(updateSubscription()));
    return;
  }

  m_rules.clear();

  while (!textStream.atEnd()) {
    AdBlockRule* rule = new AdBlockRule(textStream.readLine(), this);

    if (disabledRules.contains(rule->filter())) {
      rule->setEnabled(false);
    }

    m_rules.append(rule);
  }

  // Initial update
  if (m_rules.isEmpty() && !m_updated) {
    QTimer::singleShot(0, this, SLOT(updateSubscription()));
  }
}

void AdBlockSubscription::saveSubscription()
{
}

void AdBlockSubscription::updateSubscription()
{
  if (m_reply || !m_url.isValid()) {
    return;
  }

  m_reply = new FollowRedirectReply(m_url, mainApp->networkManager());

  connect(m_reply, SIGNAL(finished()), this, SLOT(subscriptionDownloaded()));
}

void AdBlockSubscription::subscriptionDownloaded()
{
  if (m_reply != qobject_cast<FollowRedirectReply*>(sender())) {
    return;
  }

  bool error = false;
  const QByteArray response = QString::fromUtf8(m_reply->readAll()).toUtf8();

  if (m_reply->error() != QNetworkReply::NoError ||
      !response.startsWith(QByteArray("[Adblock")) ||
      !saveDownloadedData(response)
      ) {
    error = true;
  }

  m_reply->deleteLater();
  m_reply = 0;

  if (error) {
    emit subscriptionError(tr("Cannot load subscription!"));
    return;
  }

  loadSubscription(AdBlockManager::instance()->disabledRules());

  emit subscriptionUpdated();
  emit subscriptionChanged();
}

bool AdBlockSubscription::saveDownloadedData(const QByteArray &data)
{
  QFile file(m_filePath);

  if (!file.open(QFile::ReadWrite | QFile::Truncate)) {
    qWarning() << "AdBlockSubscription::" << __FUNCTION__ << "Unable to open adblock file for writing:" << m_filePath;
    return false;
  }

  // Write subscription header
  file.write(QString("Title: %1\nUrl: %2\n").arg(title(), url().toString()).toUtf8());

  if (AdBlockManager::instance()->useLimitedEasyList() && m_url == QUrl(ADBLOCK_EASYLIST_URL)) {
    // Third-party advertisers rules are with start domain (||) placeholder which needs regexps
    // So we are ignoring it for keeping good performance
    // But we will use whitelist rules at the end of list

    QByteArray part1 = data.left(data.indexOf(QString("!-----------------------------Third-party adverts-----------------------------!").toLatin1()));
    QByteArray part2 = data.mid(data.indexOf(QString("!---------------------------------Whitelists----------------------------------!").toLatin1()));

    file.write(part1);
    file.write(part2);
    file.close();
    return true;
  }

  file.write(data);
  file.close();
  return true;
}

const AdBlockRule* AdBlockSubscription::rule(int offset) const
{
  if (!(offset >= 0 && m_rules.count() > offset)) {
    return 0;
  }

  return m_rules[offset];
}

QVector<AdBlockRule*> AdBlockSubscription::allRules() const
{
  return m_rules;
}

const AdBlockRule* AdBlockSubscription::enableRule(int offset)
{
  if (!(offset >= 0 && m_rules.count() > offset)) {
    return 0;
  }

  AdBlockRule* rule = m_rules[offset];
  rule->setEnabled(true);
  AdBlockManager::instance()->removeDisabledRule(rule->filter());

  emit subscriptionChanged();

  if (rule->isCssRule())
    mainApp->reloadUserStyleBrowser();

  return rule;
}

const AdBlockRule* AdBlockSubscription::disableRule(int offset)
{
  if (!(offset >= 0 && m_rules.count() > offset)) {
    return 0;
  }

  AdBlockRule* rule = m_rules[offset];
  rule->setEnabled(false);
  AdBlockManager::instance()->addDisabledRule(rule->filter());

  emit subscriptionChanged();

  if (rule->isCssRule())
    mainApp->reloadUserStyleBrowser();

  return rule;
}

bool AdBlockSubscription::canEditRules() const
{
  return false;
}

bool AdBlockSubscription::canBeRemoved() const
{
  return true;
}

int AdBlockSubscription::addRule(AdBlockRule* rule)
{
  Q_UNUSED(rule)
  return -1;
}

bool AdBlockSubscription::removeRule(int offset)
{
  Q_UNUSED(offset)
  return false;
}

const AdBlockRule* AdBlockSubscription::replaceRule(AdBlockRule* rule, int offset)
{
  Q_UNUSED(rule)
  Q_UNUSED(offset)
  return 0;
}

AdBlockSubscription::~AdBlockSubscription()
{
  qDeleteAll(m_rules);
}

// AdBlockCustomList

AdBlockCustomList::AdBlockCustomList(QObject* parent)
  : AdBlockSubscription(tr("Custom Rules"), parent)
{
  setFilePath(mainApp->dataDir() + "/adblock/customlist.txt");
}

void AdBlockCustomList::retranslateStrings()
{
  setTitle(tr("Custom Rules"));
}

void AdBlockCustomList::loadSubscription(const QStringList &disabledRules)
{
  // DuckDuckGo ad whitelist rules
  // They cannot be removed, but can be disabled.
  // Please consider not disabling them. Thanks!

  const QString ddg1 = QSL("@@||duckduckgo.com^$document");
  const QString ddg2 = QSL("duckduckgo.com#@#.has-ad");

  const QString rules = Common::readAllFileContents(filePath());

  QFile file(filePath());

  // for generate header to avoid load file error
  if (!file.exists()) {
    saveSubscription();
  }

  if (file.open(QFile::WriteOnly | QFile::Append)) {
    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    if (!rules.contains(ddg1 + QLatin1String("\n")))
      stream << ddg1 << Qt::endl;

    if (!rules.contains(QLatin1String("\n") + ddg2))
      stream << ddg2 << Qt::endl;
  }
  file.close();

  AdBlockSubscription::loadSubscription(disabledRules);
}

void AdBlockCustomList::saveSubscription()
{
  QFile file(filePath());

  if (!file.open(QFile::ReadWrite | QFile::Truncate)) {
    qWarning() << "AdBlockSubscription::" << __FUNCTION__ << "Unable to open adblock file for writing:" << filePath();
    return;
  }

  QTextStream textStream(&file);
  textStream.setCodec("UTF-8");
  textStream << "Title: " << title() << Qt::endl;
  textStream << "Url: " << url().toString() << Qt::endl;
  textStream << "[Adblock Plus 1.1.1]" << Qt::endl;

  foreach (const AdBlockRule* rule, m_rules) {
    textStream << rule->filter() << Qt::endl;
  }

  file.close();
}

bool AdBlockCustomList::canEditRules() const
{
  return true;
}

bool AdBlockCustomList::canBeRemoved() const
{
  return false;
}

bool AdBlockCustomList::containsFilter(const QString &filter) const
{
  foreach (const AdBlockRule* rule, m_rules) {
    if (rule->filter() == filter) {
      return true;
    }
  }

  return false;
}

bool AdBlockCustomList::removeFilter(const QString &filter)
{
  for (int i = 0; i < m_rules.count(); ++i) {
    const AdBlockRule* rule = m_rules.at(i);

    if (rule->filter() == filter) {
      return removeRule(i);
    }
  }

  return false;
}

int AdBlockCustomList::addRule(AdBlockRule* rule)
{
  m_rules.append(rule);

  emit subscriptionChanged();

  if (rule->isCssRule())
    mainApp->reloadUserStyleBrowser();

  return m_rules.count() - 1;
}

bool AdBlockCustomList::removeRule(int offset)
{
  if (!(offset >= 0 && m_rules.count() > offset)) {
    return false;
  }

  AdBlockRule* rule = m_rules.at(offset);
  const QString filter = rule->filter();

  m_rules.remove(offset);

  emit subscriptionChanged();

  if (rule->isCssRule())
    mainApp->reloadUserStyleBrowser();

  AdBlockManager::instance()->removeDisabledRule(filter);

  delete rule;
  return true;
}

const AdBlockRule* AdBlockCustomList::replaceRule(AdBlockRule* rule, int offset)
{
  if (!(offset >= 0 && m_rules.count() > offset)) {
    return 0;
  }

  AdBlockRule* oldRule = m_rules.at(offset);
  m_rules[offset] = rule;

  emit subscriptionChanged();

  if (rule->isCssRule() || oldRule->isCssRule())
    mainApp->reloadUserStyleBrowser();

  delete oldRule;
  return m_rules[offset];
}
