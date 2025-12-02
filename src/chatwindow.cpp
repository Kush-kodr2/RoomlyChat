#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QMessageBox>

ChatWindow::ChatWindow(QString username, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}
