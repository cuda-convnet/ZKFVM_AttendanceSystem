#ifndef M_DB_H
#define M_DB_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSql>
#include <QSqlError>
#include <QDateTime>

class m_Db : public QObject
{
    Q_OBJECT
public:
    explicit m_Db(QObject *parent = 0);

public:
    bool init();

    void addUser(QString USERNAME,qint32 adminID);
    void addData(qint32 USERID, unsigned char **m_preRegFPTmps, unsigned char **m_preRegFVTmps);
    void addAdmin(QString UserName,QString PassWd,qint32 createBy);
    void addLog(qint32 USERID);

    qint32 getAdminId(QString UserName,QString PassWd);
    qint32 getUserId(QString UserName);

    QVector<QVariantMap> getAllData();
    QVector<QVariantMap> getALLLog();

private:
    QSqlDatabase database;
    void setTables();
    qint32 getIdByUSERNAME(QString USERNAME);

};

#endif // M_DB_H
