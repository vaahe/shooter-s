#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QMetaType>
#include <QSqlDatabase>

namespace Database {
    struct Result {
        QString result;
        QString userId;
        int distance;
        int imitationDistance;
        QString date;
    };

    struct User {
        QString username;
        QString password;
    };
}


using Database::Result;
using Database::User;

Q_DECLARE_METATYPE(Result)


class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    static DatabaseManager* getInstance();

public slots:
    void connect();
    void disconnect();

    void createTable(const QString& tableName, const QString& tableDescription);
    void createUsersTable();
    void createResultsTable();

    void signIn(const User& user);
    void signUp(const User& user);

    QList<Result> getResults(const QString& userId);
    QList<Result> getResultsByRange(const QString& userId, const QDate& startDate, const QDate& endDate);
    void insertResult(const Result& data);

signals:
    void loginFailed();
    void loginSucceeded();

    void registerFailed();
    void registerSucceeded();

    void connectionFailed();
    void connectionSucceeded();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
