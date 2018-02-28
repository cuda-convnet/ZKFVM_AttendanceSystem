#include "m_db.h"

m_Db::m_Db(QObject *parent) : QObject(parent)
{

}


bool m_Db::init()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("MyDataBase.db");

        //set tables
        database.open();
        if(database.tables().isEmpty())
        {
            setTables();
            addAdmin("admin","admin",0);
        }
    }
    database.close();
    return 1;
}

void m_Db::setTables()
{
    qDebug()<<"setting";
    database.open();
    QSqlQuery sql_query;
    QString create_dataTable = "create table data (ID INTEGER PRIMARY KEY   AUTOINCREMENT , USERID int(32), FP1 blob(2048),FP2 blob(2048),FP3 blob(2048),FV1 blob(2048),FV2 blob(2048),FV3 blob(2048))";
    QString create_userTable = "create table user (USERID INTEGER PRIMARY KEY   AUTOINCREMENT , name varchar(32),CREATEBY int(32))";
    QString create_adminTable = "create table admin (ID INTEGER PRIMARY KEY   AUTOINCREMENT , USERNAME varchar(32),PASSWD varchar(32), CREATEBY int(32))";
    QString create_logTable="create table log (ID INTEGER PRIMARY KEY   AUTOINCREMENT , USERID int(32),DATETIME datetime)";

    sql_query.prepare(create_dataTable);
    if(!sql_query.exec())
    {
        qDebug() << "Error: Fail to create create_dataTable." << sql_query.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }

    sql_query.prepare(create_userTable);
    if(!sql_query.exec())
    {
        qDebug() << "Error: Fail to create create_userTable." << sql_query.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }

    sql_query.prepare(create_adminTable);
    if(!sql_query.exec())
    {
        qDebug() << "Error: Fail to create create_adminTable." << sql_query.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }

    sql_query.prepare(create_logTable);
    if(!sql_query.exec())
    {
        qDebug() << "Error: Fail to create create_logTable." << sql_query.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }

    database.close();
}

void m_Db::addUser(QString USERNAME, qint32 adminID)
{
    qDebug()<<"addUser"<<USERNAME<<adminID;
    database.open();
    QString strSql = "INSERT INTO user (USERID,name,CREATEBY) VALUES (null,:v1,:v2);";
    QSqlQuery query;
    query.prepare(strSql);
    query.bindValue(":v1", USERNAME);
    query.bindValue(":v2", adminID);
    query.exec();
    database.close();
}

void m_Db::addData(qint32 USERID, unsigned char **m_preRegFPTmps, unsigned char **m_preRegFVTmps)
{
    database.open();

    QByteArray FP1((const char *)m_preRegFPTmps[0],2048);
    QByteArray FP2((const char *)m_preRegFPTmps[1],2048);
    QByteArray FP3((const char *)m_preRegFPTmps[2],2048);

    QByteArray FV1((const char *)m_preRegFVTmps[0],2048);
    QByteArray FV2((const char *)m_preRegFVTmps[1],2048);
    QByteArray FV3((const char *)m_preRegFVTmps[2],2048);

    QString strSql = "INSERT INTO data (USERID,FP1,FP2,FP3,FV1,FV2,FV3)\
            VALUES (:id,:v1,:v2,:v3,:v4,:v5,:v6);";

    QSqlQuery query;
    query.prepare(strSql);
    query.bindValue(":id", USERID);
    query.bindValue(":v1", FP1);
    query.bindValue(":v2", FP2);
    query.bindValue(":v3", FP3);
    query.bindValue(":v4", FV1);
    query.bindValue(":v5", FV2);
    query.bindValue(":v6", FV3);

    query.exec();

    database.close();
}

void m_Db::addAdmin(QString UserName, QString PassWd, qint32 createBy)
{

    database.open();

    QString strSql = "INSERT INTO admin (ID,USERNAME,PASSWD,CREATEBY)\
            VALUES (null,:v1,:v2,:v3);";

    QSqlQuery query;
    query.prepare(strSql);
    query.bindValue(":v1", UserName);
    query.bindValue(":v2", PassWd);
    query.bindValue(":v3", createBy);

    query.exec();
    database.close();
}

