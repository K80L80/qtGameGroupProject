#ifndef DROPLET_H
#define DROPLET_H

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsItem>
#include <QTimer>


class droplet:public QObject,public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    droplet(bool* gameOver, QGraphicsItem *parent = nullptr);
    int drop_speed = 1;
    bool* gameOver;
public slots:
    void move();
};

#endif // DROPLET_H
