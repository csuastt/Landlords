#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <package.h>
#include <QTime>
#include <card.h>
#include <algorithm>

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
private:
    int fir;

signals:
    void choosingDone(bool flag);
    void determineDone(QString ansA, QString ansB, QString ansC);
private slots:
    //定义槽函数
    //选地主
    void choosingLandlord(QTcpSocket* clientB, QTcpSocket* clientC, QList<Card>* hands, QList<Card>* landLordBonus);
    //定地主
    void determineLandLord(QTcpSocket* clientB, QTcpSocket* clientC,QString AIsRun,QString BIsRun,QString CIsRun,
                                      int* status, bool* isAuthor, QList<Card>* hands, QList<Card>* landLordBonus);
};

#endif // SERVERWORKER_H
