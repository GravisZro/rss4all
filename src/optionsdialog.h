#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtWidgets>
#include <QtSql>
#include <QNetworkProxy>

#include "dialog.h"
#include "lineedit.h"
#include "notificationswidget.h"

#define STATIC_ICON_TRAY        0
#define CHANGE_ICON_TRAY        1
#define NEW_COUNT_ICON_TRAY     2
#define UNREAD_COUNT_ICON_TRAY  3

class OptionsDialog : public Dialog
{
  Q_OBJECT
public:
  explicit OptionsDialog(QWidget *parent);
  int currentIndex();
  void setCurrentItem(int index);

  // general
  QCheckBox *showSplashScreen_;
  QCheckBox *reopenFeedStartup_;
  QCheckBox *openNewTabNextToActive_;
  QCheckBox *hideFeedsOpenTab_;
  QCheckBox *showCloseButtonTab_;
  QCheckBox *showToggleFeedsTree_;
  QCheckBox *defaultIconFeeds_;
  QCheckBox *autocollapseFolder_;
  QCheckBox *storeDBMemory_;
  QSpinBox *saveDBMemFileInterval_;
  QCheckBox *saveDBMemFileMiniSystemTray_;

  // systemTray
  void setBehaviorIconTray(int behavior);
  int behaviorIconTray();
  QGroupBox *showTrayIconBox_;
  QCheckBox *startingTray_;
  QCheckBox *minimizingTray_;
  QCheckBox *closingTray_;
  QCheckBox *singleClickTray_;
  QCheckBox *clearStatusNew_;
  QCheckBox *emptyWorking_;

  // network connection
  QNetworkProxy proxy();
  void setProxy(const QNetworkProxy proxy);

  QSpinBox *timeoutRequest_;
  QSpinBox *numberRequests_;
  QSpinBox *numberRepeats_;

  LineEdit *editUserAgent_;

  // browser
  QRadioButton *embeddedBrowserOn_;
  QRadioButton *externalBrowserOn_;
  QRadioButton *defaultExternalBrowserOn_;
  QRadioButton *otherExternalBrowserOn_;
  LineEdit *otherExternalBrowserEdit_;
  QPushButton *otherExternalBrowserButton_;
  QCheckBox *autoLoadImages_;
  QCheckBox *javaScriptEnable_;
  QCheckBox *pluginsEnable_;
  QSpinBox *defaultZoomPages_;
  QCheckBox *openLinkInBackground_;
  QCheckBox *openLinkInBackgroundEmbedded_;
  LineEdit *userStyleBrowserEdit_;

  QSpinBox *maxPagesInCache_;
  QGroupBox *diskCacheOn_;
  QSpinBox *maxDiskCache_;
  QLineEdit *dirDiskCacheEdit_;
  QPushButton *dirDiskCacheButton_;

  QRadioButton *saveCookies_;
  QRadioButton *deleteCookiesOnClose_;
  QRadioButton *blockCookies_;
  QPushButton *clearCookies_;

  LineEdit *downloadLocationEdit_;
  QCheckBox *askDownloadLocation_;

  // feeds
  void setOpeningFeed(int action);
  int getOpeningFeed();
  QCheckBox *updateFeedsStartUp_;
  QCheckBox *updateFeedsEnable_;
  QSpinBox *updateFeedsInterval_;
  QComboBox *updateIntervalType_;

  QRadioButton *positionLastNews_;
  QRadioButton *positionFirstNews_;
  QRadioButton *positionUnreadNews_;
  QCheckBox *openNewsWebViewOn_;
  QRadioButton *nottoOpenNews_;

  QGroupBox *markNewsReadOn_;
  QRadioButton *markCurNewsRead_;
  QRadioButton *markPrevNewsRead_;
  QSpinBox *markNewsReadTime_;
  QCheckBox *markReadSwitchingFeed_;
  QCheckBox *markReadClosingTab_;
  QCheckBox *markReadMinimize_;

