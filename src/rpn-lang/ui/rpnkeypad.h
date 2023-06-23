#pragma once

#include <QDialog>

#include <QStack>
#include <QDebug>
#include <QScrollBar>

#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class RpnKeypad; }
QT_END_NAMESPACE

namespace rpn {

class Interp;

class KeypadController : public QWidget
{
    Q_OBJECT

public:
    KeypadController(Interp &rpn, QWidget* parent = nullptr);
    ~KeypadController();

    void assignButton(unsigned column, unsigned row, const std::string &rpnword, const QString &label);
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
    void on_button_chs_clicked();
    void on_button_enter_clicked();
    void on_button_back_clicked();

    void on_button_add_clicked();
    void on_button_subtract_clicked();
    void on_button_multiply_clicked();
    void on_button_divide_clicked();
    //    void on_button_ok_clicked();

    void on_programmable_button_clicked();

    void on_file_open();

 public:
    struct Privates;

 private:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    Privates *_p;
};

}
