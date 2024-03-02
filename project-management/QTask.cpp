
#include "QTask.h"



QTask::QTask(QString nameTaskC, QString priorityC, QString StartTaskC, QString DeadlineC, QWidget *parent):
    QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    del_Task=new QPushButton("Видалити");
    del_Task->setMaximumHeight(30);
    del_Task->setStyleSheet("font-size: 18px;");
    mainLayout->addWidget(del_Task);

    formLayout = new MyFormLayout;
    mainLayout->addLayout(formLayout);
    this->setMinimumSize(200,200);
    this->setMaximumSize(400,400);
    nameTask = new QLineEdit(nameTaskC);
    nameTask->setPlaceholderText("Напишіть...");
    StartTask = new QLineEdit(StartTaskC);
    StartTask->setPlaceholderText("Напишіть...");
    Deadline = new QLineEdit(DeadlineC);
    Deadline->setPlaceholderText("Напишіть...");
    Completed = new QLineEdit;
    Completed->setPlaceholderText("Ще не виконано...");
    User_Name = new QLineEdit;
    User_Name->setPlaceholderText("Напишіть...");

    formLayout->addRow("Назва", nameTask);
    prEdit = new QLineEdit(priorityC);

    QPushButton* butPriority = new QPushButton;
    addPriorityMenu(butPriority);
    formLayout->addRow("Приорітет", prEdit, butPriority);
    formLayout->addRow("Початок", StartTask);
    formLayout->addRow("Дедлайн", Deadline);
    formLayout->addRow("Виконано", Completed);
    formLayout->addRow("Виконавець", User_Name);
    descriptionTask = new QTextEdit;
    descriptionTask->setPlaceholderText("Опис...");

    addStyle();
    mainLayout->addWidget(descriptionTask);

    connect(del_Task, &QPushButton::clicked,[this](){ emit delObj(this);});
}

QTask::QTask(QString nameTaskC, QString priorityC, QString StartTaskC, QString DeadlineC, QString CompletedC, QWidget *parent):
    QTask(nameTaskC, priorityC, StartTaskC, DeadlineC, parent){Completed->setText(CompletedC);}

QTask::QTask(const QTask *other)
    : QWidget(other->parentWidget()), Progect_Name(other->Progect_Name)
{
    mainLayout = new QVBoxLayout(this);
    del_Task = new QPushButton("Видалити");
    del_Task->setMaximumHeight(30);
    del_Task->setStyleSheet("font-size: 18px;");
    mainLayout->addWidget(del_Task);

    formLayout = new MyFormLayout;
    mainLayout->addLayout(formLayout);
    this->setMinimumSize(200, 200);
    this->setMaximumSize(400, 400);
    nameTask = new QLineEdit(other->nameTask->text());
    nameTask->setPlaceholderText("Напишіть...");
    StartTask = new QLineEdit(other->StartTask->text());
    StartTask->setPlaceholderText("Напишіть...");
    Deadline = new QLineEdit(other->Deadline->text());
    Deadline->setPlaceholderText("Напишіть...");
    Completed = new QLineEdit(other->Completed->text());
    Completed->setPlaceholderText("Ще не виконано...");
    User_Name = new QLineEdit(other->User_Name->text());
    User_Name->setPlaceholderText("Напишіть...");

    formLayout->addRow("Назва", nameTask);

    prEdit = new QLineEdit(other->prEdit->text());

    QPushButton* butPriority = new QPushButton;
    addPriorityMenu(butPriority);
    formLayout->addRow("Приорітет", prEdit, butPriority);
    formLayout->addRow("Початок", StartTask);
    formLayout->addRow("Дедлайн", Deadline);
    formLayout->addRow("Виконано", Completed);
    formLayout->addRow("Виконавець", User_Name);
    descriptionTask = new QTextEdit(other->descriptionTask->toPlainText());
    descriptionTask->setPlaceholderText("Опис...");

    addStyle();
    mainLayout->addWidget(descriptionTask);

    connect(del_Task, &QPushButton::clicked, [this]() { emit delObj(this); });
}

void QTask::addStyle()
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
                        "QPushButton QMenu {"
                        "    background-color: #888888;}");
}

void QTask::addPriorityMenu(QPushButton *butPriority)
{
    QMenu* menu = new QMenu(butPriority);  // Создание QMenu

    QAction* action1 = new QAction("Низький", menu);
    QAction* action2 = new QAction("Середній", menu);
    QAction* action3 = new QAction("Високий", menu);
    QAction* action4 = new QAction("Критичний", menu);

    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    menu->addAction(action4);

    butPriority->setMenu(menu);

    connect(action1, &QAction::triggered, [this, action1](){
        prEdit->setText(action1->text());
    });
    connect(action2, &QAction::triggered, [this, action2](){
        prEdit->setText(action2->text());
    });
    connect(action3, &QAction::triggered, [this, action3](){
        prEdit->setText(action3->text());
    });
    connect(action4, &QAction::triggered, [this, action4](){
        prEdit->setText(action4->text());
    });
}

void QTask::setReadOnlyTask(bool check)
{
    if(check)
    {
        del_Task->hide();

        User_Name->setReadOnly(true);
        prEdit->setReadOnly(true);
        nameTask->setReadOnly(true);
        descriptionTask->setReadOnly(true);
        StartTask->setReadOnly(true);
        Deadline->setReadOnly(true);
        Completed->setReadOnly(true);
    }
    else
    {
        del_Task->show();

        User_Name->setReadOnly(false);
        prEdit->setReadOnly(false);
        nameTask->setReadOnly(false);
        descriptionTask->setReadOnly(false);
        StartTask->setReadOnly(false);
        Deadline->setReadOnly(false);
        Completed->setReadOnly(false);
    }
}
