#ifndef QJOYSTICK_H
#define QJOYSTICK_H

#include <QObject>
#include <QString>
#include <QList>

#include "SDL/SDL.h"

class QJoystick : public QObject
{
    Q_OBJECT

public:

    QJoystick();
    ~QJoystick();

    int availableJoysticks();
    int currentJoystick();
    QString joystickName(int id);
    int joystickNumAxes(int id);
    int joystickNumButtons(int id);
    QList<int> axis;
    QList<bool> buttons;
    void getdata();

    int joystickGetAxisX();
    int joystickGetAxisY();
    int joystickGetAxisZ();

public slots:

    void setJoystick(int jsNumber);
    void pollData();

signals:

    void axisMotion(int a_axis, int a_value);
    void buttonPressed(int a_button, bool is_down);

private:

    SDL_Joystick* m_joystick;
};

#endif // QJOYSTICK_H
