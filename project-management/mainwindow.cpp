#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString styleSheet = "QMenuBar { background-color: gray; }"
                         "QMenu { border: 1px solid #303030; }"
                         "QMenu { background-color: #505050; }"
                         "QMenu::item:selected { background-color: #404040; }"
                         "QMenu::item:disabled { color: #404040; }"
                         "QMenu { color: white; }";
    ui->menubar->setStyleSheet(styleSheet);

    QMenu *fileMenu = ui->menubar->addMenu("Файл");
    QMenu *editMenu = ui->menubar->addMenu("Акаунт");

    QAction *saveAction = new QAction("Зберегти", this);
    QAction *updateAction = new QAction("Оновити данні", this);
    QAction *changeAction = new QAction("Змінити акаунт", this);
    QAction *delAction = new QAction("Видалити свій акаунт", this);
    QAction *addAction = new QAction("Створити новий акаунт", this);
    QAction *editAction = new QAction("Редагувати свій акаунт", this);

    fileMenu->addAction(saveAction);
    fileMenu->addAction(updateAction);

    editMenu->addAction(changeAction);
    editMenu->addAction(delAction);
    editMenu->addAction(addAction);
    editMenu->addAction(editAction);

    ui->searchEdit->setPlaceholderText("Напишіть...");
    ui->searchEdit->setStyleSheet("QLineEdit {"
                                  "background-color: #888888;"
                                  "padding: 5px;"
                                  "border: 1px solid #111111;"
                                  "border-radius: 4px;"
                                  "}");

    projects=new QList<QProject*>;

    changeLayout = new QVBoxLayout;
    ui->gridLayout->addLayout(changeLayout, 1, 1);

    editUserLog=new QVBoxLayout;
    butEdit=new QPushButton("Зберегти");
    editUserLog->addWidget(butEdit);
    butEdit->hide();
    ui->gridLayout->addLayout(editUserLog, 2, 1);

    connect(ui->searchBut, &QPushButton::clicked, this, MainWindow::search);

    connect(saveAction, &QAction::triggered, this, &MainWindow::save);
    connect(updateAction, &QAction::triggered, this, &MainWindow::updateBd);
    connect(delAction, &QAction::triggered, this, &MainWindow::delUser);
    connect(addAction, &QAction::triggered, [this](){
        save();
        QCoreApplication::quit();
        QProcess::startDetached(QCoreApplication::applicationFilePath());
    });
    connect(changeAction, &QAction::triggered, [this](){
        save();
        QCoreApplication::quit();
        QProcess::startDetached(QCoreApplication::applicationFilePath());
    });
    connect(editAction, &QAction::triggered, this, &MainWindow::editUser);
}

MainWindow::~MainWindow()
{
    dataBase->close();
    delete ui;
}

void MainWindow::search()
{
    QString data = ui->searchEdit->text();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Пошук");
    msgBox.setText("<font color='white'>Виберіть потрібний пошук:</font>");
    msgBox.addButton("проєкт", QMessageBox::AcceptRole);
    msgBox.addButton("користувач", QMessageBox::RejectRole);

    QVBoxLayout *boxL=new QVBoxLayout;
    int result = msgBox.exec();
    if(!dataBase->isOpen())
    {
        dataBase->open();
    }
    if (result == QMessageBox::AcceptRole) //Project
    {
        QString getProjectsQuery =  "SELECT Project.* "
                                   "FROM Project "
                                   "WHERE Project.Name = :name";
        QSqlQuery Project_query;
        Project_query.prepare(getProjectsQuery);
        Project_query.bindValue(":name", data);
        if (Project_query.exec()) {
            while (Project_query.next()) {
                QProject *project=new QProject(Project_query.value("Name").toString());

                project->setDescription(Project_query.value("Description").toString());
                project->setID(Project_query.value("ProjectID").toInt());

                TaskUserQuery(project, new QUser("","")); //search Tasks and User
                project->setReadOnlyProject(true);

                boxL->addWidget(project);
            }
        }
        else
        {
            qDebug()<< "Ошибка при обращении к User - "+Project_query.lastError().text();
        }
        Project_query.finish();
    }
    else if (result == QMessageBox::RejectRole) //User
    {

        QString getUserssQuery =  "SELECT \"User\".* "
                                   "FROM \"User\" "
                                   "WHERE \"User\".Name = :name";

        QSqlQuery Users_query;
        Users_query.prepare(getUserssQuery);
        Users_query.bindValue(":name", data);

        if (Users_query.exec()) {
            while (Users_query.next()) {
                QUser *user=new QUser(Users_query.value("Name").toString(), Users_query.value("Password").toString());
                user->setMinimumSize(300,350);
                user->setGmail(Users_query.value("Email").toString());
                user->setRole(Users_query.value("Role").toString());
                user->setId(Users_query.value("UserID").toInt());

                UserProjectsQuery(user); //search projects
                UserTasksQuery(user); //search tasks

                user->setReadOnlyUser(true);
                boxL->addWidget(user);
            }
        }
        else
        {
            qDebug()<< "Ошибка при обращении к User - "+Users_query.lastError().text();
        }
        Users_query.finish();
    }
    QMainWindow *newWindow = new QMainWindow(this);
    newWindow->resize(1000,500);
    newWindow->show();
    QWidget *widget=new QWidget;
    widget->setLayout(boxL);
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    newWindow->setCentralWidget(scrollArea);
}

