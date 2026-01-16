#ifndef DBTEST_H
#define DBTEST_H

#include "src/database/databasemanager.h"

#include <QObject>
#include <QtTest/QtTest>

class DBTest : public QObject
{
    Q_OBJECT

public:
    explicit DBTest(QObject *parent = nullptr);

private slots:
    // void testDBConnection();
    // void testDBDisconnection();

    // void testCreateUser();
    // void testGetResults();

signals:
};

#endif // DBTEST_H
