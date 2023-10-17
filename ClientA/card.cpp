#include "card.h"


Card::Card(int number, Card::Color color, Card::Joker joker, int status, QList<int> pos):
    chosen(0),
    number(number),
    color(color),
    joker(joker),
    status(status),
    pos(pos)
{

}

int Card::getNumber()
{
    return number;
}

Card::Color Card::getColor()
{
    return color;
}

Card::Joker Card::getJoker()
{
    return joker;
}

int Card::getStatus()
{
    return status;
}

QList<int> Card::getPos()
{
    return pos;
}

void Card::setPos(QList<int> pos_)
{
    pos = pos_;
}

void Card::setStatus(int state)
{
    status = state;
}

QString Card::getPicturePath()
{
    QString path = "..//Pictures//";
    //如果是反面直接返回反面图片
    if(!status)
    {
        return path + "PADDING.png";
    }

    //如果是大小王直接返回对应图片
    if(joker == black)
    {
        return path + "BLACK JOKER.png";
    }
    else if(joker == red)
    {
        return path + "RED JOKER.png";
    }

    //决定花色
    switch (color)
    {
        case Clubs:
            path += "C";
            break;
        case Spade:
            path += "S";
            break;
        case Diamond:
            path += "D";
            break;
        case Heart:
            path += "H";
            break;
        case None:
            qDebug() << "Error Color!";
    }
    //决定数字
    if(1 <= number && number <= 8)
    {
        path += QString::number(number + 2);
    }
    else if(number == 9)
    {
        path += "J";
    }
    else if(number == 10)
    {
        path += "Q";
    }
    else if(number == 11)
    {
        path += "K";
    }
    else if(number == 12)
    {
        path += "A";
    }
    else if(number == 13)
    {
        path += "2";
    }
    return path + ".png";
}

void Card::fromString(QString str)
{
    QStringList lis = str.split(",");
    if(lis[0] == "R")
    {
        number = -1;
        color = None;
        joker = red;
    }
    else if(lis[0] == "B")
    {
        number = -1;
        color = None;
        joker = black;
    }
    else
    {
        number = lis[1].toInt();
        if(lis[2] == "C")
        {
            color = Clubs;
        }
        else if(lis[2] == "D")
        {
            color = Diamond;
        }
        else if(lis[2] == "H")
        {
            color = Heart;
        }
        else if(lis[2] == "S")
        {
            color = Spade;
        }
        joker = none;
    }
}

QString Card::toString()
{
    if(joker == red)
    {
        return QString("R,-1,N");
    }
    else if(joker == black)
    {
        return QString("B,-1,N");
    }
    else
    {
        QString num_str = QString::number(number);
        QString colo_str = "";
        if(color == Clubs)
        {
            colo_str = "C";
        }
        else if(color == Diamond)
        {
            colo_str = "D";
        }
        else if(color == Heart)
        {
            colo_str = "H";
        }
        else if(color == Spade)
        {
            colo_str = "S";
        }
        return QString("N," + num_str + "," + colo_str);
    }
}

int Card::isShunZi(QList<Card> cards)
{
    if(cards.size() < 5 || cards.size() > 12)
        return 0;
    QList<int> nums;
    for(int i = 0;i < cards.size(); i++)
    {
        if(cards[i].number == 13 || cards[i].joker == red || cards[i].joker == black)
            return 0;
        nums.append(cards[i].number);
    }
    std::sort(nums.begin(), nums.end());
    for(int i = 1;i < nums.size(); i++)
    {
        if(nums[i] != (nums[i-1] + 1))
            return 0;
    }
    return 1;
}

int Card::isShuangShun(QList<Card> cards)
{
    if(cards.size() % 2 == 1 || cards.size() < 6 || cards.size() > 20)
        return 0;
    QList<int> nums;
    for(int i = 0;i < cards.size(); i++)
    {
        if(cards[i].number == 13 || cards[i].joker == red || cards[i].joker == black)
            return 0;
        nums.append(cards[i].number);
    }
    std::sort(nums.begin(), nums.end());
    for(int i = 1;i < nums.size(); i++)
    {
        if(i % 2)
        {
            if(nums[i] != nums[i-1])
                return 0;
        }
        else
        {
            if(nums[i] != (nums[i-1] + 1))
                return 0;
        }
    }
    return 1;
}

int Card::isThree(QList<Card> cards, int *num)
{
    //只考虑三带一、三带二和三带
    //数量不对直接返回-1
    if(cards.size() < 3 || cards.size() > 5)
        return -1;

    QList<int> nums;
    for(int i = 0;i < cards.size(); i++)
    {
        nums.append(cards[i].number);
    }
    bool ok = false;
    int dui = 0;
    for(int i = 0;i < nums.size(); i++)
    {
        if(nums.count(nums[i]) == 3)
        {
            //确实有三个一样的
            if(num != NULL)
                *num = nums[i];
            ok = true;
        }
        else if(nums.count(nums[i]) == 2)
        {
            dui++;
        }
    }
    //没有三个一样的直接返回
    if(!ok)
        return -1;

    if(cards.size() == 3)
        return 0;
    else
    {
        //必须是三带一对才行
        if(dui == 2)
            return 2;
        else if(cards.size() == 4)
            return 1;
        else
            return -1;
    }
}

