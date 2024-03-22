#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        settings("IN204", "ChatApp") {

    if (!settings.contains("username")) {

        showFirstTimeSetupDialog();

    } else {

        InitializeUI();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

//fonctionnalites de base implementees mais y a des ameliorations a faire

void MainWindow::InitializeUI() {

    clientSocket = new ClientSocket(this);
    connect(clientSocket, &ClientSocket::messageReceived, this, &MainWindow::handleMessageReceived);
    connect(clientSocket, &ClientSocket::updateUiTitle, this, &MainWindow::handleUiTitleUpdate);

    clientSocket->start();

    ui->setupUi(this);

    // Appliquer une feuille de style à l'ensemble de l'application ou à la fenêtre principale
    qApp->setStyleSheet(
            "QWidget { background-color: #E0E0E0; } QPushButton { background-color: #0084FF; color: white; border-radius: 5px; } QListWidget { background-color: #F0F0F0; color: black; } QListWidget::item:selected { background-color: #D0D0D0; }");

    this->setStyleSheet("QWidget { background-color: #F0F0F0; }");

    // Styliser
    ui->chatWidget->setStyleSheet("background-color: #FFFFFF; border: 1px solid #CCCCCC; border-radius: 5px;");

    ui->contactNameLabel->setStyleSheet("background-color: #F0F0F0; color: #4A4A4A; font-weight: bold; padding: 10px;");

    ui->messageLineEdit->setStyleSheet(
            "background-color: #FFFFFF; border: 1px solid #CCCCCC; border-radius: 5px; padding: 5px;");

    ui->searchButton->setIcon(QIcon("../icons/recherche.svg"));
    ui->searchButton->setStyleSheet(
            "QPushButton:pressed { background-color: #333333; }""QPushButton {icon-size: 20px; background-color: #22CB5D; color: white; border-radius: 5px; padding: 5px; }");

    ui->sendButton->setIcon(QIcon("../icons/envoie.svg"));
    ui->sendButton->setStyleSheet(
            "QPushButton:pressed { background-color: #333333; }""QPushButton {icon-size: 20px; background-color: #22CB5D; color: white; border-radius: 5px; padding: 5px; }");

    ui->addButton->setIcon(QIcon("../icons/ajout.svg"));
    ui->addButton->setStyleSheet(
            "QPushButton:pressed { background-color: #333333; }""QPushButton {icon-size: 20px; background-color: #22CB5D; color: white; border-radius: 5px; padding: 5px; }");
    ui->addButton->setMaximumWidth(250);

    ui->sidebarWidget->setStyleSheet("background-color: #128C7E;");
    ui->sidebarWidget->setMaximumWidth(250);

    ui->searchLineEdit->setStyleSheet(
            "background-color: #FFFFFF; border: 1px solid #128C7E; border-radius: 10px; padding: 5px;");

    ui->contactList->setStyleSheet(
            "QListWidget { background-color: #FFFFFF; color: #000000; } QListWidget::item { border-radius: 10px; padding: 10px; border: 1px solid black; } QListWidget::item:selected { background-color: #D0D0D0; }");

    ui->statusBar->setStyleSheet("background-color: #CCDDFF;");

    ui->chatDisplay->setStyleSheet(
            "QListWidget {""background-image: url('../icons/background.jpg');""}""QListWidget::item {""border: 1px solid #a2a2a2;""border-radius: 15px;""margin-top: 2px;""margin-bottom: 2px;""padding: 5px 10px;""background-color: #ffffff;""}""QListWidget::item:selected {""background-color: #d0d0d0;""}");

    // Connecter le signal clicked de chosir une personne au slot openConversationWindow
    connect(ui->contactList, &QListWidget::itemClicked, this, &MainWindow::openConversationWindow);

    // Connecter le signal clicked du bouton d'envoi au slot sendMessage
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);

    // Connecter le signal clicked du bouton de recherche au slot searchContact
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::searchContact);

    // Connecter le signal clicked du bouton d'ajout au slot addContact
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::addContact);

    populateContactList();
}

