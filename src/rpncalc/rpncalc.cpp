#include <cmath>

#include "rpncalc.h"
#include "ui_rpncalc.h"
#include "../frmmain.h"

static double degrees_to_radians(double degrees) {
  return degrees * (M_PI / 180.);
}

RpnCalcDialog::RpnCalcDialog(frmMain *frm, QStack<float> &stack, QWidget* parent) : m_stack(stack), QDialog(parent), m_ui(new Ui::RpnCalcDialog), m_frm(frm)
{
    m_ui->setupUi(this);
    m_ui->lineEdit->setValidator(new QDoubleValidator());
    m_ui->textEdit->setReadOnly(true);
    m_ui->textEdit->setAlignment(Qt::AlignRight);
    setWindowTitle("Candle RPN Keypad");
    redrawDisplay();
}

RpnCalcDialog::~RpnCalcDialog() { delete m_ui; }

/******************************** DIGITS ********************************/

void RpnCalcDialog::on_button_0_clicked() { m_ui->lineEdit->insert("0"); }

void RpnCalcDialog::on_button_1_clicked() { m_ui->lineEdit->insert("1"); }

void RpnCalcDialog::on_button_2_clicked() { m_ui->lineEdit->insert("2"); }

void RpnCalcDialog::on_button_3_clicked() { m_ui->lineEdit->insert("3"); }

void RpnCalcDialog::on_button_4_clicked() { m_ui->lineEdit->insert("4"); }

void RpnCalcDialog::on_button_5_clicked() { m_ui->lineEdit->insert("5"); }

void RpnCalcDialog::on_button_6_clicked() { m_ui->lineEdit->insert("6"); }

void RpnCalcDialog::on_button_7_clicked() { m_ui->lineEdit->insert("7"); }

void RpnCalcDialog::on_button_8_clicked() { m_ui->lineEdit->insert("8"); }

void RpnCalcDialog::on_button_9_clicked() { m_ui->lineEdit->insert("9"); }

void RpnCalcDialog::on_button_dot_clicked() { m_ui->lineEdit->insert("."); }

/******************************** OPERATIONS ********************************/

void RpnCalcDialog::on_button_sum_clicked()
{
    pushEntry();

    if (m_stack.length() < 2)                                  //stack must have at least two elements for an operation
        return;

    float a = popStack();
    float b = popStack();

    pushStack(b + a);                                         //inserts operation result into stack and updates TextEdit
}

void RpnCalcDialog::on_button_subtract_clicked()
{
    pushEntry();

    if (m_stack.length() < 2)
        return;

    float a = popStack();
    float b = popStack();

    pushStack(b - a);
}

void RpnCalcDialog::on_button_multiplication_clicked()
{
    pushEntry();

    if (m_stack.length() < 2)
        return;

    float a = popStack();
    float b = popStack();

    pushStack(b * a);
}

void RpnCalcDialog::on_button_division_clicked()
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

void RpnCalcDialog::on_button_sin_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(sin(degrees_to_radians(a)));
}

void RpnCalcDialog::on_button_cos_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(cos(degrees_to_radians(a)));
}

void RpnCalcDialog::on_button_tan_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(tan(degrees_to_radians(a)));
}

void RpnCalcDialog::on_button_sqrt_clicked()
{
    pushEntry();

    if (m_stack.isEmpty())
        return;

    float a = popStack();

    pushStack(sqrt(a));
}

void RpnCalcDialog::on_button_plus_minus_clicked()
{
  if (m_ui->lineEdit->text() != "") {
    float val = m_ui->lineEdit->text().toFloat() * -1.;
    m_ui->lineEdit->setText(QString::number(val));
  } else {
    float val = popStack();
    val *= -1.;
    pushStack(val);
  }
}

void RpnCalcDialog::on_button_roll_down_clicked()
{
  pushEntry();
  float val = m_stack.front();
  m_stack.pop_front();
  m_stack.push_back(val);
  redrawDisplay();
}

void RpnCalcDialog::on_button_roll_up_clicked()
{
  pushEntry();
  float val = m_stack.last();
  m_stack.pop_back();
  m_stack.push_front(val);
  redrawDisplay();
}

void RpnCalcDialog::on_button_enter_clicked()
{
  if (m_ui->lineEdit->text() == "") {
    pushStack(m_stack.top());

  } else {

    pushEntry();
  }
}

void RpnCalcDialog::on_button_swap_clicked()
{
    pushEntry();
    if (m_stack.length() < 2)
        return;

    float a = popStack();
    float b = popStack();

    pushStack(a);
    pushStack(b);
}


