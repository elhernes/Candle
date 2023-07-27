#include <cmath>

#include "../rpn.h"
#include "qtkeypad.h"
#include "ui_qtkeypad.h"

#include <QRegularExpression>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

struct QtKeypadController::Privates {
  Privates(rpn::Interp &rpn, QtKeypadController *d) : _rpn(rpn), _rpnd(d), _ui(new Ui::RpnKeypad) {

    _ui->setupUi(_rpnd);
    QMenuBar *menubar = new QMenuBar(_rpnd);
    menubar->setNativeMenuBar(false);
    _ui->verticalLayout->setMenuBar(menubar);

    _mFile = menubar->addMenu("&File");
    QAction *action = new QAction("Open", _rpnd);
    _mFile->addAction(action);
    connect(action, &QAction::triggered, _rpnd, &QtKeypadController::on_file_open);

    action = new QAction("Save Stack", _rpnd);
    _mFile->addAction(action);
    connect(action, &QAction::triggered, _rpnd, &QtKeypadController::on_file_save_stack);

    action = new QAction("Restore Stack", _rpnd);
    _mFile->addAction(action);
    connect(action, &QAction::triggered, _rpnd, &QtKeypadController::on_file_restore_stack);

    _mKeys = menubar->addMenu("&Keys");

    _ui->textEdit->setReadOnly(true);
    _ui->textEdit->setAlignment(Qt::AlignRight);

    for(auto *w : programmableButtons()) {
      QPushButton *b = dynamic_cast<QPushButton*>(w);
      b->setProperty("rpn-word", ""); // QString(QString::fromStdString(btn)));
      b->setFixedWidth(75);
      connect(b, SIGNAL(clicked()), _rpnd, SLOT(on_programmable_button_clicked()));
    }

    auto v = QFontDatabase::addApplicationFont(":/etc/led-counter-7/led_counter-7.ttf");
    QString family = QFontDatabase::applicationFontFamilies(v).at(0);
    _ui->textEdit->setFontFamily(family);
    _ui->textEdit->setFontPointSize(18);
    redraw_display();

  }

  ~Privates() {
    delete _ui;
  }

  void clear_assigned_buttons() {
    for(auto *w : programmableButtons()) {
      QPushButton *b = dynamic_cast<QPushButton*>(w);
      b->setText("");
      b->setEnabled(false);
    }
    /*
    assign_button(1, 1, "calc-keys", "MATH");
    assign_button(2, 1, "stack-keys", "STACK");
    assign_button(3, 1, "logic-keys", "LOGIC");
    assign_button(4, 1, "type-keys", "TYPES");
    */
    assign_button(0,8, "CLEAR");
    assign_button(0,9, "OVER");
    assign_button(0,10, "SWAP");
  }

  rpn::WordDefinition::Result pushEntry() {
    rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
    std::string line = _ui->lineEdit->text().toStdString();
    if (line != "") {
      rv = _rpn.parse(line);
      _ui->lineEdit->clear();
    }
    return rv;
  }

  QList<QWidget*> programmableButtons() {
    QRegularExpression exp("^pb_");
    return _rpnd->findChildren<QWidget *>(exp);
  }

  rpn::Interp &_rpn;
  QtKeypadController *_rpnd;
  Ui::RpnKeypad* _ui;
  QMenu *_mKeys;
  QMenu *_mFile;

  void redraw_display();
  void assign_button(unsigned column, unsigned row, const std::string &rpnword, const QString &label="");
  void assign_menu(const QString &menu, const std::string &rpnword, const QString &label="");

};

QtKeypadController::QtKeypadController(rpn::Interp &rpn, QWidget* parent) : QWidget(parent), _p(new Privates(rpn, this))  {
  add_words(_p->_rpn);

  assignMenu("Keys", "stack-keys", "Stack");
  assignMenu("Keys", "math-keys", "Math");
  assignMenu("Keys", "logic-keys", "Logic");
  assignMenu("Keys", "type-keys", "Types");

  setWindowTitle("RPN Keypad");
  installEventFilter(this);
}

QtKeypadController::~QtKeypadController() {
  remove_words(_p->_rpn);
  delete _p;
}

void
QtKeypadController::assignButton(unsigned column, unsigned row, const std::string &rpnword, const std::string &label) {
  _p->assign_button(column,row,rpnword,QString::fromStdString(label));
}

void
QtKeypadController::assignMenu(const std::string &menu, const std::string &rpnword, const std::string &label) {
  _p->assign_menu(QString::fromStdString(menu),rpnword,QString::fromStdString(label));
}

void
QtKeypadController::clearAssignedButtons() {
  _p->clear_assigned_buttons();
}

bool
QtKeypadController::eventFilter(QObject *watched, QEvent *event) {
  if(event->type() == QKeyEvent::KeyPress) {
    QKeyEvent * ke = static_cast<QKeyEvent*>(event);
    if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
      on_button_enter_clicked();
      return true; // do not process this event further
    }
  }
  return false; // process this event further
}

/******************************** Menubar actions ***********************/
void
QtKeypadController::on_file_open() {
  QString fileName = QFileDialog::getOpenFileName(this,
						  "Open RPN Script", "", "RPN Files (*.rpn *.4th *.4nc)");
  if (fileName != "") {
    _p->_rpn.parseFile(fileName.toStdString());
    _p->redraw_display();
  }
}

