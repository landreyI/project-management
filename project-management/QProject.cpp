#include "QProject.h"
#include <QFormLayout>
#include <QMessageBox>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QtSql>
QProject::QProject(QString nameProjectC, QWidget *parent):QWidget(parent)
{
    this->setMinimumSize(500,300);
    this->setMaximumSize(1000,400);

    listTask=new QList<QTask*>;
    listUser=new QList<QUser*>;

    saveName = new QString(nameProjectC);

    mainLayout = new QHBoxLayout(this);

    save = new QPushButton("Зберегти");
    save->setMaximumHeight(50);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QFormLayout *formLayout = new QFormLayout;

    nameProject = new QLineEdit(nameProjectC);
    nameProject->setPlaceholderText("Напишіть...");
    descriptionProject = new QTextEdit;
    descriptionProject->setPlaceholderText("Опис...");

    start =new QLineEdit;
    start->setPlaceholderText("Напишіть...");
    deadline = new QLineEdit;
    deadline->setPlaceholderText("Напишіть...");

    formLayout->addRow("Назва", nameProject);
    formLayout->addRow("Початок", start);
    formLayout->addRow("Дедлайн", deadline);

    vBoxLayout->addWidget(save);
    vBoxLayout->addLayout(formLayout);
    vBoxLayout->addWidget(descriptionProject);

    mainLayout->addLayout(vBoxLayout);

    addSeacrh(mainLayout);

    QGridLayout *butLayout=new QGridLayout;

    QPushButton *Search=new QPushButton("Пошук");
    AddTask=new QPushButton("Додати задачу");
    AddUser=new QPushButton("Додати робітника");
    QPushButton *infTask=new QPushButton("Подивитися всі задачі");
    QPushButton *infUser=new QPushButton("Подивитися всіх робітників");

    butLayout->addWidget(Search,0,0);
    butLayout->addWidget(AddTask,1,0);
    butLayout->addWidget(AddUser,1,1);
    butLayout->addWidget(infTask,2,0);
    butLayout->addWidget(infUser,2,1);
    mainLayout->addLayout(butLayout);

    listBut=new QList<QPushButton*>{Search, AddTask, AddUser, infTask, infUser};

    AddTaskOrUser=new QVBoxLayout;
    mainLayout->addLayout(AddTaskOrUser);

    connect(Search, &QPushButton::clicked, [this, Search](){
            hideButton(listBut);
            showLayout(UserTaskSearch->layout());
    });

    connect(AddTask, &QPushButton::clicked, [this](){
        hideButton(listBut);
        showLayout(AddTaskOrUser->layout());
        addTask(new QTask("","","",""));
    });

    connect(AddUser, &QPushButton::clicked, [this](){
        hideButton(listBut);
        showLayout(AddTaskOrUser->layout());

        QLineEdit *gmailEdit=new QLineEdit;
        gmailEdit->setPlaceholderText("Напишіть...");

        QFormLayout *form=new QFormLayout;
        form->addRow("Пошта",gmailEdit);

        QPushButton *agree=new QPushButton("Додати");
        AddTaskOrUser->addWidget(agree);
        AddTaskOrUser->addLayout(form);

        connect(agree, &QPushButton::clicked,[=](){
            QSqlQuery query;
            query.prepare("SELECT * FROM \"User\" WHERE Email = ?");
            query.addBindValue(gmailEdit->text());

            if (query.exec())
            {
                if (query.next())
                {
                    QUser *user= new QUser(query.value("Name").toString(),query.value("Password").toString());
                    user->pushBackListProject(nameProject->text());
                    user->setGmail(gmailEdit->text());
                    user->setRole(query.value("Role").toString());
                    user->setId(query.value("UserID").toInt());

                    query.clear();
                    query.prepare("INSERT INTO ProjectUser (ProjectID, UserID) VALUES (:projectId, :userId)");

                    query.bindValue(":projectId", this->id);
                    query.bindValue(":userId", user->getId());

                    if (query.exec()) {
                        listUser->push_back(user);
                    }
                    else
                    {
                        qDebug() << "Ошибка при добавлении записи в таблицу ProjectUser:" << query.lastError().text();
                        QMessageBox::critical(nullptr, "Помилка", "Цей користувач вже є у проекті!");
                    }
                }
                else
                {
                    QMessageBox::critical(nullptr, "Помилка", "Користувача з поштою "+gmailEdit->text()+" - не існує!");
                }
            }
            else
            {
                QMessageBox::critical(nullptr, "Помилка", "При обробці запросу у БД!");
            }
            query.finish();

            while (QLayoutItem* item = form->takeAt(0)) {
                if (QWidget* widget = item->widget()) {
                    AddTaskOrUser->removeWidget(widget);
                    delete widget;
                }
                delete item;
            }

            AddTaskOrUser->removeWidget(agree);
            delete form;
            delete agree;
            showButton(listBut);
        });
    });

    connect(this, &QProject::disconAddBut, [=](){
        disconnect(AddTask, &QPushButton::clicked, 0, 0);
        disconnect(AddUser, &QPushButton::clicked, 0, 0);
    });

    connect(infTask, &QPushButton::clicked, [this](){
        hideButton(listBut);
        QTextEdit *label=new QTextEdit;

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidget(label);
        scrollArea->setWidgetResizable(true);

        QPushButton *hideBut=new QPushButton("Назад");
        QVBoxLayout *inf=new QVBoxLayout;
        if(!listTask->isEmpty())
        {
            for(int i=0; i<listTask->size(); i++)
            {
                label->append("Задача - "+listTask->at(i)->getName()+" (Початок: "+listTask->at(i)->getStartTask()+
                              ", Дедлайн: "+listTask->at(i)->getDeadLine()+", Виконано: "+listTask->at(i)->getCompleted()+") Виконавець - "+listTask->at(i)->getUser_Name());
            }
        }
        inf->addWidget(hideBut);
        inf->addWidget(scrollArea);
        mainLayout->addLayout(inf);
        connect(hideBut, &QPushButton::clicked,[=](){
            delete hideBut;
            delete label;
            delete scrollArea;
            delete inf;
            showButton(listBut);
        });
    });

    connect(infUser, &QPushButton::clicked, [this](){
        hideButton(listBut);
        QTextEdit *label=new QTextEdit;

        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidget(label);
        scrollArea->setWidgetResizable(true);

        QPushButton *hideBut=new QPushButton("Назад");
        QVBoxLayout *inf=new QVBoxLayout;
        if(!listUser->isEmpty())
        {
            for(int i=0; i<listUser->size(); i++)
            {
                label->append(listUser->at(i)->getName()+" - Роль: "+listUser->at(i)->getRole());
            }
        }
        inf->addWidget(hideBut);
        inf->addWidget(scrollArea);
        mainLayout->addLayout(inf);
        connect(hideBut, &QPushButton::clicked,[=](){
            delete hideBut;
            delete label;
            delete scrollArea;
            delete inf;
            showButton(listBut);
        });
    });

    connect(save, &QPushButton::clicked, [this](){
        bool check=false;
        for(int i=0; i<listTask->size(); i++)
        {
            if(listTask->at(i)->getUserNameStr()!=listTask->at(i)->getUser_Name())
            {
                for(int j=0; j<listUser->size(); j++)
                {
                    if(listUser->at(j)->getName() == listTask->at(i)->getUser_Name())
                    {
                        listTask->at(i)->setUserNameStr(listTask->at(i)->getUser_Name());
                        listTask->at(i)->setIdUser(listUser->at(j)->getId());
                        check=true;
                    }
                }
                if(!check)
                {
                    listTask->at(i)->setUser_Name(listTask->at(i)->getUserNameStr());
                }
            }
        }

        for(int i=0; i<listUser->size(); i++)
        {
            QList<std::pair<QString,QString>> *listTask_Project = listUser->at(i)->getListTask_Project();
            for(int i=0; i<listTask_Project->size();i++)
            {
                if(listTask_Project->at(i).second == *saveName)
                {
                    QString second = nameProject->text();
                    QString first = listTask_Project->at(i).first;
                    listTask_Project->removeAt(i);
                    listTask_Project->insert(i, std::pair(first,second));
                }
            }

            QList<QString> *listProject = listUser->at(i)->getListProject();
            for(int i=0; i<listProject->size();i++)
            {
                if(listProject->at(i) == *saveName)
                {
                    QString nameP = nameProject->text();
                    listProject->removeAt(i);
                    listProject->insert(i, nameP);
                }
            }
        }
        *saveName = nameProject->text();
    });

    addStyle();
}

