//=========================================================================================
//  __   __  ___        ___  ______    _____   __   __   ________  ___      ___  _______
// |  | |  | \  \  /\  /  / |   _  \  |   __| |  | |  | |__    __| \  \    /  / /   _   \
// |  |_|  |  \  \/  \/  /  |  | |  | |  |    |  |_|  |    |  |     \  \  /  /  |  | |  |
// |   _   |   \        /   |  | |  | |  |    |   _   |    |  |      \  \/  /   |  |_|  |
// |  | |  |    \  /\  /    |  |_|  | |  |__  |  | |  |  __|  |__     \    /    |   _   |
// |__| |__|     \/  \/     |______/  |_____| |__| |__| |________|     \__/     |__| |__|
//
//=========================================================================================

#include "MainWindow.h"
#include "HttpRequestWorker.h"
#include "joystick/qjoystick.h"

#include <QProgressBar>
#include <QPalette>
#include <QShortcut>

#define ADJUST_BOTH_VALVES 1
#define ON_VALVE_COMMAND_REQUEST 0

const static float MIN_JOYSTICK_VALUE = -32768.0;
const static float MAX_JOYSTICK_VALUE = 32768.0;
const static float MIN_CONTROL_VALUE = 0.0;
const static float MAX_CONTROL_VALUE = 1.0;

const static QString base_comp = "valve_";