void MainWindow::handleLogin(const QString& email, const QString& password, const QString &username, const QString &role, const int &id, QSqlDatabase *dataBase)
{
    this->dataBase=dataBase;
    QString getProjectsQuery =  "SELECT Project.* "
                                "FROM Project "
                               "JOIN ProjectUser ON Project.ProjectID = ProjectUser.ProjectID "
                               "WHERE ProjectUser.UserID = :userID";

    QSqlQuery Projects_query;
    Projects_query.prepare(getProjectsQuery);
    Projects_query.bindValue(":userID", id);

    QUser *user=new QUser(username,password);
    user->setGmail(email);
    user->setRole(role);
    user->setId(id);

    if (logUser == nullptr) {
        logUser=new QUser(user);
        editUserLog->addWidget(logUser);
        logUser->hide();
    }

    if (Projects_query.exec()) {
        while (Projects_query.next()) { //НЕ РАБОТАЕТ ЦЫКЛ ПРИ ОБНОВЛЕНИИ ДАННЫХ
            QString ProjectName = Projects_query.value("Name").toString();
            user->pushBackListProject(ProjectName);

            QProject *pr=new QProject(ProjectName);
            pr->setID(Projects_query.value("ProjectID").toInt());
            pr->setStart(Projects_query.value("Start").toString());
            pr->setDeadline(Projects_query.value("Deadline").toString());
            pr->setDescription(Projects_query.value("Description").toString());
            pr->setID(Projects_query.value("ProjectID").toInt());

            TaskUserQuery(pr, user); //search Tasks and Users

            projects->push_back(pr);

            ui->gridLayout->addWidget(pr,1,0);
            pr->hide();
        }
    }
    else
    {
       qDebug()<< "Ошибка при обращении к Project - "+Projects_query.lastError().text();
    }
    Projects_query.finish();
    if(!projects->empty())
    projects->at(0)->show();
}

void MainWindow::on_change_clicked()
{
    QLayoutItem* item;
    while ((item = changeLayout->takeAt(0)) != nullptr) {
       delete item->widget();
       delete item;
    }
    // Добавьте новые виджеты в changeLayout
    for (int i = 0; i < projects->size(); i++) {
       if (projects->at(i)->isVisible()) {
            projects->at(i)->hide();
       }
       QPushButton *pushBut = new QPushButton(projects->at(i)->getName());
       changeLayout->addWidget(pushBut);
       connect(pushBut, &QPushButton::clicked, [i, this]() {
           projects->at(i)->show();
           QLayoutItem* item;
           while ((item = changeLayout->takeAt(0)) != nullptr) {
               delete item->widget();
               delete item;
           }
       });
    }
}

