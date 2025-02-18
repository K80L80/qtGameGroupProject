#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>
#include "usermanager.h"

namespace Ui {
class history;
}

class history : public QWidget
{
    Q_OBJECT

public:
    explicit history(UserInfo* userinfo, QWidget *parent = nullptr);
    ~history();
    UserInfo* userinfo;

private:
    Ui::history *ui;
};

#endif // HISTORY_H
