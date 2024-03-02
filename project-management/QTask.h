#ifndef QTASK_H
#define QTASK_H

#include <QLineEdit>
#include <QTextEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QMenu>

class MyFormLayout:public QFormLayout
{
public:
    void addRow(const QString &label, QWidget *line, QWidget *but)
    {

        QFormLayout::insertRow(-1,label,line);
        QFormLayout::insertRow(-1,but);
        but->setMaximumWidth(20);
    }
    void addRow(const QString &label, QWidget *line)
    {
        QFormLayout::addRow(label,line);
    }
};

class QTask : public QWidget
{
    Q_OBJECT
public:
    explicit QTask(QString nameTaskC, QString priorityC, QString StartTaskC, QString DeadlineC, QWidget *parent=nullptr);
    explicit QTask(QString nameTaskC, QString priorityC, QString StartTaskC, QString DeadlineC, QString CompletedC, QWidget *parent=nullptr);
    QTask(const QTask *obj);

    QTask* getObj(){return this;}
    QString getName(){return nameTask->text();}
    QString getProgect_Name(){return Progect_Name;}
    QString getStartTask(){return StartTask->text();}
    QString getDeadLine(){return Deadline->text();}
    QString getDescription(){return descriptionTask->toPlainText();}
    QString getCompleted(){return Completed->text();}
    QString getUser_Name(){return User_Name->text();}
    QString getPriority(){return prEdit->text();}
    void setUser_Name(QString user){User_Name->setText(user);}
    void setDescription(QString desc){descriptionTask->setText(desc);}

    void setId(int id){ this->id= id; }
    int getId(){return id;}

    void setIdUser(int id){ this->idUser= id; }
    int getIdUser(){return idUser;}

    QString getUserNameStr(){return userName;}
    void setUserNameStr(QString user){userName = user;}

    void setReadOnlyTask(bool check);
private:

    MyFormLayout* formLayout;
    QString Progect_Name;
    QString userName;
    QLineEdit *User_Name;
    QLineEdit *prEdit;
    QLineEdit *nameTask;
    QPushButton *del_Task;

    QTextEdit *descriptionTask;
    QLineEdit *StartTask;
    QLineEdit *Deadline;
    QLineEdit *Completed;
    QVBoxLayout *mainLayout;

    int id;
    int idUser;


    void addStyle();
    void addPriorityMenu(QPushButton *but);
    private slots:

    signals:
        void delObj(QTask *task);
};
#endif // QTASK_H
