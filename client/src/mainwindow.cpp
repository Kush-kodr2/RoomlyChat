#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatwindow.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->nicknameInput->setFocus();
    setWindowTitle("Roomly Chat Home");
    connect(ui->nicknameInput, &QLineEdit::returnPressed, this, &MainWindow::on_joinButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_joinButton_clicked()
{
    QString name = ui->nicknameInput->text();

    if(name.isEmpty()){
        QMessageBox::warning(this, "Error", "Please enter a nickname.");
        return;
    }

    ChatWindow *chat = new ChatWindow(name, this);
    chat -> show();
    this->hide();
}