void QProject::addStyle()
{
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
                        "}"
                        "QTextEdit {"
                        "    background-color: #888888;"
                        "    padding: 5px;"
                        "    border: 1px solid #111111;"
                        "    border-radius: 4px;}"
                        "QPushButton {font-size: 16px;}");
}

void QProject::addSeacrh(QHBoxLayout *mainLayout)
{
    QLineEdit *searchUser=new QLineEdit();
    searchUser->setPlaceholderText("Напишіть фамілію...");
    QLineEdit *searchTask=new QLineEdit();
    searchTask->setPlaceholderText("Напишіть назву задачі...");

    QHBoxLayout *Users=new QHBoxLayout;
    QHBoxLayout *Tasks=new QHBoxLayout;

    QPushButton *butSearchUser = new QPushButton("Шукати");
    QPushButton *butSearchTask = new QPushButton("Шукати");
    butSearchUser->setMaximumHeight(30);
    butSearchTask->setMaximumHeight(30);

    Users->addWidget(searchUser);
    Users->addWidget(butSearchUser);

    Tasks->addWidget(searchTask);
    Tasks->addWidget(butSearchTask);

    UserTaskSearch=new QVBoxLayout;

    QPushButton *hideBut=new QPushButton("Назад");

    UserTaskSearch->addWidget(hideBut);
    hideBut->setMaximumWidth(130);

    UserTaskSearch->addLayout(Users);
    UserTaskSearch->addLayout(Tasks);

    mainLayout->addLayout(UserTaskSearch);
    hideLayout(UserTaskSearch->layout());

    connect(butSearchUser, &QPushButton::clicked, this, [=]() {
        hideBut->hide();
        QPushButton *show=new QPushButton("Скрити");
        show->setMaximumSize(120,40);
        QUser *user=new QUser("","");

        bool off=false;

        for(int i=0; i<listUser->size(); i++)
        {
            if(listUser->at(i)->getName()==searchUser->text())
            {
                butSearchUser->hide();
                butSearchTask->hide();
                searchTask->hide();
                searchUser->hide();

                Users->addWidget(listUser->at(i));
                user=listUser->at(i);
                user->show();
                UserTaskSearch->addWidget(show);
                off=true;
            }
        }
        if(!off)
        {
            QMessageBox::critical(nullptr, "Помилка", "Такого користувача не знайдено!");
            hideBut->show();
        }
        connect(user, QUser::delObj, [=](){
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Видалення", "Ви точно хочите видалити?", QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                QSqlQuery query;
                query.prepare("DELETE FROM ProjectUser WHERE UserID = :userID");
                query.bindValue(":userID", user->getId());
                if (!query.exec()) {
                    qDebug() << "Ошибка при удалении записей из таблицы ProjectUser:" << query.lastError().text();
                    QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при видаленні!");
                }
                else
                {
                    listUser->removeOne(user);
                    UserTaskSearch->removeWidget(show);
                    show->deleteLater();
                    mainLayout->removeWidget(user);

                    butSearchUser->show();
                    butSearchTask->show();
                    searchTask->show();
                    searchUser->show();
                    hideBut->show();
                    delete user;
                }
                query.finish();
            }
        });

        connect(show, &QPushButton::clicked, [=](){
            UserTaskSearch->removeWidget(show);
            show->deleteLater();
            Users->removeWidget(user);
            UserTaskSearch->removeWidget(user);
            user->hide();

            butSearchUser->show();
            butSearchTask->show();
            searchTask->show();
            searchUser->show();
            hideBut->show();
        });
    }); //Знайти користувача

    connect(butSearchTask, &QPushButton::clicked, this, [=]() {
        hideBut->hide();
        QPushButton *show=new QPushButton("Скрити");
        show->setMaximumSize(120,40);
        QTask *task=new QTask("","","","");

        bool off=false;

        for(int i=0; i<listTask->size(); i++)
        {
            if(listTask->at(i)->getName()==searchTask->text())
            {
                butSearchUser->hide();
                butSearchTask->hide();
                searchTask->hide();
                searchUser->hide();

                Tasks->addWidget(listTask->at(i));
                task=listTask->at(i);
                task->show();
                UserTaskSearch->addWidget(show);
                off=true;
            }
        }
        if(!off)
        {
            QMessageBox::critical(nullptr, "Помилка", "Такої задачі не існує");
            hideBut->show();
        }
        connect(task, QTask::delObj, [=](){
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Видалення", "Ви точно хочите видалити?", QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                QSqlQuery query;
                query.prepare("DELETE FROM Task WHERE TaskID = :taskID");
                query.bindValue(":taskID", task->getId());
                if (!query.exec()) {
                    qDebug() << "Ошибка при удалении записей из таблицы Task:" << query.lastError().text();
                    QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при видаленні!");
                }
                else
                {
                    listTask->removeOne(task);
                    UserTaskSearch->removeWidget(show);
                    show->deleteLater();
                    mainLayout->removeWidget(task);

                    butSearchUser->show();
                    butSearchTask->show();
                    searchTask->show();
                    searchUser->show();
                    hideBut->show();
                    delete task;
                }
                query.finish();
            }
        });

        connect(show, &QPushButton::clicked, [=](){
            UserTaskSearch->removeWidget(show);
            show->deleteLater();
            Tasks->removeWidget(task);
            UserTaskSearch->removeWidget(task);
            task->hide();

            butSearchUser->show();
            butSearchTask->show();
            searchTask->show();
            searchUser->show();
            hideBut->show();
        });
    }); //Знайти задачу

    connect(hideBut, &QPushButton::clicked, [this](){
        hideLayout(UserTaskSearch->layout());
        showButton(listBut);
    });
}

