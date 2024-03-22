#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlistwidget.h"
#include "ClientSocket.h"
#include <QMainWindow>
#include <QInputDialog>
#include <QDebug>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QSettings>

struct Message{
    QString expeditor;
    QString message;

    Message(QString expeditor, QString message) : expeditor(expeditor), message(message) {}
};

namespace Ui {
class MainWindow;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendMessage();
    void searchContact();
    void addContact();
    void populateContactList();
    void openConversationWindow(QListWidgetItem *item);
    void showFirstTimeSetupDialog();
    void InitializeUI();

    void handleMessageReceived(const QString &expeditor, const QString &message);
    void handleUiTitleUpdate(const QString &name);


private:
    Ui::MainWindow *ui;
    QList<QString> contacts;
    QSettings settings;

    ClientSocket *clientSocket;
    //map of contact name to list of messages
    QMap<QString, QList<Message>> messages;
};

#endif // MAINWINDOW_H
