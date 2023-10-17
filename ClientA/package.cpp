#include<package.h>

void sendPackage(QTcpSocket *sender, QByteArray sendByte)
{
    //要传的数据
    QByteArray msg;
    //绑定字节流
    QDataStream out(&msg, QIODevice::WriteOnly);
    //设置大端模式
    out.setByteOrder(QDataStream::BigEndian);
    //占位符,先占位，然后算出整体长度再插入
    out << ushort(0) << sendByte;
    //回到文件开头，插入真实的长度
    out.device()->seek(0);
    ushort len = (ushort)(sendByte.size());
    out << len;
    sender->write(msg);
    //必要的时候可以flush
    //sender->flush();
}