void RpnCalcDialog::on_button_back_clicked() {
  if (m_ui->lineEdit->text() != "") {
    m_ui->lineEdit->backspace();
  } else {
    if (!m_stack.isEmpty()) {
      popStack();
    }
  }
}

void RpnCalcDialog::on_button_go_x_abs_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->goAbsolute({pos, std::nanf(""), std::nanf("")});
  }
}

void RpnCalcDialog::on_button_go_x_rel_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->goRelative({pos, 0, 0});
  }
}

void RpnCalcDialog::on_button_set_x_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->setWorkPos({pos, std::nanf(""), std::nanf("")});
  }
}

void RpnCalcDialog::on_button_push_x_clicked() {
  pushEntry();
  QVector3D pos = m_frm->workPos();
  pushStack(pos.x());
}

void RpnCalcDialog::on_button_go_y_abs_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->goAbsolute({std::nanf(""),pos,std::nanf("")});
  }
}

void RpnCalcDialog::on_button_go_y_rel_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->goRelative({0,pos,0});
  }
}

void RpnCalcDialog::on_button_set_y_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->setWorkPos({std::nanf(""), pos, std::nanf("")});
  }
}

void RpnCalcDialog::on_button_push_y_clicked() {
  pushEntry();
  QVector3D pos = m_frm->workPos();
  pushStack(pos.y());
}

void RpnCalcDialog::on_button_go_z_abs_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->goAbsolute({std::nanf(""), std::nanf(""), pos});
  }
}

void RpnCalcDialog::on_button_go_z_rel_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = m_stack.last();
    m_frm->goRelative({0, 0, pos});
  }
}

void RpnCalcDialog::on_button_set_z_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float pos = popStack();
    m_frm->setWorkPos({std::nanf(""), std::nanf(""), pos});
  }
}

void RpnCalcDialog::on_button_push_z_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    QVector3D pos = m_frm->workPos();
    pushStack(pos.z());
  }
}

void RpnCalcDialog::on_button_ok_clicked() {
  done(0);
}

void RpnCalcDialog::on_button_set_spindle_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float speed = popStack();
    m_frm->setSpindle(speed);
  }
}

void RpnCalcDialog::on_button_push_spindle_clicked() {
  pushEntry();
  pushStack(m_frm->spindle());
}

void RpnCalcDialog::on_button_set_feed_clicked() {
  pushEntry();
  if (!m_stack.isEmpty()) {
    float feed = popStack();
    m_frm->setJogFeed(feed);
  }
}

void RpnCalcDialog::on_button_push_feed_clicked() {
  pushEntry();
  pushStack(m_frm->jogFeed());
}

void RpnCalcDialog::on_button_go_xy_abs_clicked() {
  pushEntry();
  if (m_stack.size()>1) {
    float y = popStack();
    float x = popStack();
    m_frm->goRelative({x, y, std::nanf("")});
  }
}

void RpnCalcDialog::on_button_go_xy_rel_clicked() {
  pushEntry();
  if (m_stack.size()>1) {
    float y = popStack();
    float x = popStack();
    m_frm->goRelative({x, y, 0});
  }
}

void RpnCalcDialog::on_button_set_xy_clicked() {
  pushEntry();
  if (m_stack.size()>1) {
    float y = popStack();
    float x = popStack();
    m_frm->setWorkPos({x, y, std::nanf("")});
  }
}

void RpnCalcDialog::on_button_push_xy_clicked() {
  pushEntry();
  QVector3D pos = m_frm->workPos();
  pushStack(pos.x());
  pushStack(pos.y());
}

/******************************** UTILS ********************************/

float RpnCalcDialog::popStack()
{
    float last = m_stack.pop();                                               //removes last element

    redrawDisplay();
    return last;                                                            //returns removed element
}

void RpnCalcDialog::pushStack(float n)
{
    float nn = round(n*100000.)/100000.;
    m_ui->textEdit->insertPlainText(QString::number(nn) + "\n");
    m_stack.push(n);
}

void RpnCalcDialog::redrawDisplay() {
  m_ui->textEdit->clear();
  m_ui->textEdit->setAlignment(Qt::AlignRight);

  foreach(float i, m_stack) {
    float n = round(i*100000.)/100000.;
    m_ui->textEdit->insertPlainText(QString::number(n) + "\n");
  }

}

void RpnCalcDialog::pushEntry() {
  if (m_ui->lineEdit->text()!="") {
    float next = m_ui->lineEdit->text().toFloat();
    m_ui->lineEdit->clear();
    pushStack(next);
  }
}
