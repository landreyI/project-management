#ifndef QUSER_H
#define QUSER_H
#include <utility>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QTextEdit>
#include <QScrollArea>

class QUser : public QWidget
{
    Q_OBJECT
public:
    explicit QUser(QString nameC, QString passwordC, QUser *parent=nullptr);
    QUser(const QUser *other);
    void setListTask_Project(QList<std::pair<QString, QString>> *list){ listTask_Project = list; }
    void pushBackListTask_Project(QString task, QString project) {
        listTask_Project->push_back(std::make_pair(task, project));
    }

    void setListProject(QList<QString> *list){ listProject = list; }
    void pushBackListProject(QString project){ listProject->push_back(project); }
    QList<QString> *getListProject(){ return listProject; }
    QList<std::pair<QString,QString>> *getListTask_Project(){ return listTask_Project; }

    void setName(QString name){this->name->setText(name);}
    void setRole(QString role){ this->role->setText(role); }
    void setGmail(QString gmail){ this->gmail->setText(gmail); }
    void setPass(QString pass){this->password->setText(pass);}

    QString getName(){return name->text();}
    QString getRole(){return role->text();}
    QString getGmail(){return gmail->text();}
    QString getPass(){return password->text();}


    void setId(int id){ this->id= id; }
    int getId(){return id;}

    void setReadOnlyUser(bool check);
private:
    int id;
    QLineEdit *name;
    QLineEdit *password;
    QLineEdit *role;
    QLineEdit *gmail;
    QPushButton *delBut;
    QVBoxLayout *mainLayout;

    QList<std::pair<QString,QString>> *listTask_Project;
    QList<QString> *listProject;
    void addStyle();
    void infProjectTask(QVBoxLayout *mainLayout);
signals:
    void delObj(QUser *user);
};
#endif // QUSER_H