int Card::isFour(QList<Card> cards, int *num)
{
    //只考虑四带二、四带二对
    //数量不对直接返回-1
    if(cards.size() != 6 && cards.size() != 8)
        return -1;

    QList<int> nums;
    for(int i = 0;i < cards.size(); i++)
    {
        nums.append(cards[i].number);
    }
    std::sort(nums.begin(), nums.end());
    bool ok = false;
    int dui = 0;
    int four = 0;
    for(int i = 0;i < nums.size(); i++)
    {
        if(nums.count(nums[i]) == 4)
        {
            //确实有四个一样的
            if(num != NULL)
                *num = nums[i];
            ok = true;
            four++;
        }
        else if(nums.count(nums[i]) == 2)
        {
            dui++;
        }
    }
    //没有四个一样的直接返回
    if(!ok)
        return -1;

    if(cards.size() == 6)
        return 2;
    else
    {
        //必须是四带两对才行
        if(dui == 4 || four == 8)
            return 4;
        else
            return -1;
    }
}

int Card::isPlane(QList<Card> cards, int *num)
{
    QList<int> nums;
    QList<int> uniq_nums;
    for(int i = 0;i < cards.size(); i++)
    {
        nums.append(cards[i].number);
    }
    //获得不重复的序列
    uniq_nums = nums.toSet().toList();
    //找出所有出现次数大于等于3的牌
    QList<int> three_nums;
    for(int i = 0;i < uniq_nums.size(); i++)
    {
        if(nums.count(uniq_nums[i]) >= 3 && uniq_nums[i] != 13)
        {
            three_nums.append(uniq_nums[i]);
        }
    }
    //三条部分不足两个，返回负一
    if(three_nums.size() < 2)
        return -1;
    //找出three_nums中的最长递增子序列
    std::sort(three_nums.begin(), three_nums.end());
    //表示[r,l]区间
    int r = 0,l = 0;
    int ans_r = 0,ans_l = 0;
    for(int i = 0;i < three_nums.size();i++)
    {
        r = i,l = i;
        for(int j = i + 1;j < three_nums.size();j++)
        {
            if(three_nums[j] != three_nums[j-1] + 1)
                break;
            else
                l = j;
        }
        if(l - r + 1 >= ans_l -ans_r + 1)
        {
            ans_l = l;
            ans_r = r;
        }
    }
    //三条部分不足两个，返回负一
    if(ans_l - ans_r + 1 < 2)
        return -1;
    //抽出真正的三条部分
    for(int i = ans_r;i <= ans_l;i++)
    {
        //删除相应的三条
        nums.removeOne(three_nums[i]);
        nums.removeOne(three_nums[i]);
        nums.removeOne(three_nums[i]);
    }
    //分不同的三条情况
    if(num != NULL)
        *num = three_nums[ans_l];
    if(nums.size() == 0)
    {
        //全为三条就是无翼飞机
        return 0;
    }
    else if(nums.size() == ans_l - ans_r + 1)
    {
        //全为三带一
        return 1;
    }
    else if(nums.size() == (ans_l - ans_r + 1)*2)
    {
        //有可能全为三带二
        std::sort(nums.begin(), nums.end());
        //如果两两成对
        bool ok = true;
        for(int i = 1;i < nums.size();i += 2)
        {
            if(nums[i] != nums[i-1])
            {
                ok = false;
                break;
            }
        }
        if(ok)
            return 2;
        else
            return -1;
    }
    else
    {
        return -1;
    }
}


