#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QDataStream>
#include <QTcpSocket>
#include <QHash>
#include "person.h"

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

    QTcpServer* server;
    QTcpSocket* sock;
    quint16 _nextBlockSize;
    QString login, password;
    QHash<QString, Person> dataBase;

    void sendToClient(QTcpSocket* sock, const Person&);

public:
    explicit Server(QWidget *parent = nullptr, int nPort = 1234);
    ~Server();

public slots:
    virtual void slotNewConnection();
    void slotReadClient();
    void takeLogs(QString& pass, QString& log);

signals:
    void newLogs(QString& pass, QString& log);

private:
    Ui::Server *ui;
};

#endif // SERVER_H
