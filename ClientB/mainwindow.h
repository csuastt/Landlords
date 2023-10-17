#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QThread>
#include <package.h>
#include <card.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //定义枚举类型
    //记录游戏当前的状态
    enum Status
    {
        Ready, //游戏未开始
        Choosing, //选地主
        RoundA, //A的回合
        RoundB, //B的回合
        RoundC, //C的回合
        End, //游戏结束
    };
    Status status; //游戏当前的状态
    Ui::MainWindow *ui;
    //定义定时器
    //定时更新系统
    QTimer *updateSystemTimer;
    //是否有牌权
    bool isAuthor;
    //牌池的牌
    QList<Card> cardPool;
    //地主展示牌
    QList<Card> landLordBonus;
    //手牌
    //本玩家手牌
    QList<Card> hands;
    //负责监听的套接字
    QTcpServer* serverB;
    //负责通信的套接字
    QTcpSocket* clientB;
    QTcpSocket* clientC;
    //成员函数
    void paintEvent(QPaintEvent *e); //进行绘图
    void mousePressEvent(QMouseEvent *e); //对鼠标事件进行处理

private slots:
    //MainWindow类的一些槽函数
    void updateCardCoord(bool flag=0); //更新牌坐标
    void on_systemtimer_timeout () ; //系统定时溢出处理槽函数
    void slotNewConnection();
    void slotReadyReadB();
    void slotReadyReadC();
    void on_pushButtonStart_clicked();
    void on_pushButtonRun_clicked();
    void on_pushButtonNotRun_clicked();
    void on_pushButtonSubmit_clicked();
    void on_pushButtonSkip_clicked();
    void on_pushButtonRestart_clicked();
    void on_pushButtonQuit_clicked();
};
#endif // MAINWINDOW_H