bool Card::isOK2Submit(QList<Card> my_cards, QList<Card> opponent_cards)
{
    //考虑王炸的情况
    //本家若为王炸，直接返回正确
    if(my_cards.size() == 2 &&
       ((my_cards[0].joker == red && my_cards[1].joker == black) ||
        (my_cards[1].joker == red && my_cards[0].joker == black)))
    {
        return 1;
    }
    //对面若为王炸，直接返回错误
    if(opponent_cards.size() == 2 &&
       ((opponent_cards[0].joker == red && opponent_cards[1].joker == black) ||
        (opponent_cards[1].joker == red && opponent_cards[0].joker == black)))
    {
        return 0;
    }

    //考虑炸弹的情况
    //本家若为炸弹，看对面牌型
    if(my_cards.size() == 4 &&
        my_cards[0].number == my_cards[1].number &&
        my_cards[1].number == my_cards[2].number &&
        my_cards[2].number == my_cards[3].number
       )
    {
        bool is_bomb = (opponent_cards.size() == 4 &&
                        opponent_cards[0].number == opponent_cards[1].number &&
                        opponent_cards[1].number == opponent_cards[2].number &&
                        opponent_cards[2].number == opponent_cards[3].number
                       );
        //如果对面不是炸弹也不是王炸，直接返回正确
        if(!is_bomb)
            return 1;
        //对面有炸弹，看谁的大
        if(my_cards[0].number > opponent_cards[0].number)
            return 1;
        else
            return 0;
    }
    //对面是炸弹，本家既不是炸弹也不是王炸，那么就返回错误
    if(opponent_cards.size() == 4 &&
            opponent_cards[0].number == opponent_cards[1].number &&
            opponent_cards[1].number == opponent_cards[2].number &&
            opponent_cards[2].number == opponent_cards[3].number
           )
    {
        return 0;
    }

    //下面是其他牌型的情况
    //本家和对面都没有王炸和炸弹
    if(opponent_cards.size() == 1)
    {
        //上家牌型是单张的情况
        if(my_cards.size() != 1)
        {
            //本家牌型不是单张，返回错误
            return 0;
        }
        else
        {
            //本家和上家都是单张
            if(opponent_cards[0].joker == red)
            {
                //上家出大王，本家必出不来
                return 0;
            }
            else if(opponent_cards[0].joker == black)
            {
                //上家出小王，本家只能出大王
                return my_cards[0].joker == red;
            }
            else
            {
                //上家无王
                //本家有王必赢
                if(my_cards[0].joker == red || my_cards[0].joker == black)
                    return 1;
                //否则比大小
                else
                    return my_cards[0].number > opponent_cards[0].number;
            }
        }
    }
    else if(opponent_cards.size() == 2)
    {
        //上家牌型为一对
        //本家不是一对，返回错误
        if(my_cards.size() != 2 || (my_cards[0].number != my_cards[1].number))
            return 0;
        //否则比大小
        else
            return my_cards[0].number > opponent_cards[0].number;
    }
    else if(isShunZi(opponent_cards))
    {
        //如果上家为顺子(非顺子返回0)
        //本家为顺子且牌数一样才能跟
        return isShunZi(my_cards) && (opponent_cards.size() == my_cards.size()) && (my_cards[0].number > opponent_cards[0].number);
    }
    else if(isShuangShun(opponent_cards))
    {
        //如果上家为双顺
        //本家为双顺且牌数一样才能跟
        return isShuangShun(my_cards) && (opponent_cards.size() == my_cards.size()) && (my_cards[0].number > opponent_cards[0].number);
    }
    else if(isThree(opponent_cards) != -1)
    {
        //如果上家为三带
        //本家也要出和对面一样的三带
        //获得三条部分的数值
        int a,b;
        int *my = &a, *opp = &b;
        if(isThree(opponent_cards, opp) != isThree(my_cards, my))
            return 0;
        else
            return *my > *opp;
    }
    else if(isFour(opponent_cards) != -1)
    {
        //如果上家为四带
        //本家也要出和对面一样的四带
        //获得四条部分的数值
        int a,b;
        int *my = &a, *opp = &b;
        if(isFour(opponent_cards, opp) != isFour(my_cards, my))
            return 0;
        else
            return *my > *opp;
    }
    else if(isPlane(opponent_cards) != -1)
    {
        //如果上家为飞机
        //本家也要出和对面一样的飞机
        //获得飞机部分的数值
        int a,b;
        int *my = &a, *opp = &b;
        if(isPlane(opponent_cards, opp) != isPlane(my_cards, my) ||
                opponent_cards.size() != my_cards.size())
            return 0;
        else
            return *my > *opp;
    }
    qDebug() << "上家牌型不合法！";
    return 0;
}

bool Card::isOK2Submit(QList<Card> my_cards)
{
    //判断拥有牌权时出牌情况
    if(my_cards.size() == 1)
    {
        //单张可出
        qDebug() << "单张";
        return 1;
    }
    else if(my_cards.size() == 2 &&
            ((my_cards[0].joker == red && my_cards[1].joker == black) ||
             (my_cards[1].joker == red && my_cards[0].joker == black)))
    {
        //王炸可出
        qDebug() << "王炸";
        return 1;
    }
    else if(my_cards.size() == 4 &&
        my_cards[0].number == my_cards[1].number &&
        my_cards[1].number == my_cards[2].number &&
        my_cards[2].number == my_cards[3].number
       )
    {
        //炸弹可出
        qDebug() << "炸弹";
        return 1;
    }
    else if(my_cards.size() == 2 && (my_cards[0].number == my_cards[1].number))
    {
        //对子可出
        qDebug() << "对子";
        return 1;
    }
    else if(isThree(my_cards) != -1)
    {
        //三带可出
        qDebug() << "三带";
        return 1;
    }
    else if(isFour(my_cards) != -1)
    {
        //四带可出
        qDebug() << "四带";
        return 1;
    }
    else if(isShunZi(my_cards))
    {
        //顺子可出
        qDebug() << "顺子";
        return 1;
    }
    else if(isShuangShun(my_cards))
    {
        //双顺可出
        return 1;
    }
    else if(isPlane(my_cards) != -1)
    {
        //飞机可出
        return 1;
    }
    //不可出
    return 0;
}

bool Card::operator==(Card other)
{
    if(number == other.number && color == other.color && joker == other.joker)
        return 1;
    return 0;
}


bool Card::operator<(Card other)
{
    return number < other.number;
}