const static QString redBar = "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );border-bottom-right-radius: 3px;border-bottom-left-radius: 3px;}";
const static QString blueBar= "QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 #238 );border-bottom-right-radius: 3px;border-bottom-left-radius: 3px;}";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , joystick_(0)
  , valve_1_enabled_(false)
  , valve_2_enabled_(false)
  , valve_3_enabled_(false)
{
    setupUi(this);

    joystick_ = new QJoystick();

    int joysavailable = joystick_->availableJoysticks();
    debugWindow_->append("Available Joysticks: " + QString::number(joysavailable));
    if (joysavailable)
        joystick_->setJoystick(0);

    addValveModes();
    addValveBars();

    QObject::connect(joystick_, SIGNAL(axisMotion(int, int)), this, SLOT(on_axis_motion(int,int)));
    QObject::connect(joystick_, SIGNAL(buttonPressed(int, bool)), this, SLOT(on_button_pressed(int, bool)));

    valve1Ext = new QShortcut(QKeySequence(Qt::Key_W), this);
    QObject::connect(valve1Ext, SIGNAL(activated()), this, SLOT(on_valve_1_extend()));
    valve1Ret = new QShortcut(QKeySequence(Qt::Key_A), this);
    QObject::connect(valve1Ret, SIGNAL(activated()), this, SLOT(on_valve_1_retract()));

    valve2Ext = new QShortcut(QKeySequence(Qt::Key_R), this);
    QObject::connect(valve2Ext, SIGNAL(activated()), this, SLOT(on_valve_2_extend()));
    valve2Ret = new QShortcut(QKeySequence(Qt::Key_D), this);
    QObject::connect(valve2Ret, SIGNAL(activated()), this, SLOT(on_valve_2_retract()));

    valve3Ext = new QShortcut(QKeySequence(Qt::Key_Y), this);
    QObject::connect(valve3Ext, SIGNAL(activated()), this, SLOT(on_valve_3_extend()));
    valve3Ret = new QShortcut(QKeySequence(Qt::Key_G), this);
    QObject::connect(valve3Ret, SIGNAL(activated()), this, SLOT(on_valve_3_retract()));

    sensIncr = new QShortcut(QKeySequence(Qt::Key_I), this);
    QObject::connect(sensIncr, SIGNAL(activated()), this, SLOT(on_sensitivity_incr()));
    sensDecr = new QShortcut(QKeySequence(Qt::Key_J), this);
    QObject::connect(sensDecr, SIGNAL(activated()), this, SLOT(on_sensitivity_decr()));

    resetValves = new QShortcut(QKeySequence(Qt::Key_C), this);
    QObject::connect(resetValves, SIGNAL(activated()), this, SLOT(reset_all_valves()));

    joystickTimer_ = new QTimer();
    QObject::connect(joystickTimer_, SIGNAL(timeout()), joystick_, SLOT(pollData()));
    joystickTimer_->start(50);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_axis_motion(int a_axis, int a_value)
{
    switch (a_axis)
    {
        case 1:
            on_valve_input(a_value);
        break;

        case 2:
            on_sensitivity_input(a_value);
        break;
    }
}

void MainWindow::on_button_pressed(int a_button, bool down)
{
   switch (a_button)
   {
       case 0:
           on_valve_1_toggle(down);
       break;

       case 1:
           on_valve_2_toggle(down);
       break;

       case 2:
           on_valve_3_toggle(down);
       break;

       case 6:
       case 9:
           if (down) reset_all_valves();
       break;

       case 7:
       case 8:
           if (down) close();
       break;

       default:
           if (down) qDebug() << "Button " << a_button << " not used";
       break;
   }
}

void MainWindow::on_valve_1_toggle(bool valve_enable)
{
    valve_1_enabled_ = valve_enable;
    valve_2_enabled_ = false;
    valve_3_enabled_ = false;
}

void MainWindow::on_valve_2_toggle(bool valve_enable)
{
    valve_1_enabled_ = false;
    valve_2_enabled_ = valve_enable;
    valve_3_enabled_ = false;
}

void MainWindow::on_valve_3_toggle(bool valve_enable)
{
    valve_1_enabled_ = false;
    valve_2_enabled_ = false;
    valve_3_enabled_ = valve_enable;
}

float MainWindow::joystick_to_control(int a_value)
{
    //Y = (X-A)/(B-A) * (D-C) + C
    return (-a_value - MIN_JOYSTICK_VALUE) /
           (MAX_JOYSTICK_VALUE - MIN_JOYSTICK_VALUE) *
           (MAX_CONTROL_VALUE - MIN_CONTROL_VALUE) +
            MIN_CONTROL_VALUE;
}

float MainWindow::joystick_to_sensitivity(int a_value)
{
    //Y = (X-A)/(B-A) * (D-C) + C
    return (-a_value - MIN_JOYSTICK_VALUE) /
           (MAX_JOYSTICK_VALUE - MIN_JOYSTICK_VALUE) *
           ((MAX_CONTROL_VALUE/10) - (MIN_CONTROL_VALUE+0.01)) +
           (MIN_CONTROL_VALUE+0.01);
}

void MainWindow::on_sensitivity_input(int a_value)
{
    float sens = joystick_to_sensitivity(a_value);
    valveInput_[VALVE_SENS] = sens;

    updateValveBars();
}

void MainWindow::on_sensitivity_incr()
{
    valveInput_[VALVE_SENS] += MAX_CONTROL_VALUE/100.0;
    if (valveInput_[VALVE_SENS] > (MAX_CONTROL_VALUE/10.0))
        valveInput_[VALVE_SENS] = (MAX_CONTROL_VALUE/10.0);

    updateValveBars();
}

void MainWindow::on_sensitivity_decr()
{
    valveInput_[VALVE_SENS] -= MAX_CONTROL_VALUE/100.0;
    if (valveInput_[VALVE_SENS] < MIN_CONTROL_VALUE)
        valveInput_[VALVE_SENS] = MIN_CONTROL_VALUE;

    updateValveBars();
}

void MainWindow::on_valve_input(int a_value)
{
    if (valve_1_enabled_)
    {
        if (lastValveInput_[VALVE_1] > a_value)
            on_valve_1_extend();
        else if (lastValveInput_[VALVE_1] < a_value)
            on_valve_1_retract();

        lastValveInput_[VALVE_1] = a_value;
    }
    else if (valve_2_enabled_)
    {
        if (lastValveInput_[VALVE_2] > a_value)
            on_valve_2_extend();
        else if (lastValveInput_[VALVE_2] < a_value)
            on_valve_2_retract();

        lastValveInput_[VALVE_2] = a_value;
    }
    else if (valve_3_enabled_)
    {
        if (lastValveInput_[VALVE_3] > a_value)
            on_valve_3_extend();
        else if (lastValveInput_[VALVE_3] < a_value)
            on_valve_3_retract();

        lastValveInput_[VALVE_3] = a_value;
    }
}

void MainWindow::on_valve_1_extend()
{
    valveInput_[VALVE_1_EXT] += valveInput_[VALVE_SENS];
    send_valve_control_request(VALVE_1, true);
#if ADJUST_BOTH_VALVES
    valveInput_[VALVE_1_RET] -= valveInput_[VALVE_SENS];
    valveInput_[VALVE_1_RET] = 0;
    send_valve_control_request(VALVE_1, false);
#endif

#if ON_VALVE_COMMAND_REQUEST
    send_valve_command_request(VALVE_1);
#endif
}

void MainWindow::on_valve_1_retract()
{
#if ADJUST_BOTH_VALVES
    valveInput_[VALVE_1_EXT] -= valveInput_[VALVE_SENS];
    valveInput_[VALVE_1_EXT] = 0;
    send_valve_control_request(VALVE_1, true);
#endif
    valveInput_[VALVE_1_RET] += valveInput_[VALVE_SENS];
    send_valve_control_request(VALVE_1, false);

#if ON_VALVE_COMMAND_REQUEST
    send_valve_command_request(VALVE_1);
#endif
}

void MainWindow::on_valve_2_extend()
{
    valveInput_[VALVE_2_EXT] += valveInput_[VALVE_SENS];
    send_valve_control_request(VALVE_2, true);
#if ADJUST_BOTH_VALVES
    valveInput_[VALVE_2_RET] -= valveInput_[VALVE_SENS];
    valveInput_[VALVE_2_RET] = 0;
    send_valve_control_request(VALVE_2, false);
#endif

#if ON_VALVE_COMMAND_REQUEST
    send_valve_command_request(VALVE_2);
#endif
}

void MainWindow::on_valve_2_retract()
{
#if ADJUST_BOTH_VALVES
    valveInput_[VALVE_2_EXT] -= valveInput_[VALVE_SENS];
    valveInput_[VALVE_2_EXT] = 0;
    send_valve_control_request(VALVE_2, true);
#endif
    valveInput_[VALVE_2_RET] += valveInput_[VALVE_SENS];
    send_valve_control_request(VALVE_2, false);

#if ON_VALVE_COMMAND_REQUEST
    send_valve_command_request(VALVE_2);
#endif
}

void MainWindow::on_valve_3_extend()
{
    valveInput_[VALVE_3_EXT] += valveInput_[VALVE_SENS];
    send_valve_control_request(VALVE_3, true);
#if ADJUST_BOTH_VALVES
    valveInput_[VALVE_3_RET] -= valveInput_[VALVE_SENS];
    valveInput_[VALVE_3_RET] = 0;
    send_valve_control_request(VALVE_3, false);
#endif

#if ON_VALVE_COMMAND_REQUEST
    send_valve_command_request(VALVE_3);
#endif
}

void MainWindow::on_valve_3_retract()
{
#if ADJUST_BOTH_VALVES
    valveInput_[VALVE_3_EXT] -= valveInput_[VALVE_SENS];
    valveInput_[VALVE_3_EXT] = 0;
    send_valve_control_request(VALVE_3, true);
#endif
    valveInput_[VALVE_3_RET] += valveInput_[VALVE_SENS];
    send_valve_control_request(VALVE_3, false);

#if ON_VALVE_COMMAND_REQUEST
    send_valve_command_request(VALVE_3);
#endif
}

QString MainWindow::build_url_str(Valve a_valve, QString command, bool append)
{
    return q_baseAddress->text()
           + "/"
           + base_comp
           + QString::number(1)
           + "/call/"
           + command
           + (append?QString::number(a_valve+1):"");
}

void MainWindow::reset_all_valves()
{
    for (int i = 0; i < VALVE_OP_COUNT-1; i++)
        valveInput_[i] = 0.0;

    updateValveBars();

    //for (int i = 0; i < VALVE_COUNT; i++)
    for (int i = 0; i < 1; i++)
    {
        QString url_str = build_url_str((Valve) i, "ResetState", false);

        HttpRequestInput input(url_str, "POST");

        debugWindow_->append(input.url_str);

        HttpRequestWorker *worker = new HttpRequestWorker(this);
        connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result_POST(HttpRequestWorker*)));
        worker->execute(&input);
    }
}

