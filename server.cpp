#include "server.h"
#include "ui_server.h"
#include <QMessageBox>

Server::Server(QWidget *parent, int nPort) :
    QMainWindow(parent),
    ui(new Ui::Server),
    _nextBlockSize(0)
{
    ui->setupUi(this);

    sock = new QTcpSocket(this);
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, nPort))
    {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unabe to start the зerver:"
                              + server->errorString()
                              ) ;
        server->close();
        return;
    }
    connect(this, SIGNAL(newLogs(QString&, QString&)), SLOT(takeLogs(QString&, QString&)));
}

void Server::slotNewConnection()
{
    QTcpSocket* clientSocket = server->nextPendingConnection();
    connect(clientSocket , SIGNAL(disconnected()), clientSocket, SLOT(deleteLater())) ;
    connect(clientSocket , SIGNAL(readyRead()), this, SLOT(slotReadClient()));
}

void Server::slotReadClient()
{
    // получаю логин-пароль
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    QString password, login;

    in.setVersion(QDataStream::Qt_5_12);
    for (; ;) {
        if (!_nextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> _nextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < _nextBlockSize) {
            break;
        }
        in >> login >> password;
        _nextBlockSize = 0;
    }

    emit newLogs(password, login);
}

void Server::sendToClient(QTcpSocket *sock, const Person &prs)
{
    QByteArray arrblck;
    QDataStream out(&arrblck, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_5_12);
    out << quint16(0) << prs;
    out.device()->seek(0);
    out << quint16(arrblck.size() - sizeof(quint16));
    this->sock->write(arrblck);
}

Server::~Server()
{
    delete ui;
}
