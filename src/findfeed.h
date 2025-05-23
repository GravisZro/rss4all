#ifndef FINDFEED_H
#define FINDFEED_H

#include <QtWidgets>

class FindFeed : public QLineEdit
{
  Q_OBJECT
public:
  FindFeed(QWidget *parent = 0);
  void retranslateStrings();

  QActionGroup *findGroup_;

signals:
  void signalClear();
  void signalSelectFind();
  void signalVisible(bool);

protected:
  void keyPressEvent(QKeyEvent *event);
  void resizeEvent(QResizeEvent *);
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);

private slots:
  void updateClearButton(const QString &text);
  void slotClear();
  void slotMenuFind();
  void slotSelectFind(QAction *act);

private:
  QMenu *findMenu_;
  QAction *findNameAct_;
  QAction *findLinkAct_;

  QLabel *findLabel_;
  QToolButton *findButton_;
  QToolButton *clearButton_;

};

#endif // FINDFEED_H
