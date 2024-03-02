#ifndef QPROJECT_H
#define QPROJECT_H
#include "QTask.h"
#include "QUser.h"
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
class QProject : public QWidget
{
    Q_OBJECT
public:
    explicit QProject(QString nameProjectC, QWidget *parent=nullptr);
    void setListUser(QList<QUser*> *listUser){this->listUser = listUser;}
    QList<QUser*> *getListUser(){return listUser;}
    void pushBackListUser(QUser *User){listUser->push_back(User);}
    void pushBackListTask(QTask *Task){listTask->push_back(Task);}
    void setListTask(QList<QTask*> *listTask){this->listTask = listTask;}
    QList<QTask*> *getListTask(){return listTask;}
    void hideSearch(QLayout *layout);
    void setDescription(QString desc){descriptionProject->setText(desc);}
    void setName(QString name){nameProject->setText(name);}
    void setStart(QString start){this->start->setText(start);}
    void setDeadline(QString deadline){this->deadline->setText(deadline);}
    QString getName(){return nameProject->text();}
    QString getDescription(){return descriptionProject->toPlainText();}
    QString getStart(){return start->text();}
    QString getDeadline(){return deadline->text();}
    void addListUser(QList<QUser*> *listUser);
    void addListTask(QList<QTask*> *listTask);

    void setReadOnlyProject(bool check);

    void setID(int id){this->id=id;}
    int getID(){return id;}
private:
    QLineEdit *start;
    QLineEdit *deadline;
    QString *saveName;
    QLineEdit *nameProject;
    QTextEdit *descriptionProject;
    QList<QTask*> *listTask;
    QList<QUser*> *listUser;
    QVBoxLayout *UserTaskSearch;
    QVBoxLayout *AddTaskOrUser;
    QList<QPushButton*> *listBut;
    QPushButton *AddTask;
    QPushButton *AddUser;
    QHBoxLayout *mainLayout;
    QPushButton *save;
    int id;


    void addStyle();

    void hideLayout(QLayout *layout);
    void showLayout(QLayout *layout);
    void delLayout(QLayout *layout);
    void hideButton(QList<QPushButton*> *listBut);
    void showButton(QList<QPushButton*> *listBut);
private slots:
    void addSeacrh(QHBoxLayout *layout);
    void addTask(QTask *task);
signals:
    delete_obj(QProject *obj);
    disconAddBut();
};
#endif // QPROJECT_H