void QProject::addTask(QTask *taskRec)
{
    QPushButton *agree=new QPushButton("Зберегти");
    AddTaskOrUser->addWidget(agree);
    AddTaskOrUser->addWidget(taskRec);

    connect(agree, &QPushButton::clicked,[=](){

            QTask* task = new QTask(taskRec);
            QSqlQuery query;


            if(task->getUser_Name()!="")
            {
                for(int i=0; i<listUser->size();i++)
                {
                    if(listUser->at(i)->getName()==task->getUser_Name())
                    {
                        task->setUserNameStr(task->getUser_Name());
                        task->setIdUser(listUser->at(i)->getId());
                    }
                }
            }

            QString queryStr = "INSERT INTO Task (Name, Description, Start, Deadline, ProjectID, UserID, Priority) "
                               "VALUES (:name, :description, TO_DATE(:start, 'YYYY-MM-DD'), TO_DATE(:deadline, 'YYYY-MM-DD'), :projectId, :userId, ";

            if (task->getPriority()=="") {
                queryStr += "DEFAULT)";
            } else {
                queryStr += ":priority)";
            }
            query.prepare(queryStr);
            query.bindValue(":name", task->getName());
            query.bindValue(":description", task->getDescription());
            query.bindValue(":start", task->getStartTask());
            query.bindValue(":deadline", task->getDeadLine());
            query.bindValue(":projectId", this->getID());
            query.bindValue(":userId", task->getIdUser());

            if (task->getPriority()!="") {
                query.bindValue(":priority", task->getPriority());
            }

            if (query.exec()) {
                for(int i=0; i<listUser->size();i++)
                {
                    if(listUser->at(i)->getName() == task->getUser_Name())
                    {
                        listUser->at(i)->pushBackListTask_Project(task->getName(), this->getName());
                    }
                }
                query.clear();
                query.exec("SELECT currval('task_taskid_seq') AS auto_increment");
                if (query.next()) {
                    int autoIncrementValue = query.value("auto_increment").toInt();
                    task->setId(autoIncrementValue);
                }
                else {
                    qDebug() << "Error executing query:" << query.lastError().text();
                }

                listTask->push_back(task);
            }
            else {
                qDebug()<< "Ошибка при обращении к Task - "+query.lastError().text();
                QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при створенні задачі!");
            }
            query.finish();

        AddTaskOrUser->removeWidget(taskRec);
        AddTaskOrUser->removeWidget(agree);

        delete agree;
        addTask(new QTask("","","",""));

        delete taskRec;
    });

    connect(taskRec, QTask::delObj,this,[=](){
        AddTaskOrUser->removeWidget(taskRec);
        AddTaskOrUser->removeWidget(agree);
        delete taskRec;
        delete agree;
        showButton(listBut);
    });
}

