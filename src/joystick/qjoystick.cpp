#include "qjoystick.h"

#include <QDebug>

#define POLL_INTERVAL 40
#define USING_SDL2 1

QJoystick::QJoystick() :
    m_joystick(NULL)
{
    // Sure, we're only using the Joystick, but SDL doesn't work if video isn't initialised
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
}

int QJoystick::currentJoystick()
{
    if (m_joystick != NULL)
    {
#if USING_SDL2
        return SDL_JoystickInstanceID(m_joystick);
#else
        return SDL_JoystickIndex(m_joystick);
#endif
    }
    else
    {
        return -1;
    }
}

QString QJoystick::joystickName(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
#if USING_SDL2
    return QString(SDL_JoystickNameForIndex(js));
#else
    return QString(SDL_JoystickName(js));
#endif
}

int QJoystick::joystickNumAxes(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    if (m_joystick != NULL)
        return (SDL_JoystickNumAxes(m_joystick));
    else
        return 0;
}

int QJoystick::joystickNumButtons(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    if (m_joystick != NULL)
        return (SDL_JoystickNumButtons(m_joystick));
    else
        return 0;
}


void QJoystick::setJoystick(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);

    if (m_joystick != NULL)
        SDL_JoystickClose(m_joystick);
    m_joystick = SDL_JoystickOpen(js);
}

QJoystick::~QJoystick()
{
    axis.clear();
    buttons.clear();
    if (m_joystick != NULL)
    {
        SDL_JoystickClose(m_joystick);
        m_joystick = NULL;
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    this->deleteLater();
}


int QJoystick::availableJoysticks()
{
    return SDL_NumJoysticks();
}

void QJoystick::getdata()
{
    axis.clear();
    buttons.clear();

    if (m_joystick != NULL)
    {
        SDL_Event event;

        SDL_PollEvent(&event);

        for(int i=0;i<SDL_JoystickNumAxes(m_joystick);i++)
        {
            axis.append(SDL_JoystickGetAxis(m_joystick,i));
        }

        for(int i=0;i<SDL_JoystickNumButtons(m_joystick);i++)
        {
            buttons.append(SDL_JoystickGetButton(m_joystick,i));
        }
    }
}

int QJoystick::joystickGetAxisX()
{
    return SDL_JoystickGetAxis(m_joystick, 0);
}

int QJoystick::joystickGetAxisY()
{
    return SDL_JoystickGetAxis(m_joystick, 1);
}

int QJoystick::joystickGetAxisZ()
{
    return SDL_JoystickGetAxis(m_joystick, 2);
}

void QJoystick::pollData()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_JOYAXISMOTION:
            {
                qDebug() << "Axis: " << event.jaxis.axis << ", Value: " << event.jaxis.value;
                Q_EMIT(axisMotion(event.jaxis.axis, event.jaxis.value));
            }
            break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            {
                qDebug() << "Button" <<  (event.jbutton.state ? "Down: " : "Up: ") << event.jbutton.button;
                Q_EMIT(buttonPressed(event.jbutton.button, event.jbutton.state));
            }
            break;
        }
    }
}
