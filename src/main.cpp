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
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
