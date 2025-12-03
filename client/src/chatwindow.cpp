#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QMessageBox>

ChatWindow::ChatWindow(QString username, QWidget *parent)
    : QWidget(nullptr)
    , ui(new Ui::ChatWindow),
    username(username)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    mainwindowPtr = parent;

    connect(socket, &QTcpSocket::readyRead, this, &ChatWindow::onSocketReadyRead);

    connect(socket, &QTcpSocket::connected, this, &ChatWindow::onSocketConnected);

    connect(socket, &QTcpSocket::errorOccurred, this, &ChatWindow::onSocketErrorOccured);

    socket->connectToHost("127.0.0.1", 8000);

    connect(ui->messageInput, &QLineEdit::returnPressed, this, &ChatWindow::on_sendButton_clicked);
    ui->chatDisplay->append("Connecting to server....");
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::onSocketConnected(){
    ui->chatDisplay->append("Connected to server.");
    this->setWindowTitle("RoomlyChat (Connected)");
    socket->write(username.toUtf8());
}
void ChatWindow::onSocketReadyRead(){
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);

    if(message.startsWith(username + ":")){
        message.replace(0, username.length(), "You");
    }

    // qDebug() << "Received:" << message;

    ui->chatDisplay->append(message);
}
void ChatWindow::onSocketErrorOccured(QAbstractSocket::SocketError){
    QMessageBox::critical(this, "Connection Error", socket->errorString());
}

void ChatWindow::on_sendButton_clicked(){
    QString msg = ui->messageInput->text().trimmed();

    if(msg.isEmpty())
        return;

    // qDebug() << "Sending message:" << msg;

    socket->write(msg.toUtf8());

    ui->messageInput->clear();
}


void ChatWindow::on_leavechatButton_clicked()
{
    socket->write("/exit\n");
    socket->flush();

    socket->disconnectFromHost();
    socket->close();

    if(mainwindowPtr)
    {
        mainwindowPtr->findChild<QLineEdit*>("nicknameInput")->clear();
        mainwindowPtr->show();
    }
    this->close();
}


