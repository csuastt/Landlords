#include "serverworker.h"

ServerWorker::ServerWorker(QObject *parent) : QObject(parent),fir(0)
{

}

void ServerWorker::choosingLandlord(QTcpSocket *clientB, QTcpSocket *clientC, QList<Card> *hands, QList<Card>* landLordBonus)
{
    qDebug("ok");
    //发牌
    QList<Card> all;
    for(int c = 0;c <= 3;c++)
    {
        for(int n = 1;n <= 13;n++)
        {
            all.append(Card(n,Card::Color(c)));
        }
    }
    all.append(Card(-1,Card::None,Card::red));
    all.append(Card(-1,Card::None,Card::black));
    std::random_shuffle(all.begin(),all.end());
    //给本玩家分配牌
    hands->clear();
    for(int i = 0;i < 17;i++)
    {
        hands->append(all[i]);
    }
    //发信息告知其他玩家
    sendPackage(clientB,"HandsNum 17");
    clientB->flush();
    sendPackage(clientC,"HandsNum 17");
    clientC->flush();
    //给其他玩家分配牌
    QString msg = "Hands";
    for(int i = 17;i < 34;i++)
    {
        msg = msg + " " + all[i].toString();
    }
    //发信息
    sendPackage(clientB,msg.toUtf8());
    clientB->flush();

    msg = "Hands";
    for(int i = 34;i < 51;i++)
    {
        msg = msg + " " + all[i].toString();
    }
    //发信息
    sendPackage(clientC,msg.toUtf8());
    clientC->flush();

    //剩下的是地主牌
    //告诉本玩家
    landLordBonus->clear();
    for(int i = 51;i < 54;i++)
    {
        all[i].setStatus(0);
        landLordBonus->append(all[i]);
    }
    //告诉其他玩家
    msg = "LandLordBonus";
    for(int i = 51;i < 54;i++)
    {
        msg = msg + " " + all[i].toString();
    }
    //发信息
    sendPackage(clientB,msg.toUtf8());
    clientB->flush();
    sendPackage(clientC,msg.toUtf8());
    clientC->flush();

    //随机一个人开始叫地主
    bool first = 0;
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int n = qrand() % 3;    //产生3以内的随机数
    fir = n;
    if(n == 0)
    {
        first = 1;
    }
    else if(n == 1)
    {
        sendPackage(clientB,"ChooseLandLord");
        clientB->flush();
    }
    else
    {
        sendPackage(clientC,"ChooseLandLord");
        clientC->flush();
    }
    emit choosingDone(first);
}

void ServerWorker::determineLandLord(QTcpSocket *clientB, QTcpSocket *clientC, QString AIsRun, QString BIsRun, QString CIsRun, int *status, bool *isAuthor, QList<Card> *hands, QList<Card> *landLordBonus)
{
    QList<int> ans;
    ans.append(AIsRun == "叫地主");
    ans.append(BIsRun == "叫地主");
    ans.append(CIsRun == "叫地主");
    int chosenone = 0;
    if(ans[(fir+2)%3])
    {
        chosenone = (fir+2)%3;
    }
    else if(ans[(fir+1)%3])
    {
        chosenone = (fir+1)%3;
    }
    else
    {
        chosenone = fir;
    }

    if(chosenone == 0)
    {
        //A当选地主
        //给其他玩家发信息
        sendPackage(clientB,"LandLord A");
        clientB->flush();
        sendPackage(clientC,"LandLord A");
        clientC->flush();
        //A自己初始化
        *status = 2;
        *isAuthor = 1;
        for(int i = 0;i < landLordBonus->size();i++)
        {
            (*landLordBonus)[i].setStatus(1);
        }
        hands->append(*landLordBonus);
        emit determineDone("地主","农民","农民");
    }
    else if(chosenone == 1)
    {
        //B当选地主
        //给其他玩家发信息
        sendPackage(clientB,"LandLord B");
        clientB->flush();
        sendPackage(clientC,"LandLord B");
        clientC->flush();
        //A自己初始化
        *status = 3;
        for(int i = 0;i < landLordBonus->size();i++)
        {
            (*landLordBonus)[i].setStatus(1);
        }
        emit determineDone("农民","地主","农民");
    }
    else
    {
        //C当选地主
        //给其他玩家发信息
        sendPackage(clientB,"LandLord C");
        clientB->flush();
        sendPackage(clientC,"LandLord C");
        clientC->flush();
        //A自己初始化
        *status = 4;
        for(int i = 0;i < landLordBonus->size();i++)
        {
            (*landLordBonus)[i].setStatus(1);
        }
        emit determineDone("农民","农民","地主");
    }
}