void MainWindow::sendMessage() {
    QString messageText = ui->messageLineEdit->text().trimmed();

    if (!messageText.isEmpty()) {
        QListWidgetItem *newItem = new QListWidgetItem();
        newItem->setText(messageText);
        newItem->setTextAlignment(Qt::AlignRight);
        newItem->setFlags(newItem->flags() & ~Qt::ItemIsSelectable);

        newItem->setData(Qt::UserRole + 1, true);

        ui->chatDisplay->addItem(newItem);
        ui->chatDisplay->scrollToBottom();
        ui->messageLineEdit->clear();
        clientSocket->sendMessage(ui->contactNameLabel->text().toStdString(), messageText.toStdString());
        messages[ui->contactNameLabel->text()].append(
                Message(QString::fromStdString(clientSocket->getName()), messageText));
    }
}

void MainWindow::searchContact() {
    QString searchTerm = ui->searchLineEdit->text();
    QList<QString> filteredContacts;

            foreach (const QString &contact, contacts) {
            if (contact.contains(searchTerm, Qt::CaseInsensitive)) {
                filteredContacts.append(contact);
            }
        }

    ui->contactList->clear();
            foreach (const QString &contactName, filteredContacts) {
            ui->contactList->addItem(contactName);
        }
}

void MainWindow::addContact() {
    bool ok;
    QString contactName = QInputDialog::getText(this, tr("Add Contact"), tr("Contact Name:"), QLineEdit::Normal, "",
                                                &ok);
    if (ok && !contactName.isEmpty()) {
        contacts.append(contactName);
        populateContactList();

        clientSocket->askPublicKey(contactName.toStdString());
    }
}

void MainWindow::populateContactList() {
    ui->contactList->clear();
            foreach (const QString &contactName, contacts) {
            QListWidgetItem *item = new QListWidgetItem(
                    QIcon("../icons/icon.svg"),
                    contactName);
            ui->contactList->addItem(item);
        }
}

void MainWindow::openConversationWindow(QListWidgetItem *item)
{
    QString contactName = item->text();
    ui->contactNameLabel->setText(contactName);
    ui->chatDisplay->clear();
    QList<Message> messageList = messages[contactName];
            foreach (const Message &message, messageList) {
            QListWidgetItem *newItem = new QListWidgetItem();
            newItem->setText(message.message);
            ui->chatDisplay->addItem(newItem);
        }
}


void MainWindow::handleMessageReceived(const QString &expeditor, const QString &message) {
    //Add Contact if not already in list
    if (messages.find(expeditor) == messages.end()) {
        contacts.append(expeditor);
        populateContactList();
    }

    messages[expeditor].append(Message(expeditor, message));

    //find QListWidgetItem with contactName
    QList<QListWidgetItem *> items = ui->contactList->findItems(expeditor, Qt::MatchExactly);
    if (items.size() > 0) {
        ui->contactList->setCurrentItem(items[0]);
    } else {
        ui->contactList->addItem(expeditor);
        ui->contactList->setCurrentRow(ui->contactList->count() - 1);
    }

    //open conversation
    openConversationWindow(ui->contactList->currentItem());

    std::cout << "Message received from maint Thread " << message.toStdString() << std::endl;
}

void MainWindow::showFirstTimeSetupDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Bienvenue !");
    QFormLayout form(&dialog);

    QLineEdit *usernameLineEdit = new QLineEdit(&dialog);
    QLineEdit *pinLineEdit = new QLineEdit(&dialog);
    pinLineEdit->setEchoMode(QLineEdit::Password);

    form.addRow("Username:", usernameLineEdit);
    form.addRow("PIN:", pinLineEdit);

    QPushButton *okButton = new QPushButton("OK", &dialog);
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    form.addRow(okButton);

    if (dialog.exec() == QDialog::Accepted) {
        settings.setValue("username", usernameLineEdit->text());
        settings.setValue("pin", pinLineEdit->text());
        settings.sync();


        if (settings.status() != QSettings::NoError) {
            std::cout << "Failed to write to the registry. Check application permissions." << std::endl;
        } else {
            std::cout << "Successfully wrote to the registry." << std::endl;
            settings.remove("testKey");
            settings.sync();
        }

        InitializeUI();


    }
}

void MainWindow::handleUiTitleUpdate(const QString &name) {
    //get windows title and add (name)
    QString title = this->windowTitle();
    title += " (" + name + ")";
    this->setWindowTitle(title);

}
