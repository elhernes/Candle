#include <cmath>

#include "rpnkeypad.h"
#include "ui_rpnkeypad.h"

static double degrees_to_radians(double degrees) {
  return degrees * (M_PI / 180.);
}

RpnKeypadDialog::RpnKeypadDialog(MachineControl *mc, QStack<float> &stack, QWidget* parent) : QDialog(parent), m_ui(new Ui::RpnKeypadDialog), m_stack(stack), m_mc(mc)
{
    m_ui->setupUi(this);
    m_ui->lineEdit->setValidator(new QDoubleValidator());
    m_ui->textEdit->setReadOnly(true);
    m_ui->textEdit->setAlignment(Qt::AlignRight);
    setWindowTitle("Candle RPN Keypad");
    redrawDisplay();
}

RpnKeypadDialog::~RpnKeypadDialog() { delete m_ui; }

/******************************** DIGITS ********************************/

void RpnKeypadDialog::on_button_0_clicked() { m_ui->lineEdit->insert("0"); }

void RpnKeypadDialog::on_button_1_clicked() { m_ui->lineEdit->insert("1"); }

void RpnKeypadDialog::on_button_2_clicked() { m_ui->lineEdit->insert("2"); }

void RpnKeypadDialog::on_button_3_clicked() { m_ui->lineEdit->insert("3"); }

void RpnKeypadDialog::on_button_4_clicked() { m_ui->lineEdit->insert("4"); }

void RpnKeypadDialog::on_button_5_clicked() { m_ui->lineEdit->insert("5"); }

void RpnKeypadDialog::on_button_6_clicked() { m_ui->lineEdit->insert("6"); }

void RpnKeypadDialog::on_button_7_clicked() { m_ui->lineEdit->insert("7"); }

void RpnKeypadDialog::on_button_8_clicked() { m_ui->lineEdit->insert("8"); }

void RpnKeypadDialog::on_button_9_clicked() { m_ui->lineEdit->insert("9"); }

void RpnKeypadDialog::on_button_dot_clicked() { m_ui->lineEdit->insert("."); }

/******************************** OPERATIONS ********************************/

void RpnKeypadDialog::on_button_sum_clicked()
{
    pushEntry();

    if (m_stack.length() < 2)                                  //stack must have at least two elements for an operation
        return;

    float a = popStack();
    float b = popStack();

    pushStack(b + a);                                         //inserts operation result into stack and updates TextEdit
}

void RpnKeypadDialog::on_button_subtract_clicked()
{
    pushEntry();

    if (m_stack.length() < 2)
        return;

    float a = popStack();
    float b = popStack();

    pushStack(b - a);
}

void RpnKeypadDialog::on_button_multiplication_clicked()
{
    pushEntry();

    if (m_stack.length() < 2)
        return;

    float a = popStack();
    float b = popStack();

    pushStack(b * a);
}

void RpnKeypadDialog::on_button_division_clicked()
{
    pushEntry();

    if (m_stack.length() < 2)
        return;

    if (m_stack.top() == 0)                                    //avoids division by zero
        return;

    float a = popStack();
    float b = popStack();

    pushStack(b / a);
}

void RpnKeypadDialog::on_button_sin_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(sin(degrees_to_radians(a)));
}

void RpnKeypadDialog::on_button_cos_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(cos(degrees_to_radians(a)));
}

void RpnKeypadDialog::on_button_tan_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(tan(degrees_to_radians(a)));
}

void RpnKeypadDialog::on_button_sqrt_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(sqrt(a));
}

void RpnKeypadDialog::on_button_plus_minus_clicked()
{
  if (m_ui->lineEdit->text() != "") {
    float val = m_ui->lineEdit->text().toFloat() * -1.;
    m_ui->lineEdit->setText(QString::number(val));
  } else if (!m_stack.isEmpty()) {
    float val = popStack();
    val *= -1.;
    pushStack(val);
  } else {
    return; // do nothing
  }
}

void RpnKeypadDialog::on_button_roll_down_clicked()
{
  pushEntry();
  if (!m_stack.isEmpty()) {
    float val = m_stack.front();
    m_stack.pop_front();
    m_stack.push_back(val);
    redrawDisplay();
  }
}