void MainWindow::on_del_clicked()
{
    if(projects->size()!=0)
    {
       QMessageBox::StandardButton reply;
       reply = QMessageBox::question(this, "Видалення", "Ви точно хочите видалити?", QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {
           for(int i=0; i<projects->size();i++)
           {
               if (projects->at(i)->isVisible()) {
                   QProject* widget = projects->at(i);
                   QSqlQuery query;
                   query.prepare("DELETE FROM Task WHERE ProjectID = :projectId");
                   query.bindValue(":projectId", widget->getID());
                   if (!query.exec()) {
                       qDebug() << "Ошибка при удалении из таблицы Task:" << query.lastError().text();
                           QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при відаленні!");
                   }
                   else
                   {
                        query.clear();
                        query.prepare("DELETE FROM ProjectUser WHERE ProjectID = :projectId");
                        query.bindValue(":projectId", widget->getID());
                        if (!query.exec()) {
                            qDebug() << "Ошибка при удалении из таблицы ProjectUser:" << query.lastError().text();
                                QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при відаленні!");
                        }
                        else
                        {
                            query.clear();
                            query.prepare("DELETE FROM Project WHERE ProjectID = :projectId");
                            query.bindValue(":projectId", widget->getID());
                            if (!query.exec()) {
                                qDebug() << "Ошибка при удалении из таблицы Project:" << query.lastError().text();
                                    QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при відаленні!");
                            }
                            else
                            {
                                ui->centralwidget->layout()->removeWidget(widget);
                                delete widget;
                                projects->removeAt(i);
                            }
                        }
                   }
               }
           }
       }
       if(!projects->empty())
       {
           projects->at(0)->show();
       }
    }
}

void MainWindow::on_add_clicked()
{
    for (int i = 0; i < projects->size(); i++) {
       if (projects->at(i)->isVisible()) {
           projects->at(i)->hide();
       }
    }
    QProject *newProject=new QProject("Новий проєкт");
    QUser *user =new QUser(logUser);
    user->pushBackListProject("Новий проєкт");
    newProject->pushBackListUser(user);

    QSqlQuery query;
    QString queryStr = "INSERT INTO Project (Name, Description, Start, Deadline) "
                       "VALUES (:name, :description, TO_DATE(:start, 'YYYY-MM-DD'), TO_DATE(:deadline, 'YYYY-MM-DD'));";
    query.prepare(queryStr);
    query.bindValue(":name", newProject->getName());
    query.bindValue(":description", newProject->getDescription());
    query.bindValue(":start", newProject->getStart());
    query.bindValue(":deadline", newProject->getDeadline());
    if (!query.exec()) {
       qDebug()<< "Ошибка при обращении к Project - "+query.lastError().text();
       QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при створенні проєкту!");
    }
    else
    {
       query.clear();
       query.exec("SELECT currval('project_projectid_seq') AS auto_increment");
       if (query.next()) {
           int autoIncrementValue = query.value("auto_increment").toInt();
           newProject->setID(autoIncrementValue);

           query.clear();
           query.prepare("INSERT INTO ProjectUser (ProjectID, UserID)"
                         "VALUES (:projectId, :userId);");
           query.bindValue(":projectId", newProject->getID());
           query.bindValue(":userId", logUser->getId());
           if (!query.exec()) {
               qDebug()<< "Ошибка при обращении к ProjectUser - "+query.lastError().text();
               QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при створенні проєкту!");
           }
           else
           {
               projects->push_back(newProject);

               ui->gridLayout->addWidget(newProject,0,0);
               projects->at(projects->size()-1)->show();
           }
       }
       else {
           qDebug() << "Error executing query:" << query.lastError().text();
       }
    }
}

void MainWindow::TaskUserQuery(QProject *pr, QUser *user)
{
    QList<QUser*> *listUser=new QList<QUser*>;
    QList<QTask*> *listTask=new QList<QTask*>;
    if(user->getName()!="")
    listUser->push_back(user);

    QString getUserQuery = "SELECT \"User\".* "
                           "FROM \"User\" "
                           "JOIN ProjectUser ON \"User\".UserID = ProjectUser.UserID "
                           "WHERE ProjectUser.ProjectID = :projectID";

    QSqlQuery userQuery;
    userQuery.prepare(getUserQuery);
    userQuery.bindValue(":projectID", pr->getID());

    if (userQuery.exec()) {
       while (userQuery.next()) {
           if(userQuery.value("Name").toString()!=user->getName())
           {
               QUser *findUser=new QUser(userQuery.value("Name").toString(),userQuery.value("Password").toString());
               findUser->setGmail(userQuery.value("Email").toString());
               findUser->setRole(userQuery.value("Role").toString());
               findUser->setId(userQuery.value("UserID").toInt());

               UserProjectsQuery(findUser); //search projects

               listUser->push_back(findUser);
           }
       }
    }
    else
    {
       qDebug()<< "Ошибка при обращении к User - "+userQuery.lastError().text();
    }
    userQuery.finish();

    QString getTasksQuery = "SELECT Task.* "
                            "FROM Task "
                            "WHERE "
                            "Task.ProjectID = :projectID";
    QSqlQuery tasksQuery;
    tasksQuery.prepare(getTasksQuery);
    tasksQuery.bindValue(":projectID", pr->getID());
    if (tasksQuery.exec()) {
       while (tasksQuery.next()) {
           QTask *task=new QTask(tasksQuery.value("Name").toString(), tasksQuery.value("Priority").toString(), tasksQuery.value("Start").toString(),
                                   tasksQuery.value("Deadline").toString(), tasksQuery.value("Completed").toString());
           task->setDescription(tasksQuery.value("Description").toString());
           task->setId(tasksQuery.value("TaskID").toInt());
           task->setIdUser(tasksQuery.value("UserID").toInt());
           for(int i=0; i<listUser->size();i++)
           {
               if(task->getIdUser()==listUser->at(i)->getId())
               {
                    task->setUser_Name(listUser->at(i)->getName());
                    task->setUserNameStr(listUser->at(i)->getName());
                    listUser->at(i)->pushBackListTask_Project(task->getName(), pr->getName());
               }
           }
           listTask->push_back(task);
       }
    }
    else
    {
       qDebug()<< "Ошибка при обращении к Task - "+tasksQuery.lastError().text();
    }
    tasksQuery.finish();
    pr->setListUser(listUser);
    pr->setListTask(listTask);
}

void MainWindow::UserProjectsQuery(QUser *user)
{
    QSqlQuery Projects_query;
    Projects_query.prepare("SELECT Project.* "
                           "FROM Project "
                           "JOIN ProjectUser ON Project.ProjectID = ProjectUser.ProjectID "
                           "WHERE ProjectUser.UserID = :userID");
    Projects_query.bindValue(":userID", user->getId());
    if (Projects_query.exec()) {
       while (Projects_query.next()) {
           user->pushBackListProject(Projects_query.value("Name").toString());
       }
    }
    else
    {
       qDebug()<< "Ошибка при обращении к Project - "+Projects_query.lastError().text();
    }
    Projects_query.finish();
}

void MainWindow::UserTasksQuery(QUser *user)
{
    QSqlQuery Tasks_query;
    Tasks_query.prepare("SELECT Task.Name, Project.Name "
                        "FROM Task "
                        "JOIN Project ON Task.ProjectID = Project.ProjectID "
                        "WHERE Task.UserID = :userID");

    Tasks_query.bindValue(":userID", user->getId());
    if (Tasks_query.exec()) {
       while (Tasks_query.next()) {
           user->pushBackListTask_Project(Tasks_query.value("Task.Name").toString(),Tasks_query.value("Project.Name").toString());
       }
    }
    else
    {
       qDebug()<< "Ошибка при обращении к Task - "+Tasks_query.lastError().text();
    }
    Tasks_query.finish();
}

void MainWindow::save()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Попередження!");

    QLabel *label = new QLabel("Не вимикайте программу!\n Триває збереження!", dialog);
    label->setAlignment(Qt::AlignCenter);
    QFont font = label->font();
    font.setPointSize(14); // Установка размера шрифта (14 - пример)
    label->setFont(font);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(label);

    dialog->setFixedSize(300, 100);

    // Отображение диалогового окна перед выполнением запроса
    dialog->show();
    QCoreApplication::processEvents();

    if(!dataBase->isOpen())
    {
       dataBase->open();
    }

    QSqlQuery query;
    query.prepare("UPDATE Project SET Name = :name, Description = :description, Start = TO_DATE(:start, 'YYYY-MM-DD'), "
                  "Deadline = TO_DATE(:deadline, 'YYYY-MM-DD') WHERE ProjectID = :id");
    for(int i=0; i<projects->size();i++)
    {
       query.bindValue(":name", projects->at(i)->getName());
       query.bindValue(":description", projects->at(i)->getDescription());
       query.bindValue(":start", projects->at(i)->getStart());
       query.bindValue(":deadline", projects->at(i)->getDeadline());
       query.bindValue(":id", projects->at(i)->getID());
       if (query.exec()) {
           qDebug() << "Query executed successfully for user with ID:" << projects->at(i)->getName();
       } else {
           qDebug() << "Error executing query for user with ID:" << projects->at(i)->getName() << query.lastError().text();
           label->setText("Помилка при записі!");
       }
    }
    query.clear();

    query.prepare("DELETE FROM ProjectUser WHERE ProjectID = :projectId");
    for(int i=0; i<projects->size();i++)
    {
        query.bindValue(":projectId", projects->at(i)->getID());
        if (!query.exec()) {
           qDebug() << "Ошибка при удалении записей из таблицы ProjectUser:" << query.lastError().text();
           label->setText("Помилка при записі!");
        }
    }
    query.clear();

    query.prepare("INSERT INTO ProjectUser (ProjectID, UserID) VALUES (:projectId, :userId)");
    for(int i=0; i<projects->size();i++)
    {
        QList<QUser*> *listUser = projects->at(i)->getListUser();
        for(int j=0; j<listUser->size(); j++)
        {
           query.bindValue(":projectId", projects->at(i)->getID());
           query.bindValue(":userId", listUser->at(j)->getId());
           if (!query.exec()) {
               qDebug() << "Ошибка при добавлении записи в таблицу ProjectUser:" << query.lastError().text();
               label->setText("Помилка при записі!");
           }
        }
    }
    query.clear();

    query.prepare("UPDATE Task SET Name = :name, Description = :description, Start = TO_DATE(:start, 'YYYY-MM-DD'), "
                  "Deadline = TO_DATE(:deadline, 'YYYY-MM-DD'), Completed = TO_DATE(:completed, 'YYYY-MM-DD'), UserID = :idUser, Priority = :priority WHERE TaskID = :id");
    for(int i=0; i<projects->size();i++)
    {
        QList<QTask*> *listTask = projects->at(i)->getListTask();
        for(int j=0; j<listTask->size(); j++)
        {
           query.bindValue(":name", listTask->at(j)->getName());
           query.bindValue(":description", listTask->at(j)->getDescription());
           query.bindValue(":start", listTask->at(j)->getStartTask());
           query.bindValue(":deadline", listTask->at(j)->getDeadLine());

           if(listTask->at(j)->getCompleted()=="") query.bindValue(":completed", QVariant());
           else query.bindValue(":completed", listTask->at(j)->getCompleted());

           query.bindValue(":idUser", listTask->at(j)->getIdUser());

           query.bindValue(":priority", listTask->at(j)->getPriority());

           query.bindValue(":id", listTask->at(j)->getId());
           if (!query.exec()) {
               qDebug() << "Error executing query for user with ID:" << listTask->at(j)->getName() << query.lastError().text();
               label->setText("Помилка при записі!");
           }
        }
    }
    query.clear();


    query.finish();
    // Обновление метки в диалоговом окне после сохранения данных
    label->setText("Ваші дані збереженні!");
    QCoreApplication::processEvents();

    dialog->exec();
}

