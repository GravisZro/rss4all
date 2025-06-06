#include "dialog.h"

Dialog::Dialog(QWidget *parent, Qt::WindowFlags flag)
  : QDialog(parent, flag)
{
  pageLayout = new QVBoxLayout();
  pageLayout->setContentsMargins(10, 10, 10, 5);
  pageLayout->setSpacing(5);

  buttonBox = new QDialogButtonBox();

  buttonsLayout = new QHBoxLayout();
  buttonsLayout->setMargin(10);
  buttonsLayout->addWidget(buttonBox);

  QFrame *line = new QFrame();
  line->setFrameStyle(QFrame::HLine | QFrame::Sunken);

  mainLayout = new QVBoxLayout();
  mainLayout->setMargin(0);
  mainLayout->setSpacing(0);
  mainLayout->addLayout(pageLayout, 1);
  mainLayout->addWidget(line);
  mainLayout->addLayout(buttonsLayout);
  setLayout(mainLayout);

  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}