void RpnKeypadDialog::on_button_roll_up_clicked()
{
  pushEntry();
  if (!m_stack.isEmpty()) {
    float val = m_stack.last();
    m_stack.pop_back();
    m_stack.push_front(val);
    redrawDisplay();
  }
}

void RpnKeypadDialog::on_button_enter_clicked()
{
  if (m_ui->lineEdit->text() == "") {
    pushStack(m_stack.top());

  } else {

    pushEntry();
  }
}

void RpnKeypadDialog::on_button_swap_clicked()
{
    pushEntry();
    if (m_stack.length() < 2)
        return;

    float a = popStack();
    float b = popStack();

    pushStack(a);
    pushStack(b);
}


void RpnKeypadDialog::on_button_back_clicked() {
  if (m_ui->lineEdit->text() != "") {
    m_ui->lineEdit->backspace();
  } else {
    if (!m_stack.isEmpty()) {
      popStack();
    }
  }
}

void RpnKeypadDialog::on_button_go_x_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    bool jog = m_ui->toggle_jog_cut->isChecked();
    bool rel = m_ui->toggle_relative_absolute->isChecked();
    bool work = m_ui->toggle_work_machine->isChecked();
    switch((rel&1)<<1|(work&1)) {
    case 3: // relative  + work
    case 2: // relative + machine
      m_mc->goRelative({pos, 0, 0}, jog);
      break;
    case 1: // absolute + work
      m_mc->goAbsoluteWork({pos, std::nanf(""), std::nanf("")}, jog);
      break;
    case 0: // absolute + machine
      m_mc->goAbsoluteMachine({pos, std::nanf(""), std::nanf("")}, jog);
      break;
    }
  }
}

void RpnKeypadDialog::on_button_set_x_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_mc->setWorkPos({pos, std::nanf(""), std::nanf("")});
  }
}

void RpnKeypadDialog::on_button_push_x_clicked() {
  pushEntry();
  QVector3D pos;
  bool work = m_ui->toggle_work_machine->isChecked();
  if (work) {
    pos = m_mc->workPos();
  } else {
    pos = m_mc->machinePos();
  }
  pushStack(pos.x());
}

void RpnKeypadDialog::on_button_go_y_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    bool jog = m_ui->toggle_jog_cut->isChecked();
    bool rel = m_ui->toggle_relative_absolute->isChecked();
    bool work = m_ui->toggle_work_machine->isChecked();
    switch((rel&1)<<1|(work&1)) {
    case 3: // relative  + work
    case 2: // relative + machine
      m_mc->goRelative({0, pos, 0}, jog);
      break;
    case 1: // absolute + work
      m_mc->goAbsoluteWork({std::nanf(""), pos, std::nanf("")}, jog);
      break;
    case 0: // absolute + machine
      m_mc->goAbsoluteMachine({std::nanf(""), pos, std::nanf("")}, jog);
      break;
    }
  }
}


void RpnKeypadDialog::on_button_set_y_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_mc->setWorkPos({std::nanf(""), pos, std::nanf("")});
  }
}

void RpnKeypadDialog::on_button_push_y_clicked() {
  pushEntry();
  QVector3D pos;
  bool work = m_ui->toggle_work_machine->isChecked();
  if (work) {
    pos = m_mc->workPos();
  } else {
    pos = m_mc->machinePos();
  }
  pushStack(pos.y());
}

void RpnKeypadDialog::on_button_go_z_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    bool jog = m_ui->toggle_jog_cut->isChecked();
    bool rel = m_ui->toggle_relative_absolute->isChecked();
    bool work = m_ui->toggle_work_machine->isChecked();
    switch((rel&1)<<1|(work&1)) {
    case 3: // relative  + work
    case 2: // relative + machine
      m_mc->goRelative({0, 0, pos}, jog);
      break;
    case 1: // absolute + work
      m_mc->goAbsoluteWork({std::nanf(""), std::nanf(""), pos}, jog);
      break;
    case 0: // absolute + machine
      m_mc->goAbsoluteMachine({std::nanf(""), std::nanf(""), pos}, jog);
      break;
    }
  }
}


void RpnKeypadDialog::on_button_set_z_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_mc->setWorkPos({std::nanf(""), std::nanf(""), pos});
  }
}

void RpnKeypadDialog::on_button_push_z_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    QVector3D pos;
    bool work = m_ui->toggle_work_machine->isChecked();
    if (work) {
      pos = m_mc->workPos();
    } else {
      pos = m_mc->machinePos();
    }
    pushStack(pos.z());
  }
}