void MainWindow::send_valve_command_request(Valve a_valve)
{
    QString url_str = build_url_str(a_valve, "sendControlCommand", false);

    HttpRequestInput input(url_str, "POST");

    debugWindow_->append(input.url_str);

    HttpRequestWorker *worker = new HttpRequestWorker(this);
    connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result_POST(HttpRequestWorker*)));
    worker->execute(&input);
}

void MainWindow::send_valve_control_request(Valve a_valve, bool extend)
{
    updateValveBars();

    // Set valve modes
    send_valve_modes_request(a_valve);

    // Build POST command
    QString url_str = build_url_str(a_valve, "setValveControlInput_");

    adjustValves();
    float valveControlInput = extend ? valveInput_[a_valve*2] : valveInput_[a_valve*2+1];

    HttpRequestInput input(url_str, "POST");
    input.add_var("value", QString::number(valveControlInput, 'f', 3));
    input.add_var("extend", QString::number(extend));

    debugWindow_->append(input.url_str + "->" + QString::number(valveControlInput, 'f', 3));

    HttpRequestWorker *worker = new HttpRequestWorker(this);
    connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result_POST(HttpRequestWorker*)));
    worker->execute(&input);
}

void MainWindow::send_valve_modes_request(Valve a_valve)
{
    QString url_str = build_url_str(a_valve, "setValveControlModes", false);

    HttpRequestInput input(url_str, "POST");
    input.add_var("mode1", a_valve==VALVE_1 ? QString::number(v1Mode_->currentIndex()) : QString::number(0));
    input.add_var("mode2", a_valve==VALVE_2 ? QString::number(v2Mode_->currentIndex()) : QString::number(0));
    input.add_var("mode3", a_valve==VALVE_3 ? QString::number(v3Mode_->currentIndex()) : QString::number(0));

    debugWindow_->append(input.url_str + "->" + QString::number(a_valve));

    HttpRequestWorker *worker = new HttpRequestWorker(this);
    connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result_POST(HttpRequestWorker*)));
    worker->execute(&input);
}

