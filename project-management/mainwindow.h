#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QProject.h"
#include <QtSql>
#include <QMessageBox>
#include <QScrollArea>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<QProject*> *projects;
    QVBoxLayout* changeLayout;
    QVBoxLayout *editUserLog;
    QPushButton *butEdit;
    QUser *logUser = nullptr;
    QSqlDatabase *dataBase;

public slots:
    void handleLogin(const QString& email, const QString& password, const QString &name, const QString &role, const int &id, QSqlDatabase *dataBase);
private slots:
    void on_change_clicked();
    void on_del_clicked();
    void on_add_clicked();

    void TaskUserQuery(QProject *project, QUser *user);
    void UserProjectsQuery(QUser *user);
    void UserTasksQuery(QUser *user);

    void search();

    //menu action
    void save();
    void updateBd();
    void delUser();
    void editUser();
signals:

};

#endif // MAINWINDOW_H
