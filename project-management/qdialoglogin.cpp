#include "qdialoglogin.h"
#include <QMessageBox>
#include <QtSql>
QDialogLogin::QDialogLogin(QWidget* parent):QDialog(parent)
{
    QVBoxLayout *mainLayout =new QVBoxLayout(this);
    this->setMinimumSize(250,100);

    QPushButton* newLog=new QPushButton("Зберегти");
    newLog->setStyleSheet("font-size: 12px;");
    newLog->setMaximumSize(200,30);
    newLog->hide();
    QUser *user=new QUser("","");
    user->hide();
    mainLayout->addWidget(user);
    mainLayout->addWidget(newLog);

    formlayout=new QFormLayout;
    QLineEdit *gmail=new QLineEdit;
    gmail->setPlaceholderText("Введіть пошту...");
    QLineEdit *password=new QLineEdit;
    password->setPlaceholderText("Введіть пароль...");

    formlayout->addRow("Пошта",gmail);
    formlayout->addRow("Пароль",password);

    but=new QPushButton("ОК");
    but->setStyleSheet("font-size: 10px;");
    but->setMaximumSize(48,30);
    butNewUser=new QPushButton("Зараєструватися");
    butNewUser->setStyleSheet("font-size: 10px;");
    butNewUser->setMaximumSize(170,30);
    formlayout->addRow(but,butNewUser);
    mainLayout->addLayout(formlayout);

    connect(but, &QPushButton::clicked, [=](){
        loginSuccessful(gmail->text(),password->text());
    });

    connect(butNewUser, &QPushButton::clicked, [this, user, newLog](){
        this->resize(420,400);
        int itemCount = formlayout->count();
        for (int i = 0; i < itemCount; ++i) {
            QLayoutItem* item = formlayout->itemAt(i);
            if (QWidget* widget = item->widget()) {
                widget->setVisible(false); // Скрыть виджет
            }
        }
        user->show();
        newLog->show();
        connect(newLog, &QPushButton::clicked, [=](){
            QSqlQuery query;
            QString queryStr = "INSERT INTO \"User\" (Name, Password, Role, Email)"
                               "VALUES (:name, :password, :role, :email);";
            query.prepare(queryStr);
            query.bindValue(":name", user->getName());
            query.bindValue(":password", user->getPass());
            query.bindValue(":role", user->getRole());
            query.bindValue(":email", user->getGmail());
            if (!query.exec()) {
                QSqlError error = query.lastError();
                if (error.nativeErrorCode() == "23505") {
                    qDebug() << "Ошибка: Пользователь с такой электронной почтой уже существует.";
                    QMessageBox::critical(nullptr, "Ошибка", "Пользователь с такой электронной почтой уже существует!");
                } else {
                    qDebug() << "Ошибка при создании пользователя:" << error.text();
                    QMessageBox::critical(nullptr, "Ошибка", "Возникла ошибка при создании пользователя!");
                }
            }
            else
            {
                loginSuccessful(user->getGmail(),user->getPass());
            }
        });
    });

    connect(user, &QUser::delObj, [=](){
        user->hide();
        newLog->hide();
        int itemCount = formlayout->count();
        for (int i = 0; i < itemCount; ++i) {
            QLayoutItem* item = formlayout->itemAt(i);
            if (QWidget* widget = item->widget()) {
                widget->setVisible(true);
            }
        }
        this->resize(250,110);
    });

    this->setStyleSheet("QFormLayout {"
                        "    background-color: #f2f2f2;"
                        "    border: 1px solid #ccc;"
                        "    padding: 10px;"
                        "}"
                        "QLabel {"
                        "    color: black;"
                        "    font-size: 16px;"
                        "    font-weight: bold;"
                        "}"
                        "QLineEdit {"
                        "    background-color: #888888;"
                        "    padding: 5px;"
                        "    border: 1px solid #111111;"
                        "    border-radius: 4px;"
                        "}");
}
