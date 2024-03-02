#ifndef QDIALOGLOGIN_H
#define QDIALOGLOGIN_H
#include <QWidget>
#include <QDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QUser.h>

class QDialogLogin : public QDialog
{
    Q_OBJECT
public:
    QDialogLogin(QWidget* parent=nullptr);
private:
    QFormLayout *formlayout;
    QPushButton *but;
    QPushButton *butNewUser;
signals:
    void loginSuccessful(const QString& username, const QString& password);
};

#endif // QDIALOGLOGIN_H
