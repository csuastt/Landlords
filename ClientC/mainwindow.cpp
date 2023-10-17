#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , status(Ready)
    , ui(new Ui::MainWindow)
    , isAuthor(0)
    , clientB(NULL)
    , clientC(NULL)
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
}

MainWindow::~MainWindow()
{
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
        break;
    case Choosing:
        ui->labelInfo->setText("正在选地主阶段");
        //设置各个button的状态
        ui->pushButtonQuit->setVisible(0);
        ui->pushButtonRestart->setVisible(0);
        ui->pushButtonSkip->setVisible(0);
        ui->pushButtonSubmit->setVisible(0);
        ui->pushButtonStart->setVisible(0);
        break;
    case RoundA:
        ui->labelInfo->setText("玩家C的回合");
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
        ui->labelInfo->setText("玩家A的回合");
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
    case End:
        //设置各个button的状态
        ui->pushButtonRun->setVisible(0);
        ui->pushButtonNotRun->setVisible(0);
        ui->pushButtonQuit->setVisible(1);
        ui->pushButtonRestart->setVisible(1);
        ui->pushButtonSkip->setVisible(0);
        ui->pushButtonSubmit->setVisible(0);
        ui->pushButtonStart->setVisible(0);
        break;
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
        //开始处理不同的消息
        qDebug() << str;
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
        }
        else if(str.startsWith("Quit"))
        {
            //直接退出
            close();
        }
        else if(str.startsWith("Hands "))
        {
            //加入手牌
            QString cards_str = str.mid(6);
            QStringList cards = cards_str.split(" ");
            hands.clear();
            foreach(QString card, cards)
            {
                Card tmp;
                tmp.fromString(card);
                hands.append(tmp);
            }
            //更新手牌数
            ui->labelACardNum->setText("手牌数:17");
            //发送手牌数
            sendPackage(clientB,"HandsNum 17");
            clientB->flush();
            sendPackage(clientC,"HandsNum 17");
            clientC->flush();
            status = Choosing;
            updateCardCoord();
        }
        else if(str.startsWith("LandLordBonus "))
        {
            //加入地主牌
            QString cards_str = str.mid(14);
            QStringList cards = cards_str.split(" ");
            landLordBonus.clear();
            foreach(QString card, cards)
            {
                Card tmp;
                tmp.fromString(card);
                landLordBonus.append(tmp);
            }
            //隐藏每张地主牌
            for(int i = 0;i < landLordBonus.size();i++)
            {
                landLordBonus[i].setStatus(0);
            }
            update();
        }
        else if(str.startsWith("ChooseLandLord"))
        {
            ui->pushButtonRun->setVisible(1);
            ui->pushButtonNotRun->setVisible(1);
        }
        else if(str.startsWith("LandLord "))
        {
            QString alp = str.mid(9);
            //先全部设成农民，挖空法技巧
            ui->labelAStatus->setText("农民");
            ui->labelBStatus->setText("玩家A:农民");
            ui->labelCStatus->setText("玩家B:农民");
            if(alp == "A")
            {
                //A当选地主
                status = RoundB;
                ui->labelBCardNum->setText("手牌数:20");
                //设置身份
                ui->labelBStatus->setText("玩家A:地主");
            }
            else if(alp == "B")
            {
                //B当选
                status = RoundC;
                ui->labelCCardNum->setText("手牌数:20");
                //设置身份
                ui->labelCStatus->setText("玩家B:地主");
            }
            else
            {
                //C当选
                status = RoundA;
                isAuthor = 1;
                //曝光
                for(int i = 0;i < landLordBonus.size();i++)
                {
                    landLordBonus[i].setStatus(1);
                }
                hands.append(landLordBonus);
                updateCardCoord();
                ui->labelACardNum->setText("手牌数:20");
                //设置身份
                ui->labelAStatus->setText("地主");
            }
            //再次初始化label
            ui->labelAIsRun->setText("");
            ui->labelBIsRun->setText("");
            ui->labelCIsRun->setText("");
            //曝光每张地主牌
            for(int i = 0;i < landLordBonus.size();i++)
            {
                landLordBonus[i].setStatus(1);
            }
        }
        else if(str.startsWith("Restart"))
        {
            //重置一下变量
            isAuthor = 0;
            cardPool.clear();
            landLordBonus.clear();
            hands.clear();
            ui->labelAIsRun->setText("");
            ui->labelBIsRun->setText("");
            ui->labelCIsRun->setText("");
            ui->labelACardNum->setText("手牌数:未知");
            ui->labelBCardNum->setText("手牌数:未知");
            ui->labelCCardNum->setText("手牌数:未知");
            ui->labelAStatus->setText("未知");
            ui->labelBStatus->setText("玩家A:未知");
            ui->labelCStatus->setText("玩家B:未知");
            update();
            status = Choosing;
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
    //创建套接字,连接上B
    clientC = new QTcpSocket(this);
    //连接服务器
    clientC->connectToHost(QHostAddress("127.0.0.1"), 18003);
    //通过信号接收服务器数据
    connect(clientC, &QTcpSocket::readyRead,
            this, &MainWindow::slotReadyReadC);
    clientC->waitForConnected(10000);
    //创建套接字对象,连接上A
    clientB = new QTcpSocket(this);
    //连接服务器
    clientB->connectToHost(QHostAddress("127.0.0.1"), 18002);
    //通过信号接收服务器数据
    connect(clientB, &QTcpSocket::readyRead,
            this, &MainWindow::slotReadyReadB);
    clientB->waitForConnected(10000);
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
        ui->labelAIsRun->setText("出牌");
        cardPool = out;
        isAuthor = true;
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
    sendPackage(clientB,"Continued");
    clientB->flush();
}

void MainWindow::on_pushButtonQuit_clicked()
{
//    sendPackage(clientB,"NotContinued");
//    clientB->flush();
    close();
}
