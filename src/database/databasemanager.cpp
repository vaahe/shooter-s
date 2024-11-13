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
                                                    "userId INTEGER NOT NULL, "
                                                    "distance INTEGER NOT NULL, "
                                                    "imitationDistance INTEGER NOT NULL, "
                                                    "FOREIGN KEY (userId) REFERENCES users(id) ON DELETE CASCADE");

    createTable(resultsTableName, resultsTableDescription);
}

void DatabaseManager::insertResult(const Result &data) {
    QSqlQuery query(m_db);

    query.prepare("INSERT INTO results (result, userId, distance, imitationDistance) VALUES (:result, :userId, :distance, :imitationDistance)");
    query.bindValue(":result", data.result);
    query.bindValue(":userId", data.userId);
    query.bindValue(":distance", data.distance);
    query.bindValue(":imitationDistance", data.imitationDistance);

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
