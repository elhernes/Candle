#pragma once

#include <QDialog>

#include <QStack>
#include <QDebug>
#include <QScrollBar>
#include <QIntValidator>
#include <QKeyEvent>

class frmMain;

QT_BEGIN_NAMESPACE
namespace Ui { class RpnCalcDialog; }
QT_END_NAMESPACE

class RpnCalcDialog : public QDialog
{
    Q_OBJECT

public:
    RpnCalcDialog(frmMain *frm, QStack<float> &stack, QWidget* parent = nullptr);
    ~RpnCalcDialog();

private slots:
    void on_button_0_clicked();
    void on_button_1_clicked();
    void on_button_2_clicked();
    void on_button_3_clicked();
    void on_button_4_clicked();
    void on_button_5_clicked();
    void on_button_6_clicked();
    void on_button_7_clicked();
    void on_button_8_clicked();
    void on_button_9_clicked();
    void on_button_dot_clicked();
    void on_button_sum_clicked();
    void on_button_subtract_clicked();
    void on_button_multiplication_clicked();
    void on_button_division_clicked();
    void on_button_sin_clicked();
    void on_button_sqrt_clicked();
    void on_button_cos_clicked();
    void on_button_tan_clicked();
    void on_button_plus_minus_clicked();

    void on_button_roll_down_clicked();
    void on_button_roll_up_clicked();
    void on_button_swap_clicked();
    void on_button_enter_clicked();

    void on_button_back_clicked();
    void on_button_ok_clicked();

    void on_button_set_spindle_clicked();
    void on_button_push_spindle_clicked();

    void on_button_set_feed_clicked();
    void on_button_push_feed_clicked();

    void on_button_go_xy_clicked();
    void on_button_set_xy_clicked();
    void on_button_push_xy_clicked();

    void on_button_go_x_clicked();
    void on_button_set_x_clicked();
    void on_button_push_x_clicked();

    void on_button_go_y_clicked();
    void on_button_set_y_clicked();
    void on_button_push_y_clicked();

    void on_button_go_z_clicked();
    void on_button_set_z_clicked();
    void on_button_push_z_clicked();

    void on_toggle_work_machine_clicked();
    void on_toggle_relative_absolute_clicked();

 private:
    float popStack();
    void pushStack(float n);
    void redrawDisplay();
    void pushEntry();

    Ui::RpnCalcDialog* m_ui;
    QStack<float> &m_stack;
    frmMain *m_frm;
};