  QCheckBox *showDescriptionNews_;

  QComboBox *formatDate_;
  QComboBox *formatTime_;

  QCheckBox *alternatingRowColorsNews_;
  QCheckBox *simplifiedDateTime_;

  QComboBox *mainNewsFilter_;

  LineEdit *styleSheetNewsEdit_;

  QCheckBox *changeBehaviorActionNUN_;
  QCheckBox *notDeleteStarred_;
  QCheckBox *notDeleteLabeled_;
  QCheckBox *markIdenticalNewsRead_;

  QGroupBox *cleanupOnShutdownBox_;
  QCheckBox *dayCleanUpOn_;
  QSpinBox *maxDayCleanUp_;
  QCheckBox *newsCleanUpOn_;
  QSpinBox *maxNewsCleanUp_;
  QCheckBox *readCleanUp_;
  QCheckBox *neverUnreadCleanUp_;
  QCheckBox *neverStarCleanUp_;
  QCheckBox *neverLabelCleanUp_;
  QCheckBox *cleanUpDeleted_;
  QCheckBox *optimizeDB_;

  QGroupBox *avoidedOldNewsDateOn_;
  QCalendarWidget *avoidedOldNewsDate_;

  // labels
  QStringList idLabels_;
  QTreeWidget *labelsTree_;

  // notifier
  QGroupBox *soundNotifyBox_;
  QLineEdit *editSoundNotifer_;
  QPushButton *selectionSoundNotifer_;
  QPushButton *playSoundNotifer_;

  QGroupBox *showNotifyOn_;
  QComboBox *screenNotify_;
  QComboBox *positionNotify_;
  QSpinBox *transparencyNotify_;
  QSpinBox *countShowNewsNotify_;
  QSpinBox *timeShowNewsNotify_;
  QSpinBox *widthTitleNewsNotify_;

  QCheckBox *showTitlesFeedsNotify_;
  QCheckBox *showIconFeedNotify_;
  QCheckBox *showButtonMarkAllNotify_;
  QCheckBox *showButtonMarkReadNotify_;
  QCheckBox *showButtonExBrowserNotify_;
  QCheckBox *showButtonDeleteNotify_;
  QCheckBox *closeNotify_;

  QCheckBox *fullscreenModeNotify_;
  QCheckBox *showNotifyInactiveApp_;
  QCheckBox *onlySelectedFeeds_;
  QTreeWidget *feedsTreeNotify_;
  bool itemNotChecked_;
  Dialog *feedsNotifierDlg_;

  // language
  QString language();
  void setLanguage(const QString &langFileName);

  // fonts
  QTreeWidget *fontsTree_;
  QTreeWidget *colorsTree_;
  QFontComboBox *browserStandardFont_;
  QFontComboBox *browserFixedFont_;
  QFontComboBox *browserSerifFont_;
  QFontComboBox *browserSansSerifFont_;
  QFontComboBox *browserCursiveFont_;
  QFontComboBox *browserFantasyFont_;
  QSpinBox *browserDefaultFontSize_;
  QSpinBox *browserFixedFontSize_;
  QSpinBox *browserMinFontSize_;
  QSpinBox *browserMinLogFontSize_;

