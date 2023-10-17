#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , status(Ready)
    , ui(new Ui::MainWindow)
    , isCConnected(0)
    , isAuthor(0)
    , clientB(NULL)
    , clientC(NULL)
    , aIsContinue(-1)
    , bIsContinue(-1)
    , cIsContinue(-1)
{
    ui->setupUi(this);
    //初始化UI
    ui->labelAIsRun->setText("");
    ui->labelBIsRun->setText("");
    ui->labelCIsRun->setText("");
    //初始化系统更新定时器
    updateSystemTimer = new QTimer(this);
    updateSystemTimer->stop();
    updateSystemTimer->setInterval(100) ;//设置定时周期
    connect(updateSystemTimer,SIGNAL(timeout()),this,SLOT(on_systemtimer_timeout()));
    updateSystemTimer->start();

    //启动服务器线程
    //连接对应的信号与槽
    connect(this,SIGNAL(connectedOk(QTcpSocket*, QTcpSocket*, QList<Card>* , QList<Card>*)),
            &worker,SLOT(choosingLandlord(QTcpSocket*, QTcpSocket*, QList<Card>* , QList<Card>*)));
    connect(this,SIGNAL(chooseOK(QTcpSocket*, QTcpSocket*,QString,QString,QString,
                                 int*, bool* , QList<Card>*, QList<Card>*)),
            &worker,SLOT(determineLandLord(QTcpSocket*, QTcpSocket*,QString,QString,QString,
                                           int*, bool* , QList<Card>*, QList<Card>*)));
    connect(&worker,SIGNAL(choosingDone(bool)),this,SLOT(updateCardCoord(bool)));
    connect(&worker,SIGNAL(determineDone(QString, QString, QString)),
            this,SLOT(dealLandLordResult(QString, QString, QString)));
    worker.moveToThread(&serverThread);
    serverThread.start();
    serverThread.setPriority(QThread::TimeCriticalPriority);
    //测试
//    Card c;
//    c.fromString("N,8,C");
//    cardPool.append(c);
//    c.fromString("N,10,C");
//    cardPool.append(c);
//    c.fromString("N,13,C");
//    cardPool.append(c);
//    landLordBonus = cardPool;
//    c.fromString("R,-1,N");
//    cardPool.append(c);
//    c.fromString("R,-1,N");
//    c.setStatus(0);
//    cardPool.append(c);
//    hands = cardPool;
//    status = RoundA;
//    updateCardCoord();
//    isAuthor = true;
}

