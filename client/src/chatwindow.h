#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QString>
#include <QTcpSocket>

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QString username ,QWidget *parent = nullptr);
    ~ChatWindow();

private slots:
    void on_sendButton_clicked();
    void onSocketReadyRead();
    void onSocketConnected();
    void onSocketErrorOccured(QAbstractSocket::SocketError socketError);

    void on_leavechatButton_clicked();

private:
    Ui::ChatWindow *ui;
    QTcpSocket *socket;
    QString username;
    QWidget *mainwindowPtr;
};

#endif // CHATWINDOW_H