void
QtKeypadController::on_file_save_stack() {
}
void
QtKeypadController::on_file_restore_stack() {
}

/******************************** DIGITS ********************************/

void QtKeypadController::on_button_0_clicked() { _p->_ui->lineEdit->insert("0"); }

void QtKeypadController::on_button_1_clicked() { _p->_ui->lineEdit->insert("1"); }

void QtKeypadController::on_button_2_clicked() { _p->_ui->lineEdit->insert("2"); }

void QtKeypadController::on_button_3_clicked() { _p->_ui->lineEdit->insert("3"); }

void QtKeypadController::on_button_4_clicked() { _p->_ui->lineEdit->insert("4"); }

void QtKeypadController::on_button_5_clicked() { _p->_ui->lineEdit->insert("5"); }

void QtKeypadController::on_button_6_clicked() { _p->_ui->lineEdit->insert("6"); }

void QtKeypadController::on_button_7_clicked() { _p->_ui->lineEdit->insert("7"); }

void QtKeypadController::on_button_8_clicked() { _p->_ui->lineEdit->insert("8"); }

void QtKeypadController::on_button_9_clicked() { _p->_ui->lineEdit->insert("9"); }

void QtKeypadController::on_button_dot_clicked() { _p->_ui->lineEdit->insert("."); }

/******************************** enter/back ********************************/


void
QtKeypadController::on_button_enter_clicked() {
  if (_p->_ui->lineEdit->text() == "") {
    std::string line = "DUP";
    _p->_rpn.parse(line);
  } else {
    _p->pushEntry();
  }
  _p->redraw_display();
}

void
QtKeypadController::on_button_back_clicked() {
  if (_p->_ui->lineEdit->text() != "") {
    _p->_ui->lineEdit->backspace();
  } else {
    std::string line = "DROP";
    _p->_rpn.parse(line);
  }
  _p->redraw_display();
}

void
QtKeypadController::on_button_chs_clicked() {
  if (_p->_ui->lineEdit->text() != "") {
    float val = _p->_ui->lineEdit->text().toFloat() * -1.;
    _p->_ui->lineEdit->setText(QString::number(val));
  } else {
    std::string line = "CHS";
    _p->_rpn.parse(line);
  }
  _p->redraw_display();
}

void QtKeypadController::on_button_add_clicked() {
  if (_p->pushEntry()==rpn::WordDefinition::Result::ok) {
      std::string line = "+";
      _p->_rpn.parse(line);
    }
  _p->redraw_display();
}

void QtKeypadController::on_button_subtract_clicked() {
  if (_p->pushEntry()==rpn::WordDefinition::Result::ok) {
      std::string line = "-";
      _p->_rpn.parse(line);
    }
  _p->redraw_display();
}

void QtKeypadController::on_button_multiply_clicked() {
  if (_p->pushEntry()==rpn::WordDefinition::Result::ok) {
      std::string line = "*";
      _p->_rpn.parse(line);
    }
  _p->redraw_display();
}

void QtKeypadController::on_button_divide_clicked() {
  if (_p->pushEntry()==rpn::WordDefinition::Result::ok) {
      std::string line = "/";
      _p->_rpn.parse(line);
    }
  _p->redraw_display();
}

/******************************** application programmable buttons ********************************/

void
QtKeypadController::Privates::assign_button(unsigned column, unsigned row, const std::string &rpnword, const QString &label) {
  char btn[32];
  snprintf(btn, sizeof(btn), "pb_%d_%d", row, column);
  auto *b = _rpnd->findChild<QPushButton*>(btn);
  if (b) {
    if (label != "") {
      b->setText(label);
    } else {
      b->setText(QString::fromStdString(rpnword));
    }
    b->setProperty("rpn-word", QString(QString::fromStdString(rpnword)));
    b->setEnabled(true);
  }
}

void
QtKeypadController::Privates::assign_menu(const QString &menu, const std::string &rpnword, const QString &l) {
  QString label = (l == "") ? QString::fromStdString(rpnword) : l;
  QAction *action = new QAction(l, _rpnd);
  action->setProperty("rpn-word", QString(QString::fromStdString(rpnword)));
  connect(action, &QAction::triggered, _rpnd, &QtKeypadController::on_programmable_button_clicked);
  _mKeys->addAction(action);
}

void QtKeypadController::on_programmable_button_clicked() {
  if (_p->pushEntry()==rpn::WordDefinition::Result::ok) {
    QObject *b = this->sender();
    QString qword = b->property("rpn-word").toString();
    if (qword != "") {
      std::string word=qword.toStdString();
      _p->_rpn.parse(word);
    }
  }
  _p->redraw_display();
}


/******************************** Stack display  ********************************/

void
QtKeypadController::Privates::redraw_display() {
  _ui->textEdit->clear();
  _ui->textEdit->setAlignment(Qt::AlignRight);
  
  for(size_t i=_rpn.stack.depth(); i!=0; i--) {
    char level[32];
    snprintf(level, sizeof(level), " : %02d%s", i, i>1?"\n":"");
    auto so = _rpn.stack.peek_as_string(i);
    _ui->textEdit->insertPlainText(QString::fromStdString(so+level));
  }
  _ui->statusLabel->setText(QString::fromStdString(_rpn.status()));
  _ui->textEdit->verticalScrollBar()->setValue(_ui->textEdit->verticalScrollBar()->maximum());
}