void QProject::hideLayout(QLayout *layout)
{
    if (!layout)
        return;

    int itemCount = layout->count();
    for (int i = 0; i < itemCount; ++i) {
        QLayoutItem* item = layout->itemAt(i);
        QWidget* widget = item->widget();

        if (widget) {
            widget->hide();
        } else {
            QLayout* childLayout = item->layout();
            if (childLayout) {
                hideLayout(childLayout); // Рекурсивный вызов для вложенного компоновщика
            }
        }
    }
}

void QProject::showLayout(QLayout *layout)
{
    if (!layout)
        return;

    int itemCount = layout->count();
    for (int i = 0; i < itemCount; ++i) {
        QLayoutItem* item = layout->itemAt(i);
        QWidget* widget = item->widget();

        if (widget) {
            widget->show();
        } else {
            QLayout* childLayout = item->layout();
            if (childLayout) {
                showLayout(childLayout); // Рекурсивный вызов для вложенного компоновщика
            }
        }
    }
}

void QProject::delLayout(QLayout *layout)
{
    if (!layout)
        return;

    int itemCount = layout->count();
    for (int i = 0; i < itemCount; ++i) {
        QLayoutItem* item = layout->itemAt(i);
        QWidget* widget = item->widget();

        if (widget) {
            delete widget;
        } else {
            QLayout* childLayout = item->layout();
            if (childLayout) {
                delLayout(childLayout); // Рекурсивный вызов для вложенного компоновщика
                delete childLayout; // Удаление вложенного компоновщика
            }
        }
    }
}

