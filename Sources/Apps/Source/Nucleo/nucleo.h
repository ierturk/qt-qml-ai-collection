#ifndef NUCLEO_H
#define NUCLEO_H
#include <QThread>
#include <QObject>

class Nucleo : public QThread
{
    Q_OBJECT
public:
    Nucleo();
    void run();
private:
    QString name;
};
#endif // NUCLEO_H
