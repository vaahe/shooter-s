#include "databasemanager.h"

DatabaseManager* DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return &instance;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("shooter-s.db");

    QObject::connect(this, &DatabaseManager::connectionSucceeded, this, &DatabaseManager::createUsersTable);
    QObject::connect(this, &DatabaseManager::connectionSucceeded, this, &DatabaseManager::createResultsTable);
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

void DatabaseManager::connect() {
    if (!m_db.isOpen()) {
        if (m_db.open()) {
            qDebug() << "Connected to db";
            emit connectionSucceeded();
        } else {
            emit connectionFailed();
            qWarning() << "Failed to open database:" << m_db.lastError().text();
        }
    } else {
        qWarning() << "Database already opened";
    }
}

void DatabaseManager::disconnect() {
    if (m_db.isOpen()) {
        m_db.close();
    } else {
        qWarning() << "Database already closed";
    }
}

void DatabaseManager::createTable(const QString &tableName, const QString &tableDescription) {
    if (tableName.isEmpty() || tableDescription.isEmpty()) {
        return;
    }

    QSqlQuery query(m_db);

    const QString queryStr = QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(tableName, tableDescription);
    query.prepare(queryStr);

    if (!query.exec()) {
        qWarning() << "Failed to create table:" << query.lastError().text();
    }
    // else {
        // qDebug() << QString("Table %1 created successfully or already exists.").arg(tableName);
    // }
}

void DatabaseManager::createUsersTable() {
    const QString userTableName = "users";
    const QString usersTableDescription = QString("id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL, password TEXT NOT NULL");

    createTable(userTableName, usersTableDescription);
}

void DatabaseManager::createResultsTable() {
    const QString resultsTableName = "results";
    const QString resultsTableDescription = QString("id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                                    "result TEXT NOT NULL, "
                                                    "\"date\" TEXT NOT NULL DEFAULT (DATETIME('now')), "
                                                    "userId TEXT NOT NULL, "
                                                    "distance INTEGER NOT NULL, "
                                                    "imitationDistance INTEGER NOT NULL, "
                                                    "FOREIGN KEY (userId) REFERENCES users(id) ON DELETE CASCADE");

    createTable(resultsTableName, resultsTableDescription);
}

void DatabaseManager::insertResult(const Result &resultData) {
    QSqlQuery query(m_db);

    query.prepare("INSERT INTO results (result, userId, distance, imitationDistance, date) VALUES (:result, :userId, :distance, :imitationDistance, :date)");
    query.bindValue(":result", resultData.result);
    query.bindValue(":userId", resultData.userId);
    query.bindValue(":distance", resultData.distance);
    query.bindValue(":imitationDistance", resultData.imitationDistance);
    query.bindValue(":date", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        qWarning() << "Failed to insert result:" <<query.lastError().text();
    }
}

void DatabaseManager::signIn(const User& user) {
    const QString username = user.username;
    const QString password = user.password;

    if (username.isEmpty() || password.isEmpty()) {
        return;
    }

    QSqlQuery query(m_db);

    query.prepare("SELECT * FROM users WHERE username = :username and password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qWarning() << "Failed to execute sign-in query:" << query.lastError().text();
        emit loginFailed();
        return;
    }

    if (query.next()) {
        qDebug() << "line 116:" << query.value(0);

        emit loginSucceeded();
    } else {
        emit loginFailed();
    }
}

void DatabaseManager::signUp(const User &user) {
    const QString username = user.username;
    const QString password = user.password;

    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    checkQuery.bindValue(":username", username);

    if (!checkQuery.exec()) {
        qWarning() << "Failed to check username existence:" << checkQuery.lastError().text();
        emit registerFailed();
        return;
    }

    checkQuery.next();
    if (checkQuery.value(0).toInt() > 0) {
        emit registerFailed();
        return;
    }

    QSqlQuery query(m_db);

    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        qWarning() << "Failed to execute sign-up query:" << query.lastError().text();
        emit registerFailed();
    } else {
        emit registerSucceeded();
    }
}

QList<Result> DatabaseManager::getResults(const QString& userId) {
    QList<Result> results;
    QSqlQuery query(m_db);

    query.prepare("SELECT * FROM results WHERE userId = :userId ORDER BY date DESC");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qWarning() << "Failed to retrieve results by" << userId;
        return results;
    }

    while (query.next()) {
        Result resultData;
        resultData.result = query.value("result").toString();
        resultData.userId = query.value("userId").toString();
        resultData.distance = query.value("distance").toInt();
        resultData.imitationDistance = query.value("imitationDistance").toInt();
        resultData.date = query.value("date").toString();

        results.append(resultData);
    }

    return results;
}

QList<Result> DatabaseManager::getResultsByRange(const QString& userId, const QDate &startDate, const QDate &endDate) {
    QList<Result> resultsByRange;

    if (startDate.isNull() || endDate.isNull()) {
        return resultsByRange;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM results WHERE userId = :userId AND date BETWEEN :startDate and :endDate");
    query.bindValue(":userId", userId);
    query.bindValue(":startDate", startDate);
    query.bindValue(":endDate", endDate);

    if (!query.exec()) {
        qWarning() << "Failed to retrieve results within range" << startDate << "to" << endDate << query.lastError().text();
        return resultsByRange;
    }

    while (query.next()) {
        Result resultData;
        resultData.result = query.value("result").toString();
        resultData.userId = query.value("userId").toString();
        resultData.distance = query.value("distance").toInt();
        resultData.imitationDistance = query.value("imitationDistance").toInt();
        resultData.date = query.value("date").toString();

        resultsByRange.append(resultData);
    }

    return resultsByRange;
}
