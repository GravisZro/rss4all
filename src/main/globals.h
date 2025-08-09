#ifndef GLOBALS_H
#define GLOBALS_H

#include "VersionRev.h"
#include <QString>

#define DEFAULT_USER_AGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36"

class Globals
{
public:
  Globals();

  void init();

  QString userAgent() const { return userAgent_; }
  void setUserAgent(const QString userAgent);

  // public on purpose
  const bool logFileOutput_;
  bool noDebugOutput_;
  bool isInit_;
  bool isPortable_;
  QString resourcesDir_;
  QString dataDir_;
  QString cacheDir_;
  QString soundNotifyDir_;

private:
  QString userAgent_;

};

extern Globals globals;

#endif // GLOBALS_H