void MainWindow::updateBd()
{
    for(int i=0; i<projects->size(); i++)
    {
        this->layout()->removeWidget(projects->at(i));
        delete projects->at(i);
    }
    projects->clear();
    handleLogin(logUser->getGmail(), logUser->getPass(), logUser->getName(), logUser->getRole(), logUser->getId(), dataBase);
}

void MainWindow::delUser()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Видалення", "Ви точно хочите видалити?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM Task WHERE UserId = :userId");
        query.bindValue(":userId", logUser->getId());
        if (!query.exec()) {
           qDebug() << "Ошибка при удалении из таблицы Task:" << query.lastError().text();
               QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при відаленні!");
        }
        else
        {
            query.clear();

            query.prepare("DELETE FROM ProjectUser WHERE UserId = :userId");
            query.bindValue(":userId", logUser->getId());
            if (!query.exec()) {
               qDebug() << "Ошибка при удалении из таблицы ProjectUser:" << query.lastError().text();
                   QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при відаленні!");
            }
            else
            {
                query.clear();

                query.prepare("DELETE FROM \"User\" WHERE UserId = :userId");
                query.bindValue(":userId", logUser->getId());
                if (!query.exec()) {
                 qDebug() << "Ошибка при удалении из таблицы User:" << query.lastError().text();
                     QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при відаленні!");
                }
                else
                {
                    delete logUser;
                    for(int i=0; i<projects->size(); i++)
                    {
                        this->layout()->removeWidget(projects->at(i));
                        delete projects->at(i);
                    }
                    projects->clear();
                    QCoreApplication::quit();
                    QProcess::startDetached(QCoreApplication::applicationFilePath());
                }
            }
        }
    }
}

void MainWindow::editUser()
{
    for (int i = 0; i < projects->size(); i++) {
        if (projects->at(i)->isVisible()) {
            projects->at(i)->hide();
        }
    }
    logUser->show();
    butEdit->show();
    connect(butEdit, &QPushButton::clicked, [this](){
        QSqlQuery query;
        query.prepare("UPDATE \"User\" SET Name = :name, Password = :password, Role = :role, Email = :email WHERE UserID = :id");
        query.bindValue(":name", logUser->getName());
        query.bindValue(":password",logUser->getPass());
        query.bindValue(":role", logUser->getRole());
        query.bindValue(":email", logUser->getGmail());
        query.bindValue(":id", logUser->getId());
        if(!query.exec())
        {
            qDebug()<< "Ошибка при редактировании User - "+query.lastError().text();
            QMessageBox::critical(nullptr, "Помилка", "Виникла помилка при редагуванні користувача!");
        }
        else
        {
            logUser->hide();
            butEdit->hide();
            updateBd();
        }
    });
}