  // shortcut
  void loadActionShortcut(QList<QAction *> actions, QStringList *list);
  void saveActionShortcut(QList<QAction *> actions, QActionGroup *labelGroup);

signals:
  void signalCategoriesTreeKeyUpDownPressed();
  void signalShortcutTreeUpDownPressed();
  void signalPlaySound(const QString &soundPath);

protected:
  bool eventFilter(QObject *obj, QEvent *event);

private slots:
  void slotCategoriesItemClicked(QTreeWidgetItem* item, int);
  void slotCategoriesTreeKeyUpDownPressed();
  void manualProxyToggle(bool checked);
  void updateProxy();
  void applyProxy();
  void acceptDialog();
  void closeDialog();
  void slotFontChange();
  void slotFontReset();
  void slotColorChange();
  void slotColorReset();
  void shortcutTreeClicked(const QModelIndex &index);
  void slotShortcutTreeUpDownPressed();
  void slotClearShortcut();
  void slotResetShortcut();
  void filterShortcutChanged(const QString &text);
  void selectionBrowser();
  void selectionUserStyleNews();
  void selectionSoundNotifer();
  void slotPlaySoundNotifer();
  void feedsTreeNotifyItemChanged(QTreeWidgetItem* item,int column);
  void setCheckStateItem(QTreeWidgetItem *item, Qt::CheckState state);
  void showNotification();
  void deleteNotification();
  void newLabel();
  void editLabel();
  void deleteLabel();
  void moveUpLabel();
  void moveDownLabel();
  void slotCurrentLabelChanged(QTreeWidgetItem *current, QTreeWidgetItem *);
  void slotDeletePass();
  void slotDeleteAllPass();
  void slotShowPass();
  void selectionUserStyleBrowser();
  void selectionDirDiskCache();
  void selectionDownloadLocation();

private:
  void showEvent(QShowEvent*);

  QSqlDatabase db_;

  QLabel *contentLabel_;
  QTreeWidget *categoriesTree_;
  QStackedWidget *contentStack_;
  QScrollArea *scrollArea_;

  //stack widgets
  QFrame *generalWidget_;
  QFrame *traySystemWidget_;
  QFrame *networkConnectionsWidget_;
  QTabWidget *browserWidget_;
  QTabWidget *feedsWidget_;
  QWidget *labelsWidget_;
  QTabWidget *notifierWidget_;
  QWidget *passwordsWidget_;
  QWidget *languageWidget_;
  QTabWidget *fontsColorsWidget_;
  QWidget *shortcutWidget_;
  QWidget *avoidOldNewsWidget_;

  // general
  void createGeneralWidget();
  QCheckBox *autoRunEnabled_;
  QSettings *autoRunSettings_;

  // systemTray
  void createTraySystemWidget();
  QRadioButton *staticIconTray_;
  QRadioButton *changeIconTray_;
  QRadioButton *newCountTray_;
  QRadioButton *unreadCountTray_;

  // network connection
  void createNetworkConnectionsWidget();
  QRadioButton *systemProxyButton_;
  QRadioButton *directConnectionButton_;
  QRadioButton *manualProxyButton_;
  QWidget *manualWidget_;
  QComboBox *typeProxy_;
  LineEdit *editHost_;
  LineEdit *editPort_;
  LineEdit *editUser_;
  LineEdit *editPassword_;
  QNetworkProxy networkProxy_;

  // browser
  void createBrowserWidget();

  // feeds
  void createFeedsWidget();

  // notifier
  void createNotifierWidget();
  void createFeedsNotifierDlg();
  void loadNotifier();
  void applyNotifier();
  bool loadNotifierOk_;
  NotificationWidget *notificationWidget_;

  // labels
  void createLabelsWidget();
  void loadLabels();
  void applyLabels();
  void addIdLabelList(const QString &idLabel);
  QPushButton *newLabelButton_;
  QPushButton *editLabelButton_;
  QPushButton *deleteLabelButton_;
  QPushButton *moveUpLabelButton_;
  QPushButton *moveDownLabelButton_;
  bool loadLabelsOk_;

  // passwords
  void createPasswordsWidget();
  void applyPass();
  QTreeWidget *passTree_;

  // language
  void createLanguageWidget();
  QTreeWidget *languageFileList_;

  // fonts
  void createFontsColorsWidget();

  // shortcut
  void createShortcutWidget();
  QStringList *listDefaultShortcut_;
  LineEdit *filterShortcut_;
  QTreeView *shortcutTree_;
  QStandardItemModel *shortcutModel_;
  QSortFilterProxyModel *shortcutProxyModel_;
  LineEdit *editShortcut_;
  QGroupBox *editShortcutBox;
  QLabel *warningShortcut_;

};

#endif // OPTIONSDIALOG_H
