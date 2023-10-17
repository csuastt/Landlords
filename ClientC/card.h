#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QDebug>

class Card
{
public:
    //定义枚举变量
    //花色
    enum Color
    {
        Clubs,
        Diamond,
        Heart,
        Spade,
        None //None表示无花色，用于大小王
    };
    //大王小王
    enum Joker
    {
        none,
        black,
        red
    };

public:
    explicit Card(int number=-1,
                  Color color=None,
                  Joker joker=none,
                  int status=1,
                  QList<int> pos={0,0});
    //定义接口函数
    int getNumber();
    Color getColor();
    Joker getJoker();
    int getStatus();
    QList<int> getPos();
    void setPos(QList<int> pos_);
    void setStatus(int state);
    //获得图片的路径
    QString getPicturePath();
    //依靠字符串设置卡牌的信息
    //例如"N,8,C"表示梅花10
    //"R,-1,N"表示大王
    void fromString(QString str);
    //将当前卡牌的信息输出为字符串
    QString toString();
    //静态成员函数，判断当前牌型是否合法
    //1为合法，0为不合法
    static int isShunZi(QList<Card> cards); //判断顺子
    static int isShuangShun(QList<Card> cards); //判断双顺
    static int isThree(QList<Card> cards, int *num = NULL); //判断三带
    static int isFour(QList<Card> cards, int *num = NULL); //判断四带
    static int isPlane(QList<Card> cards, int *num = NULL); //判断飞机
    static bool isOK2Submit(QList<Card> my_cards, QList<Card> opponent_cards);
    static bool isOK2Submit(QList<Card> my_cards);
    //选中状态
    //1为选中，0为未选中
    //是否被选中是公有可见的量
    bool chosen;
    //重载==运算符
    bool operator==(Card other);
    //重载<运算符
    bool operator<(Card other);

private:
    //卡牌类成员变量
    //数字
    //345678910JQKA2
    //依次对应12345678910111213
    //-1表示无号码，用于大小王
    int number;
    //花色
    Color color;
    //大小王
    Joker joker;
    //正反状态
    //0为反面，1为正面
    int status;
    //位置
    QList<int> pos;
};

#endif // CARD_H