void QProject::hideButton(QList<QPushButton*> *listBut)
{
    for(int i=0; i<listBut->size(); i++)
    {
        listBut->at(i)->hide();
    }
}

void QProject::showButton(QList<QPushButton*> *listBut)
{
    for(int i=0; i<listBut->size(); i++)
    {
        listBut->at(i)->show();
    }
}

void QProject::addListUser(QList<QUser*> *listUser)
{
    for(int i=0; i<listUser->size();i++)
    {
        this->listUser->push_back(listUser->at(i));
    }
}

void QProject::addListTask(QList<QTask*> *listTask)
{
    for(int i=0; i<listTask->size();i++)
    {
        this->listTask->push_back(listTask->at(i));
    }
}

void QProject::setReadOnlyProject(bool check)
{
    if(check)
    {
        save->hide();
        start->setReadOnly(true);
        deadline->setReadOnly(true);
        nameProject->setReadOnly(true);
        descriptionProject->setReadOnly(true);

        for(int i=0; i<listUser->size(); i++)
        {
            listUser->at(i)->setReadOnlyUser(true);
        }
        for(int i=0; i<listTask->size(); i++)
        {
            listTask->at(i)->setReadOnlyTask(true);
        }
        disconAddBut();
    }
    else
    {
        AddTask->show();
        AddUser->show();
        save->show();
        start->setReadOnly(false);
        deadline->setReadOnly(false);
        nameProject->setReadOnly(false);
        descriptionProject->setReadOnly(false);

        for(int i=0; i<listUser->size(); i++)
        {
            listUser->at(i)->setReadOnlyUser(false);
        }
        for(int i=0; i<listTask->size(); i++)
        {
            listTask->at(i)->setReadOnlyTask(false);
        }
    }
}

