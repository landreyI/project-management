#include "mainwindow.h"
#include "qdialoglogin.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try {
        a.setStyleSheet("QWidget{background-color: #444444;}"
                        "QPushButton {"
                          "    background-color: gray;"
                          "    color: #333333;"
                          "    font-weight: bold;"
                          "    font-family: Arial, sans-serif;"
                          "    font-size: 24px;"
                          "    padding: 8px 16px;"
                          "    border-radius: 4px;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: darkgray;"
                          "}"
                          "QPushButton:pressed {"
                          "    background-color: lightgray;"
                          "}"
                          );
        QTranslator translator;
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "project_management_" + QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }

        qDebug() << QSqlDatabase::drivers();

        QString serverName = "DESKTOP-AS7H1S7";
        int port = 5432;
        QString dbName = "project_management";
        QString userName = "postgres";
        QString password = "20042011";

        // Установка параметров подключения
        QSqlDatabase *dataBase = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL"));
        dataBase->setHostName(serverName);
        dataBase->setPort(port);
        dataBase->setDatabaseName(dbName);
        dataBase->setUserName(userName);
        dataBase->setPassword(password);

        if(!dataBase->open()) {

            qDebug()<<"ERROR: "<<QSqlError(dataBase->lastError()).text();
            QMessageBox::critical(nullptr, "Помилка", "База даних не увімкнена!");
            return 0;
        }
        else
        {
            qDebug()<<"Ok";
        }

        QDialogLogin d;
        MainWindow w;

        QObject::connect(&d, &QDialogLogin::loginSuccessful, [&w, &d, dataBase](const QString& email, const QString& password){
            QSqlQuery query(*dataBase);
            query.prepare("SELECT * FROM \"User\" WHERE Email = ?");
            query.addBindValue(email);
            if (query.exec())
            {
                if (query.next())
                {
                    QString storedPassword = query.value("Password").toString();

                    if (password == storedPassword)
                    {
                        qDebug() << "Authentication successful for user:" << email;
                    }
                    else
                    {
                        QMessageBox::critical(nullptr, "Помилка", "Неправильний пароль!");
                        return 0;
                    }
                }
                else
                {
                    QMessageBox::critical(nullptr, "Помилка", "Користувача з поштою "+email+" - не існує!");
                    return 0;
                }
            }
            else
            {
                QMessageBox::critical(nullptr, "Помилка", "При обробці запросу у БД!");
                return 0;
            }
            QString username = query.value("Name").toString();
            QString role = query.value("Role").toString();
            int id = query.value("UserID").toInt();
            query.finish();
            d.hide();

            w.show();
            w.handleLogin(email, password, username, role, id, dataBase);

        });

        d.show();
        return a.exec();
    }
    catch (const std::exception& e) {
        qCritical() << "Исключение: " << e.what();
    } catch (...) {
        qCritical() << "Неизвестное исключение";
    }

    return -1;
}
