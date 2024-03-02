#include"QUser.h"

QUser::QUser(QString nameC, QString passwordC, QUser *parent):QWidget(parent)
{

    mainLayout = new QVBoxLayout(this);
    delBut = new QPushButton("Видалити");
    mainLayout->addWidget(delBut);

    QFormLayout* formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);
    this->setMinimumSize(200,200);
    this->setMaximumSize(400,400);

    name = new QLineEdit(nameC);
    name->setPlaceholderText("Напишіть...");
    password = new QLineEdit(passwordC);
    password->setPlaceholderText("Напишіть...");
    role = new QLineEdit;
    role->setPlaceholderText("Напишіть...");
    gmail = new QLineEdit;
    gmail->setPlaceholderText("Напишіть...");
    listTask_Project = new QList<std::pair<QString,QString>>;
    listProject = new QList<QString>;

    formLayout->addRow("Ім'я:",name);
    formLayout->addRow("Пароль:",password);
    formLayout->addRow("Роль:",role);
    formLayout->addRow("Пошта:",gmail);

    infProjectTask(mainLayout);

    connect(delBut, &QPushButton::clicked,[this](){ emit delObj(this);});
    addStyle();
}

QUser::QUser(const QUser *other)
    : QWidget(other->parentWidget())
{
    this->id=other->id;

    mainLayout = new QVBoxLayout(this);
    delBut = new QPushButton("Видалити");
    mainLayout->addWidget(delBut);

    QFormLayout* formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);
    this->setMinimumSize(200, 200);
    this->setMaximumSize(400, 400);

    name = new QLineEdit(other->name->text());
    name->setPlaceholderText("Напишіть...");
    password = new QLineEdit(other->password->text());
    password->setPlaceholderText("Напишіть...");
    role = new QLineEdit(other->role->text());
    role->setPlaceholderText("Напишіть...");
    gmail = new QLineEdit(other->gmail->text());
    gmail->setPlaceholderText("Напишіть...");

    listTask_Project = new QList<std::pair<QString, QString>>(*other->listTask_Project);
    listProject = new QList<QString>(*other->listProject);

    formLayout->addRow("Ім'я:", name);
    formLayout->addRow("Пароль:", password);
    formLayout->addRow("Роль:", role);
    formLayout->addRow("Пошта:", gmail);

    infProjectTask(mainLayout);

    connect(delBut, &QPushButton::clicked, [this]() { emit delObj(this); });
    addStyle();
}

void QUser::addStyle()
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
                        "}");

}

void QUser::infProjectTask(QVBoxLayout *mainLayout)
{
    QGridLayout *gridLayout=new QGridLayout;

    QPushButton *butProject=new QPushButton("Проєкти");
    butProject->setMaximumSize(100,30);
    butProject->setStyleSheet("font-size: 16px;");

    QPushButton *butProjectTask=new QPushButton("Задачі");
    butProjectTask->setMaximumSize(100,30);
    butProjectTask->setStyleSheet("font-size: 16px;");

    gridLayout->addWidget(butProject,0,0);
    gridLayout->addWidget(butProjectTask,1,0);

    QHBoxLayout *projectList=new QHBoxLayout;
    QHBoxLayout *projectTaskList=new QHBoxLayout;

    gridLayout->addLayout(projectList,0,1);
    gridLayout->addLayout(projectTaskList,1,1);

    mainLayout->addLayout(gridLayout);

    QTextEdit *label=new QTextEdit;

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(label);
    scrollArea->setWidgetResizable(true);
    projectList->addWidget(scrollArea);
    connect(butProject,&QPushButton::clicked,[this, projectList, label](){
        if(label->toPlainText()=="")
        {
            if(!listProject->empty())
            {
                for(int i=0; i<listProject->size(); i++)
                {
                    if(i==0) label->append(listProject->at(i));
                    else label->append(listProject->at(i));
                }
            }
        }
        else
        {
            label->clear();
        }
    });

    QTextEdit *label2= new QTextEdit;

    QScrollArea *scrollArea2 = new QScrollArea;
    scrollArea2->setWidget(label2);
    scrollArea2->setWidgetResizable(true);
    projectTaskList->addWidget(scrollArea2);
    connect(butProjectTask,&QPushButton::clicked,[this, label2 ,projectTaskList](){
        if(label2->toPlainText()=="")
        {
            if(!listTask_Project->empty())
            {   for(int i=0; i<listTask_Project->size(); i++)
                {
                    if(i==0) label2->append(listTask_Project->at(i).first+" - "+listTask_Project->at(i).second);
                    else label2->append(listTask_Project->at(i).first+" - "+listTask_Project->at(i).second);
                }
            }
        }
        else
        {
            label2->clear();
        }
    });
}

void QUser::setReadOnlyUser(bool check)
{
    if(check)
    {
        delBut->hide();

        name->setReadOnly(true);
        password->setReadOnly(true);
        role->setReadOnly(true);
        gmail->setReadOnly(true);
    }
    else
    {
        delBut->show();
        name->setReadOnly(false);
        password->setReadOnly(false);
        role->setReadOnly(false);
        gmail->setReadOnly(false);
    }
}
