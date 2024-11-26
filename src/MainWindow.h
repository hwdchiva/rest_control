//=========================================================================================
//  __   __  ___        ___  ______    _____   __   __   ________  ___      ___  _______
// |  | |  | \  \  /\  /  / |   _  \  |   __| |  | |  | |__    __| \  \    /  / /   _   \
// |  |_|  |  \  \/  \/  /  |  | |  | |  |    |  |_|  |    |  |     \  \  /  /  |  | |  |
// |   _   |   \        /   |  | |  | |  |    |   _   |    |  |      \  \/  /   |  |_|  |
// |  | |  |    \  /\  /    |  |_|  | |  |__  |  | |  |  __|  |__     \    /    |   _   |
// |__| |__|     \/  \/     |______/  |_____| |__| |__| |________|     \__/     |__| |__|
//
//=========================================================================================

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include "ui_MainWindow.h"

class QProgressBar;
class HttpRequestWorker;
class QJoystick;
class QShortcut;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:

    typedef enum ValveOp
    {
        VALVE_1_EXT,
        VALVE_1_RET,
        VALVE_2_EXT,
        VALVE_2_RET,
        VALVE_3_EXT,
        VALVE_3_RET,
        VALVE_SENS,
        VALVE_OP_COUNT
    } ValveOp;

    typedef enum Valve
    {
        VALVE_1,
        VALVE_2,
        VALVE_3,
        VALVE_COUNT
    } Valve;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void on_axis_motion(int a_axis, int a_value);
    void on_button_pressed(int a_button, bool down);
    void send_valve_mode_request(Valve a_value, int a_mode);

    void handle_result_POST(HttpRequestWorker *worker);
    void handle_result(HttpRequestWorker *worker);

    void on_valve_1_extend();
    void on_valve_1_retract();
    void on_valve_2_extend();
    void on_valve_2_retract();
    void on_valve_3_extend();
    void on_valve_3_retract();

    void on_sensitivity_incr();
    void on_sensitivity_decr();

    void reset_all_valves();

private:

    void on_valve_input(int a_value);
    void on_sensitivity_input(int a_value);

    void on_valve_1_toggle(bool valve_enable);
    void on_valve_2_toggle(bool valve_enable);
    void on_valve_3_toggle(bool valve_enable);

    void send_valve_modes_request(Valve a_valve);
    void send_valve_control_request(Valve a_valve, bool extend);
    void send_valve_command_request(Valve a_valve);
    void adjustValves();

    void addValveModes();
    void addValveBars();
    void updateValveBars();
    QProgressBar * createProgressBar();

    float joystick_to_control(int a_value);
    float joystick_to_sensitivity(int a_value);

    QString build_url_str(Valve a_valve, QString command, bool append = true);

private:

    QJoystick * joystick_;
    QTimer * joystickTimer_;

    bool valve_1_enabled_;
    bool valve_2_enabled_;
    bool valve_3_enabled_;

    QList<QLabel*> valveLabelList_;
    QList<float> lastValveInput_;
    QList<float> valveInput_;
    QList<QProgressBar*> valveBarList_;
    QList<QComboBox*> valveModes_;

    QShortcut *valve1Ext;
    QShortcut *valve1Ret;
    QShortcut *valve2Ext;
    QShortcut *valve2Ret;
    QShortcut *valve3Ext;
    QShortcut *valve3Ret;
    QShortcut *sensIncr;
    QShortcut *sensDecr;
    QShortcut *resetValves;

};

#endif // MAINWINDOW_H