void MainWindow::send_valve_mode_request(Valve a_valve, int a_mode)
{
    QString url_str = build_url_str(a_valve, "setValveControlMode_");

    HttpRequestInput input(url_str, "POST");
    input.add_var("mode", QString::number(a_mode));

    debugWindow_->append(input.url_str + "->" + QString::number(a_mode));

    HttpRequestWorker *worker = new HttpRequestWorker(this);
    connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result_POST(HttpRequestWorker*)));
    worker->execute(&input);
}

void MainWindow::handle_result_POST(HttpRequestWorker *worker)
{
    // communication was successful
    if (worker->error_type != QNetworkReply::NoError)
        debugWindow_->append("Received: " + worker->error_str + ":" + QString::number(worker->error_type));
}

void MainWindow::handle_result(HttpRequestWorker *worker)
{

}

void MainWindow::addValveModes()
{
    valveModes_.append(v1Mode_);
    valveModes_.append(v2Mode_);
    valveModes_.append(v3Mode_);

    for (int i = 0; i < VALVE_COUNT; i++)
    {
        valveModes_.at(i)->addItem("0: Pass");
        valveModes_.at(i)->addItem("1: Pass,CC");
        valveModes_.at(i)->addItem("2: Blocked, CC");
        valveModes_.at(i)->addItem("3: Pass, PC");
        valveModes_.at(i)->addItem("4: Blocked, PC");
        valveModes_.at(i)->addItem("5: Blocked, DPC");
        valveModes_.at(i)->setCurrentIndex(4);
    }
}

void MainWindow::addValveBars()
{
    lastValveInput_.append(0.0);
    lastValveInput_.append(0.0);
    lastValveInput_.append(0.0);

    for (int i = 0; i < VALVE_OP_COUNT; i++)
    {
        valveInput_.append(0.0);
        valveBarList_.append(createProgressBar());
        if (i%2)
        {
            valveBarList_.at(i)->setInvertedAppearance(i%2);
            valveBarList_.at(i)->setStyleSheet(redBar);
        }
        else
            valveBarList_.at(i)->setStyleSheet(blueBar);

        if (i < 2)
            v1Layout_->addWidget(valveBarList_.at(i), 2, i%2, 1, 1);
        else if (i < 4)
            v2Layout_->addWidget(valveBarList_.at(i), 2, i%2, 1, 1);
        else if (i < 6)
            v3Layout_->addWidget(valveBarList_.at(i), 2, i%2, 1, 1);
        else
            sensLayout_->addWidget(valveBarList_.at(i), 1, i%2, 1, 1);
    }

    valveBarList_[VALVE_SENS]->setRange(MIN_CONTROL_VALUE, MAX_CONTROL_VALUE*10.0);
    valveInput_[VALVE_SENS] = 0.01;

    valveLabelList_.append(v1Ext_);
    valveLabelList_.append(v1Ret_);
    valveLabelList_.append(v2Ext_);
    valveLabelList_.append(v2Ret_);
    valveLabelList_.append(v3Ext_);
    valveLabelList_.append(v3Ret_);
    valveLabelList_.append(sensVal_);

    updateValveBars();
}

void MainWindow::updateValveBars()
{
    for (int i = 0; i < VALVE_OP_COUNT; i++)
    {
        valveLabelList_[i]->setText(QString::number(valveInput_[i], 'f', 3));
        valveBarList_[i]->setValue(valveInput_[i]*100.0);
    }
}

void MainWindow::adjustValves()
{
    for (int i = 0; i < VALVE_OP_COUNT-1; i++)
    {
        valveInput_[i] = qMax(MIN_CONTROL_VALUE, valveInput_[i]);
        valveInput_[i] = qMin(MAX_CONTROL_VALUE, valveInput_[i]);
    }
}

QProgressBar * MainWindow::createProgressBar()
{
    QProgressBar *tempbar = new QProgressBar;
    tempbar->setOrientation(Qt::Vertical);
    tempbar->setTextVisible(false);
    tempbar->setRange(MIN_CONTROL_VALUE, MAX_CONTROL_VALUE*100.0);
    tempbar->setValue(0.0);
    tempbar->setAlignment(Qt::AlignCenter);

    return tempbar;
}