void m_Db::addLog(qint32 USERID)
{
    qDebug()<<"addLog";
    database.open();

    QString strSql = "INSERT INTO log (ID,USERID,DATETIME)\
            VALUES (null,:v1,:v2);";

    QSqlQuery query;
    query.prepare(strSql);
    query.bindValue(":v1", USERID);
    query.bindValue(":v2", QDateTime::currentDateTime().toString());

    query.exec();

    database.close();
}

qint32 m_Db::getAdminId(QString UserName, QString PassWd)
{
    database.open();

    QString strSql = "SELECT ID FROM admin WHERE USERNAME = :v1 AND PASSWD = :v2 ";

    QSqlQuery query;
    query.prepare(strSql);
    query.bindValue(":v1", UserName);
    query.bindValue(":v2", PassWd);

    query.exec();

    query.first();
    if(query.value(0).toString().isEmpty())
    {
        database.close();
        return -1;
    }else
    {
        database.close();
        return query.value(0).toInt();
    }

    database.close();

}

qint32 m_Db::getUserId(QString UserName)
{
    database.open();

    QString strSql = "SELECT USERID FROM user WHERE name = :v1 ";

    QSqlQuery query;
    query.prepare(strSql);
    query.bindValue(":v1", UserName);

    query.exec();

    query.first();
    if(query.value(0).toString().isEmpty())
    {
        database.close();
        return -1;
    }else
    {
        database.close();
        return query.value(0).toInt();
    }

    database.close();

}

QVector<QVariantMap> m_Db::getAllData()
{
    qDebug()<<"getAllData";
    QVector<QVariantMap> allData;
    QVariantMap oneData;
    database.open();


    QString strSql = "SELECT * FROM data ";
    //*********************************
    // * 有毒，不知道取的数据那个是那个
    QSqlQuery query;
    query.prepare(strSql);
    if(!query.exec())
    {
        qDebug()<<query.lastError();
    }
    else
    {
        while(query.next())
        {
            int id = query.value(1).toInt();
            QByteArray FP1=query.value(2).toByteArray();
            QByteArray FP2=query.value(3).toByteArray();
            QByteArray FP3=query.value(4).toByteArray();
            QByteArray FV1=query.value(5).toByteArray();
            QByteArray FV2=query.value(6).toByteArray();
            QByteArray FV3=query.value(7).toByteArray();

            oneData.clear();
            oneData.insert("id",id);
            oneData.insert("FP1",FP1);
            oneData.insert("FP2",FP2);
            oneData.insert("FP3",FP3);
            oneData.insert("FV1",FV1);
            oneData.insert("FV2",FV2);
            oneData.insert("FV3",FV3);

            allData.append(oneData);
        }
    }
    database.close();

qDebug()<<"return allData;";
    return allData;


}

QVector<QVariantMap> m_Db::getALLLog()
{

    qDebug()<<"getAllLog";
    QVector<QVariantMap> allLog;
    QVariantMap oneLog;
    database.open();

    QString strSql="SELECT user.name,log.DATETIME FROM user,log WHERE user.USERID=log.USERID";

    //*********************************
    // * 有毒，不知道取的数据那个是那个
    QSqlQuery query;
    query.prepare(strSql);
    if(!query.exec())
    {
        qDebug()<<query.lastError();
    }
    else
    {
        while(query.next())
        {
            QString name = query.value(0).toString();
            QString datetime=query.value(1).toString();
            qDebug()<<"QDateTime datetime=query.value(1).toDateTime();"<<datetime;

            oneLog.clear();
            oneLog.insert("name",name);
            oneLog.insert("datetime",datetime);

            allLog.append(oneLog);
        }
    }
    database.close();
    qDebug()<<"return allLog;";
    return allLog;



}

qint32 m_Db::getIdByUSERNAME(QString USERNAME)
{
    QSqlQuery query;
    QString strSql = "select USERID from user where USERNAME=:name";
    query.prepare(strSql);
    query.bindValue(":name", USERNAME);

    qint32 USERID = query.value(0).toInt();
    return USERID;
}
