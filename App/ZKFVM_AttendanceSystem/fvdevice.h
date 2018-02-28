#ifndef FVDEVICE_H
#define FVDEVICE_H

//常量
#define ENROLL_CNT	3
#define HEIGHT_CAPTION 56
#define SHOWIMG_WIDTH  256
#define SHOWIMG_HEIGHT  320

//include windows API
#include <windows.h>

//include Qt libraries
#include <QString>
#include <QImage>
#include <QDebug>



//include ZK

#include "zkfvapi.h"
#include "zkfvapierrdef.h"
#include "zkfvapitype.h"

#include "m_db.h"


using namespace std;

class FVDevice:public QObject
{
    Q_OBJECT
    static DWORD WINAPI ThreadCapture(LPVOID lParam);
signals:
    void pressed(char *);
    void restTimes(int);
    void regOver();
    void logged(char*);//已注册手指按压
public slots:
    void on_logged(char *);

public:
    m_Db database;
    qint32 newUserId;

    static char * newimg;

public:

    HANDLE m_hDevice = NULL;
    HANDLE m_hDBCache = NULL;
    HANDLE m_hThreadWork = NULL;
    int m_imgFPWidth;
    int m_imgFPHeight;
    int m_imgFVWidth;
    int m_imgFVHeight;

    BOOL m_bIdentify;
    BOOL m_bStopThread;

    QString m_strFingerID;

    int GetIdentifyMode();

    unsigned char** m_preRegFPTmps;
    int  m_nPreRegFPTmpSize[3];
    unsigned char** m_preRegFVTmps;
    int  m_nPreRegFVTmpSize[3];
    int m_nEnrollIndex;

    //verify sample
    unsigned char** m_lastRegFPTmps;
    int  m_nlastRegFPTmpSize[3];
    unsigned char** m_lastRegFVTmps;
    int  m_nlastRegFVTmpSize[3];

    //登记图片的存放
    unsigned char ** m_regFpImage;
    int  m_nregFpImageSize[3];
    unsigned char ** m_regFvImage;
    int  m_nregFvImageSize[3];

    BOOL m_bRegister;
    void DoRegister(unsigned char* fpTmp, int cbFPTmp, unsigned char* fvTmp, int cbFVTmp, unsigned char * fpImg, int fpImgSize, unsigned char * fvImg, int fvImgSize);
    void DoVerify(unsigned char* fpTmp, int cbFPTmp, unsigned char* fvTmp, int cbFVTmp);

    LRESULT m_Init();
    LRESULT m_Conn();
    LRESULT m_DisConn();
    LRESULT m_Enroll(qint32 USERNAME);
    LRESULT m_Verify();
    LRESULT m_Identify();
    LRESULT m_Clear();

    int saveToDataBase(qint32 USERID, unsigned char **m_preRegFPTmps, unsigned char **m_preRegFVTmps);

    int loadFromDataBase();

};

#endif // FVDEVICE_H