MainWindow::~MainWindow()
{
    serverThread.quit();
    serverThread.wait();
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    //设置画笔
    QPainter p(this);
    //绘制地主展示牌
    int y = 180, x = 500 - 30;
    for(int i = 0;i < landLordBonus.size();i++)
    {
        QPixmap tmp(landLordBonus[i].getPicturePath());
        tmp = tmp.scaledToHeight(120);
        p.drawPixmap(x + 30*i - 44,y - 60,tmp);
    }
    //绘制手牌
    for(int i = 0;i < hands.size();i++)
    {
        QPixmap tmp(hands[i].getPicturePath());
        tmp = tmp.scaledToHeight(120);
        p.drawPixmap(hands[i].getPos()[0] - 44,hands[i].getPos()[1] - 60 + ((hands[i].chosen) ? -30:0),tmp);
    }
    //绘制牌池的牌
    y = 330, x = 500 - cardPool.size()/2*30;
    for(int i = 0;i < cardPool.size();i++)
    {
        QPixmap tmp(cardPool[i].getPicturePath());
        tmp = tmp.scaledToHeight(120);
        p.drawPixmap(x + 30*i - 44,y - 60,tmp);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //只有在本回合才能选牌
    if(status == RoundA)
    {
        //获取鼠标当前的坐标
        float x=e->pos().x(),y=e->pos().y();
        if(e->button()==Qt::LeftButton)
        {
            for(int i = 0;i < hands.size();i++)
            {
                int hx = hands[i].getPos()[0];
                int hy = hands[i].getPos()[1] + ((hands[i].chosen) ? -30:0);
                if(i == hands.size() - 1)
                {
                    if(x <= hx + 44 && x >= hx - 44 && y <= hy + 60 && y >= hy - 60)
                    {
                        hands[i].chosen = !hands[i].chosen;
                        break;
                    }
                }
                else
                {
                    if(x <= hx - 44 + 30 && x >= hx - 44 && y <= hy + 60 && y >= hy - 60)
                    {
                        hands[i].chosen = !hands[i].chosen;
                        break;
                    }
                }
            }
            //更新画面
            update();
        }
    }
}

void MainWindow::updateCardCoord(bool flag)
{
    //为hands排序
    std::sort(hands.begin(),hands.end());
    int y = 580, x = 500 - hands.size()/2*30;
    for(int i = 0;i < hands.size();i++)
    {
        hands[i].setPos(QList<int>({x + 30*i,y}));
    }
    ui->labelACardNum->setText("手牌数:" + QString::number(hands.size()));
    if(flag)
    {
        ui->pushButtonRun->setVisible(1);
        ui->pushButtonNotRun->setVisible(1);
    }
    update();
}

void MainWindow::dealLandLordResult(QString ansA, QString ansB, QString ansC)
{
    ui->labelAIsRun->setText("");
    ui->labelBIsRun->setText("");
    ui->labelCIsRun->setText("");
    ui->labelAStatus->setText(ansA);
    ui->labelBStatus->setText("玩家B:" + ansB);
    ui->labelCStatus->setText("玩家C:" + ansC);
    //更新手牌数
    if(ansA == "地主")
    {
        ui->labelACardNum->setText("手牌数:20");
        updateCardCoord();
    }
    else if(ansB == "地主")
    {
        ui->labelBCardNum->setText("手牌数:20");
    }
    else
    {
        ui->labelCCardNum->setText("手牌数:20");
    }
}

void MainWindow::on_systemtimer_timeout()
{
    switch(status)
    {
    case Ready:
        //设置各个button的状态
        ui->pushButtonRun->setVisible(0);
        ui->pushButtonNotRun->setVisible(0);
        ui->pushButtonQuit->setVisible(0);
        ui->pushButtonRestart->setVisible(0);
        ui->pushButtonSkip->setVisible(0);
        ui->pushButtonSubmit->setVisible(0);
        ui->pushButtonStart->setVisible(1);
        if(isCConnected && clientB != NULL && clientB->state() == QAbstractSocket::ConnectedState
                && clientC != NULL && clientC->state() == QAbstractSocket::ConnectedState)
        {
            emit connectedOk(clientB, clientC, &hands, &landLordBonus);
            status = Choosing;
        }
        break;
    case Choosing:
        ui->labelInfo->setText("正在选地主阶段");
        //设置各个button的状态
        ui->pushButtonQuit->setVisible(0);
        ui->pushButtonRestart->setVisible(0);
        ui->pushButtonSkip->setVisible(0);
        ui->pushButtonSubmit->setVisible(0);
        ui->pushButtonStart->setVisible(0);
        if(ui->labelAIsRun->text() != "" &&
                ui->labelBIsRun->text() != "" &&
                ui->labelCIsRun->text() != "")
            emit chooseOK(clientB, clientC,ui->labelAIsRun->text(),ui->labelBIsRun->text(),ui->labelCIsRun->text(),
                          (int*)&status, &isAuthor, &hands,&landLordBonus);
        break;
    case RoundA:
        ui->labelInfo->setText("玩家A的回合");
        //设置各个button的状态
        ui->pushButtonRun->setVisible(0);
        ui->pushButtonNotRun->setVisible(0);
        ui->pushButtonQuit->setVisible(0);
        ui->pushButtonRestart->setVisible(0);
        ui->pushButtonSkip->setVisible(1);
        ui->pushButtonSubmit->setVisible(1);
        ui->pushButtonStart->setVisible(0);
        break;
    case RoundB:
        ui->labelInfo->setText("玩家B的回合");
        //设置各个button的状态
        ui->pushButtonRun->setVisible(0);
        ui->pushButtonNotRun->setVisible(0);
        ui->pushButtonQuit->setVisible(0);
        ui->pushButtonRestart->setVisible(0);
        ui->pushButtonSkip->setVisible(0);
        ui->pushButtonSubmit->setVisible(0);
        ui->pushButtonStart->setVisible(0);
        break;
    case RoundC:
        ui->labelInfo->setText("玩家C的回合");
        //设置各个button的状态
        ui->pushButtonRun->setVisible(0);
        ui->pushButtonNotRun->setVisible(0);
        ui->pushButtonQuit->setVisible(0);
        ui->pushButtonRestart->setVisible(0);
        ui->pushButtonSkip->setVisible(0);
        ui->pushButtonSubmit->setVisible(0);
        ui->pushButtonStart->setVisible(0);
        break;
    case End:
        //设置各个button的状态
        ui->pushButtonRun->setVisible(0);
        ui->pushButtonNotRun->setVisible(0);
        ui->pushButtonQuit->setVisible(1);
        ui->pushButtonRestart->setVisible(1);
        ui->pushButtonSkip->setVisible(0);
        ui->pushButtonSubmit->setVisible(0);
        ui->pushButtonStart->setVisible(0);
        //检查是否重新开始
        qDebug() << aIsContinue << " " << bIsContinue << " " << cIsContinue << " ";
        if(aIsContinue == 1 && bIsContinue == 1 && cIsContinue == 1)
        {
            //重新开始
            //发信息
            sendPackage(clientB,"Restart");
            clientB->flush();
            sendPackage(clientC,"Restart");
            clientC->flush();
            //重置一下变量
            isAuthor = 0;
            cardPool.clear();
            landLordBonus.clear();
            hands.clear();
            aIsContinue = -1;
            bIsContinue = -1;
            cIsContinue = -1;
            ui->labelAIsRun->setText("");
            ui->labelBIsRun->setText("");
            ui->labelCIsRun->setText("");
            ui->labelACardNum->setText("手牌数:未知");
            ui->labelBCardNum->setText("手牌数:未知");
            ui->labelCCardNum->setText("手牌数:未知");
            ui->labelAStatus->setText("未知");
            ui->labelBStatus->setText("玩家B:未知");
            ui->labelCStatus->setText("玩家C:未知");
            update();
            emit connectedOk(clientB, clientC, &hands, &landLordBonus);
            status = Choosing;
        }
        break;
    }
}

void MainWindow::slotNewConnection()
{
    //处理客户端的连接请求
    if(serverB->hasPendingConnections())
    {
        clientB = serverB->nextPendingConnection();
        connect(clientB, &QTcpSocket::readyRead,
        this, &MainWindow::slotReadyReadB);
    }
    else
    {
        clientC = serverC->nextPendingConnection();
        connect(clientC, &QTcpSocket::readyRead,
        this, &MainWindow::slotReadyReadC);
    }
}

void MainWindow::slotReadyReadB()
{
    //接收数据
    //拆包
    QByteArray recv_data;
    static QByteArray m_buffer; //缓存上一次没有读完的数据
    //缓冲区没有数据，直接无视
    if(clientB->bytesAvailable() <= 0 )
    {
        return;
    }
    //临时获得从缓存区取出来的数据，但是不确定每次取出来的是多少。
    QByteArray buffer;
    //如果是信号readyRead触发的，使用readAll时会一次把这一次可用的数据全总读取出来
    buffer = clientB->readAll();
    //上次缓存加上这次数据
    m_buffer.append(buffer);
    ushort mesg_len;
    int totalLen = m_buffer.size();
    while(totalLen)
    {
        //与QDataStream绑定，方便操作。
        QDataStream packet(m_buffer);
        packet.setByteOrder(QDataStream::BigEndian);
        //不够包头的数据直接就不处理。
        if( (unsigned int)totalLen < 6)
        {
            break;
        }
        packet >> mesg_len;
        //如果不够长度等够了在来解析
        if( totalLen < mesg_len )
        {
            break;
        }
        //数据足够多
        packet >> recv_data;
        QString str(recv_data);
        qDebug() << str;
        //开始处理不同的消息
        if(str.startsWith("Call "))
        {
            if(str[5] == '0')
                ui->labelBIsRun->setText("不叫");
            else
                ui->labelBIsRun->setText("叫地主");
        }
        else if(str.startsWith("HandsNum "))
        {
            QString num_str = str.mid(9);
            ui->labelBCardNum->setText("手牌数:"+num_str);
        }
        else if(str.startsWith("Round"))
        {
            if(status != End)
            {
                status = RoundB;
                ui->labelBIsRun->setText("");
            }
        }
        else if(str.startsWith("Cards "))
        {
            //显示出牌
            ui->labelBIsRun->setText("出牌");
            QString cards_str = str.mid(6);
            QStringList cards = cards_str.split(" ");
            if(str.endsWith("End"))
                cards.pop_back();
            cardPool.clear();
            foreach(QString card, cards)
            {
                Card tmp;
                tmp.fromString(card);
                cardPool.append(tmp);
            }
            isAuthor = false;
            update();
        }
        else if(str.startsWith("Skip"))
        {
            //显示不出
            ui->labelBIsRun->setText("不出");
        }
        else if(str.startsWith("Success"))
        {
            //显示出牌
            ui->labelBIsRun->setText("出牌");
            //显示胜利标志
            if(ui->labelBStatus->text().mid(4) == ui->labelAStatus->text())
                ui->labelInfo->setText("你赢了");
            else
                ui->labelInfo->setText("你输了");
            status = End;
            //开始结算线程
        }
        else if(str.startsWith("CConnected"))
        {
            isCConnected = true;
        }
        else if(str.startsWith("Continued"))
        {
            bIsContinue = 1;
        }
        else if(str.startsWith("NotContinued"))
        {
            on_pushButtonQuit_clicked();
        }

        //缓存多余的数据
        buffer = m_buffer.right(totalLen - mesg_len - 6);
        //更新长度
        totalLen = buffer.size();
        //更新多余数据
        m_buffer = buffer;
    }



}

void MainWindow::slotReadyReadC()
{
    //接收数据
    //拆包
    QByteArray recv_data;
    static QByteArray m_buffer; //缓存上一次没有读完的数据
    //缓冲区没有数据，直接无视
    if(clientC->bytesAvailable() <= 0 )
    {
        return;
    }
    //临时获得从缓存区取出来的数据，但是不确定每次取出来的是多少。
    QByteArray buffer;
    //如果是信号readyRead触发的，使用readAll时会一次把这一次可用的数据全总读取出来
    buffer = clientC->readAll();
    //上次缓存加上这次数据
    m_buffer.append(buffer);
    ushort mesg_len;
    int totalLen = m_buffer.size();
    while(totalLen)
    {
        //与QDataStream绑定，方便操作。
        QDataStream packet(m_buffer);
        packet.setByteOrder(QDataStream::BigEndian);
        //不够包头的数据直接就不处理。
        if( (unsigned int)totalLen < 6)
        {
            break;
        }
        packet >> mesg_len;
        //如果不够长度等够了在来解析
        if( totalLen < mesg_len )
        {
            break;
        }
        //数据足够多
        packet >> recv_data;
        QString str(recv_data);
        qDebug() << str;
        //开始处理不同的消息
        if(str.startsWith("Call "))
        {
            if(str[5] == '0')
                ui->labelCIsRun->setText("不叫");
            else
                ui->labelCIsRun->setText("叫地主");
            if(ui->labelAIsRun->text() == "")
            {
                ui->pushButtonRun->setVisible(1);
                ui->pushButtonNotRun->setVisible(1);
            }
        }
        else if(str.startsWith("HandsNum "))
        {
            QString num_str = str.mid(9);
            ui->labelCCardNum->setText("手牌数:"+num_str);
        }
        else if(str.startsWith("Round"))
        {
            status = RoundC;
            ui->labelCIsRun->setText("");
        }
        else if(str.startsWith("Cards "))
        {
            //显示出牌
            ui->labelCIsRun->setText("出牌");
            QString cards_str = str.mid(6);
            QStringList cards = cards_str.split(" ");
            if(str.endsWith("End"))
                cards.pop_back();
            cardPool.clear();
            foreach(QString card, cards)
            {
                Card tmp;
                tmp.fromString(card);
                cardPool.append(tmp);
            }
            isAuthor = false;
            update();
            if(!str.endsWith("End"))
            {
                //进入我方回合
                status = RoundA;
                ui->labelAIsRun->setText("");
                sendPackage(clientB,"Round");
                clientB->flush();
                sendPackage(clientC,"Round");
                clientC->flush();
            }
        }
        else if(str.startsWith("Skip"))
        {
            //显示不出
            ui->labelCIsRun->setText("不出");
            //进入我方回合
            status = RoundA;
            ui->labelAIsRun->setText("");
            sendPackage(clientB,"Round");
            clientB->flush();
            sendPackage(clientC,"Round");
            clientC->flush();
        }
        else if(str.startsWith("Success"))
        {
            //显示出牌
            ui->labelCIsRun->setText("出牌");
            //显示胜利标志
            if(ui->labelCStatus->text().mid(4) == ui->labelAStatus->text())
                ui->labelInfo->setText("你赢了");
            else
                ui->labelInfo->setText("你输了");
            status = End;
            //开始结算线程
        }
        else if(str.startsWith("Continued"))
        {
            cIsContinue = 1;
        }
        else if(str.startsWith("NotContinued"))
        {
            on_pushButtonQuit_clicked();
        }

        //缓存多余的数据
        buffer = m_buffer.right(totalLen - mesg_len - 6);
        //更新长度
        totalLen = buffer.size();
        //更新多余数据
        m_buffer = buffer;
    }
}


void MainWindow::on_pushButtonStart_clicked()
{
    //创建套接字对象
    serverB = new QTcpServer(this);
    //将套接字设置为监听模式
    serverB->listen(QHostAddress::Any, 18001);
    //通过信号接收客户端请求
    connect(serverB, &QTcpServer::newConnection,
    this, &MainWindow::slotNewConnection);
    //创建套接字对象
    serverC = new QTcpServer(this);
    //将套接字设置为监听模式
    serverC->listen(QHostAddress::Any, 18002);
    //通过信号接收客户端请求
    connect(serverC, &QTcpServer::newConnection,
    this, &MainWindow::slotNewConnection);
}

void MainWindow::on_pushButtonRun_clicked()
{
    //设置状态
    ui->pushButtonRun->setVisible(0);
    ui->pushButtonNotRun->setVisible(0);
    ui->labelAIsRun->setText("叫地主");
    //发信息
    sendPackage(clientB,"Call 1");
    clientB->flush();
    sendPackage(clientC,"Call 1");
    clientC->flush();
}

void MainWindow::on_pushButtonNotRun_clicked()
{
    //设置状态
    ui->pushButtonRun->setVisible(0);
    ui->pushButtonNotRun->setVisible(0);
    ui->labelAIsRun->setText("不叫");
    //发信息
    sendPackage(clientB,"Call 0");
    clientB->flush();
    sendPackage(clientC,"Call 0");
    clientC->flush();
}

void MainWindow::on_pushButtonSubmit_clicked()
{
    //提取待出牌
    QList<Card> out;
    foreach(Card c,hands)
    {
        if(c.chosen)
        {
            out.append(c);
        }
    }
    //是否可以出牌
    bool ok = false;
    if(isAuthor)
    {
        //判断是否可以出
        if(Card::isOK2Submit(out))
        {
            ok = true;
        }
        else
        {
            ok = false;
        }
    }
    else
    {
        //判断是否可以出
        if(Card::isOK2Submit(out, cardPool))
        {
            ok = true;
        }
        else
        {
            ok = false;
        }
    }
    //进行相应操作
    if(ok)
    {
        QString msg_card = "Cards";
        foreach(Card c,out)
        {
            msg_card = msg_card + " " + c.toString();
        }
        //更新状态
        isAuthor = true;
        ui->labelAIsRun->setText("出牌");
        cardPool = out;
        //删去这些出的牌
        foreach(Card c,out)
        {
            hands.removeOne(c);
        }
        //更新牌坐标
        updateCardCoord();
        //发送牌数
        QString msg_num = "HandsNum " + QString::number(hands.size());
        //发牌数信息
        sendPackage(clientB,msg_num.toUtf8());
        clientB->flush();
        sendPackage(clientC,msg_num.toUtf8());
        clientC->flush();
        //如果牌数为0则胜利
        //显示胜利标志
        if(hands.size() == 0)
        {
            ui->labelInfo->setText("你赢了");
            status = End;
            //发送信息
            sendPackage(clientB,"Success");
            clientB->flush();
            sendPackage(clientC,"Success");
            clientC->flush();
            msg_card += " End";
        }
        //发送卡牌信息
        sendPackage(clientB,msg_card.toUtf8());
        clientB->flush();
        sendPackage(clientC,msg_card.toUtf8());
        clientC->flush();
    }
    else
    {
        QString dlgTitle="提示";
        QString strInfo="出牌不合法";
        QMessageBox::information(this, dlgTitle, strInfo,
                                  QMessageBox::Ok,QMessageBox::NoButton);
        ui->statusbar->showMessage("出牌不合法",2000);
    }
}

void MainWindow::on_pushButtonSkip_clicked()
{
    if(isAuthor)
    {
        QString dlgTitle="提示";
        QString strInfo="您有牌权，必须出牌哦";
        QMessageBox::information(this, dlgTitle, strInfo,
                                  QMessageBox::Ok,QMessageBox::NoButton);
        return;
    }
    else
    {
        ui->labelAIsRun->setText("不出");
        //复位
        for(int i = 0;i < hands.size();i++)
        {
            hands[i].chosen = 0;
        }
        update();
        //发信息
        sendPackage(clientB,"Skip");
        clientB->flush();
        sendPackage(clientC,"Skip");
        clientC->flush();
    }
}

void MainWindow::on_pushButtonRestart_clicked()
{
    aIsContinue = 1;
}

void MainWindow::on_pushButtonQuit_clicked()
{
    //发信息
//    sendPackage(clientB,"Quit");
//    clientB->flush();
//    sendPackage(clientC,"Quit");
//    clientC->flush();
    //直接退出
    close();
}
