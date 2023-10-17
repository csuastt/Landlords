#ifndef PACKAGE_H
#define PACKAGE_H

#include <QtNetwork>

extern void sendPackage(QTcpSocket *sender, QByteArray sendByte);

#endif // PACKAGE_H