void RpnKeypadDialog::on_button_ok_clicked() {
  done(0);
}

void RpnKeypadDialog::on_button_set_spindle_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float speed = popStack();
    m_mc->setSpindle(speed);
  }
}

void RpnKeypadDialog::on_button_push_spindle_clicked() {
  pushEntry();
  pushStack(m_mc->spindle());
}

void RpnKeypadDialog::on_button_set_feed_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float feed = popStack();
    m_mc->setJogFeed(feed);
  }
}

void RpnKeypadDialog::on_button_push_feed_clicked() {
  pushEntry();
  pushStack(m_mc->jogFeed());
}

void RpnKeypadDialog::on_button_go_xy_clicked() {
  pushEntry();
  if (m_stack.size()>1) {
    float y = popStack();
    float x = popStack();

    bool jog = m_ui->toggle_jog_cut->isChecked();
    bool rel = m_ui->toggle_relative_absolute->isChecked();
    bool work = m_ui->toggle_work_machine->isChecked();
    switch((rel&1)<<1|(work&1)) {
    case 3: // relative  + work
    case 2: // relative + machine
      m_mc->goRelative({x, y, 0}, jog);
      break;
    case 1: // absolute + work
      m_mc->goAbsoluteWork({x, y, std::nanf("")}, jog);
      break;
    case 0: // absolute + machine
      m_mc->goAbsoluteMachine({x, y, std::nanf("")}, jog);
      break;
    }
  }
}


void RpnKeypadDialog::on_button_set_xy_clicked() {
  pushEntry();
  if (m_stack.size()>1) {
    float y = popStack();
    float x = popStack();
    m_mc->setWorkPos({x, y, std::nanf("")});
  }
}

void RpnKeypadDialog::on_button_push_xy_clicked() {
  pushEntry();
  bool work = m_ui->toggle_work_machine->isChecked();
  QVector3D pos;
  if (work) {
    pos = m_mc->workPos();
  } else {
    //    pos = m_mc->machinePos();
  }
  pushStack(pos.x());
  pushStack(pos.y());
}

void RpnKeypadDialog::on_toggle_work_machine_clicked() {
  QString label;
  bool enabled = true;
  if (m_ui->toggle_work_machine->isChecked()) {
    label = "Work";
    enabled = true;
  } else {
    label = "Machine";
    enabled = false;
  }
  m_ui->button_set_x->setEnabled(enabled);
  m_ui->button_set_y->setEnabled(enabled);
  m_ui->button_set_z->setEnabled(enabled);
  m_ui->button_set_xy->setEnabled(enabled);

  m_ui->toggle_work_machine->setText(label);
}

void RpnKeypadDialog::on_toggle_relative_absolute_clicked() {
  QString label;
  if (m_ui->toggle_relative_absolute->isChecked()) {
    label = "Relative";
  } else {
    label = "Absolute";
  }
  m_ui->toggle_relative_absolute->setText(label);
}

void RpnKeypadDialog::on_toggle_jog_cut_clicked() {
  QString label;
  if (m_ui->toggle_jog_cut->isChecked()) {
    label = "Jog";
  } else {
    label = "Rapid";
  }
  m_ui->toggle_jog_cut->setText(label);
}

/******************************** UTILS ********************************/

float RpnKeypadDialog::popStack()
{
    float last = m_stack.pop();                                               //removes last element

    redrawDisplay();
    return last;                                                            //returns removed element
}

void RpnKeypadDialog::pushStack(float n)
{
    float nn = round(n*100000.)/100000.;
    m_ui->textEdit->insertPlainText(QString::number(nn) + "\n");
    m_stack.push(n);
}

void RpnKeypadDialog::redrawDisplay() {
  m_ui->textEdit->clear();
  m_ui->textEdit->setAlignment(Qt::AlignRight);

  foreach(float i, m_stack) {
    float n = round(i*100000.)/100000.;
    m_ui->textEdit->insertPlainText(QString::number(n) + "\n");
  }

}

void RpnKeypadDialog::pushEntry() {
  if (m_ui->lineEdit->text()!="") {
    float next = m_ui->lineEdit->text().toFloat();
    m_ui->lineEdit->clear();
    pushStack(next);
  }
}
