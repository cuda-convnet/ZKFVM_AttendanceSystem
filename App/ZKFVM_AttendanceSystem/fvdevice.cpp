#include "fvdevice.h"

#include "QDebug"


char * FVDevice::newimg=new char[320*240];

LRESULT FVDevice::m_Conn()
{
    if (NULL == m_hDevice)
    {
        if (ZKFingerVein_Init() != ZKFV_ERR_OK)
        {
            qDebug() << "Init ZKFVM fail";
            return S_OK;
        }
        if ((m_hDevice = ZKFingerVein_OpenDevice(0)) == NULL)
        {
            qDebug() << "Open sensor fail" << endl;
            ZKFingerVein_Terminate();
            return S_OK;
        }
        m_hDBCache = ZKFingerVein_DBInit(m_hDevice);
        if (NULL == m_hDBCache)
        {
            qDebug() << "Create DBCache fail" << endl;
            ZKFingerVein_CloseDevice(m_hDevice);
            ZKFingerVein_Terminate();
            return S_OK;
        }

        int nFullWidth = 0;
        int nFullHeight = 0;
        int retSize = sizeof(int);
        ZKFingerVein_GetParameter(m_hDevice, 1, (unsigned char*)&nFullWidth, &retSize);
        retSize = sizeof(int);
        ZKFingerVein_GetParameter(m_hDevice, 2, (unsigned char*)&nFullHeight, &retSize);

        m_preRegFPTmps = new unsigned char*[ENROLL_CNT];
        m_preRegFVTmps = new unsigned char*[ENROLL_CNT];
        m_lastRegFPTmps = new unsigned char*[ENROLL_CNT];
        m_lastRegFVTmps = new unsigned char*[ENROLL_CNT];
        m_regFpImage = new unsigned char*[ENROLL_CNT];
        m_regFvImage = new unsigned char*[ENROLL_CNT];

        m_imgFPWidth = nFullWidth & 0xFFFF;
        m_imgFPHeight = nFullHeight & 0xFFFF;
        m_imgFVWidth = (nFullWidth >> 16) & 0xFFFF;
        m_imgFVHeight = (nFullHeight >> 16) & 0xFFFF;

        for (int i = 0; i < ENROLL_CNT; i++)
        {
            m_preRegFPTmps[i] = new unsigned char[MAX_TEMPLATE_SIZE];
            m_preRegFVTmps[i] = new unsigned char[MAX_TEMPLATE_SIZE];
            m_lastRegFPTmps[i] = new unsigned char[MAX_TEMPLATE_SIZE];
            m_lastRegFVTmps[i] = new unsigned char[MAX_TEMPLATE_SIZE];
            m_regFpImage[i] = new unsigned char[m_imgFPWidth * m_imgFPHeight];
            m_regFvImage[i] = new unsigned char[m_imgFVWidth * m_imgFVHeight];
        }

        memset(m_nPreRegFPTmpSize, 0x0, ENROLL_CNT * sizeof(int));
        memset(m_nPreRegFVTmpSize, 0x0, ENROLL_CNT * sizeof(int));
        memset(m_nlastRegFPTmpSize, 0x0, ENROLL_CNT * sizeof(int));
        memset(m_nlastRegFVTmpSize, 0x0, ENROLL_CNT * sizeof(int));
        memset(m_nregFpImageSize, 0x0, ENROLL_CNT * sizeof(int));
        memset(m_nregFvImageSize, 0x0, ENROLL_CNT * sizeof(int));

        m_nEnrollIndex = 0;
        m_hThreadWork = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadCapture, this, 0, NULL);


        qDebug() << "Init Succ" << endl;
        m_strFingerID.clear();

        //显示SDK版本号
        char cVerion[10] = { 0x0 };
        int leng = 10;
        if (ZKFingerVein_GetSDKVersion(cVerion, &leng) == ZKFV_ERR_OK)
        {
            qDebug() << "Init Succ，Version is " << cVerion << endl;
        }
    }
    else
    {
        qDebug() << "Already Init" << endl;
    }
    return S_OK;
}


LRESULT FVDevice::m_DisConn()
{
    if (NULL != m_hDevice)
    {
        m_bStopThread = TRUE;
        if (NULL != m_hThreadWork)
        {
            WaitForSingleObject(m_hThreadWork, INFINITE);
            CloseHandle(m_hThreadWork);
            m_hThreadWork = NULL;
        }
        if (NULL != m_hDBCache)
        {
            ZKFingerVein_DBFree(m_hDBCache);
            m_hDBCache = NULL;
        }
        ZKFingerVein_CloseDevice(m_hDevice);
        ZKFingerVein_Terminate();
        m_hDevice = NULL;
        qDebug() << "Close Succ" << endl;

        if (NULL != m_preRegFPTmps)
        {
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                delete[] m_preRegFPTmps[i];
            }
            delete[] m_preRegFPTmps;
            m_preRegFPTmps = NULL;
        }

        if (NULL != m_preRegFVTmps)
        {
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                delete[] m_preRegFVTmps[i];
            }
            delete[] m_preRegFVTmps;
            m_preRegFVTmps = NULL;
        }

        if (NULL != m_regFpImage)
        {
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                delete[] m_regFpImage[i];
            }
            delete[] m_regFpImage;
            m_regFpImage = NULL;
        }

        if (NULL != m_regFvImage)
        {
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                delete[] m_regFvImage[i];
            }
            delete[] m_regFvImage;
            m_regFvImage = NULL;
        }

        if (NULL == m_lastRegFVTmps)
        {
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                delete[] m_lastRegFVTmps[i];
            }
            delete[] m_lastRegFVTmps;
            m_lastRegFVTmps = NULL;
        }

        if (NULL == m_lastRegFPTmps)
        {
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                delete[] m_lastRegFPTmps[i];
            }
            delete[] m_lastRegFPTmps;
            m_lastRegFPTmps = NULL;
        }
    }
    return S_OK;
}


DWORD WINAPI FVDevice::ThreadCapture(LPVOID lParam)
{
    qDebug()<<"ThreadCapture";
    FVDevice* pDlg = (FVDevice*)lParam;
    if (NULL != pDlg)
    {
        pDlg->m_bStopThread = FALSE;
        unsigned char* pBufFPImage = new unsigned char[pDlg->m_imgFPWidth*pDlg->m_imgFPHeight];
        int nFPImageSize = 0;
        unsigned char* pBufFVImage = new unsigned char[pDlg->m_imgFVWidth*pDlg->m_imgFVHeight];
        int nFVImageSize = 0;
        unsigned char szFPTmp[MAX_TEMPLATE_SIZE] = { 0x0 };
        int nFPTmpSize = 0;
        unsigned char szFVTmp[MAX_TEMPLATE_SIZE] = { 0x0 };
        int nFVTmpSize = 0;

        while (!pDlg->m_bStopThread)
        {
            nFPImageSize = pDlg->m_imgFPWidth*pDlg->m_imgFPHeight;
            nFVImageSize = pDlg->m_imgFVWidth*pDlg->m_imgFVHeight;
            nFPTmpSize = MAX_TEMPLATE_SIZE;
            nFVTmpSize = MAX_TEMPLATE_SIZE;
            memset(szFPTmp, 0x0, MAX_TEMPLATE_SIZE);
            memset(szFVTmp, 0x0, MAX_TEMPLATE_SIZE);
            int ret = ZKFingerVein_CaptureFingerVeinImageAndTemplate(pDlg->m_hDevice,
                                                                     pBufFPImage, &nFPImageSize, pBufFVImage, &nFVImageSize,
                                                                     szFPTmp, &nFPTmpSize, szFVTmp, &nFVTmpSize);
            if (ZKFV_ERR_OK == ret)
            {

                memcpy(newimg,pBufFVImage,240*320);
                emit pDlg->pressed(newimg);

                if (nFPImageSize > 0)
                {
                    qDebug() << (WPARAM)pBufFPImage << (LPARAM)nFPImageSize << endl;
                }
                if (nFVImageSize > 0)
                {
                    qDebug() << (WPARAM)pBufFVImage << (LPARAM)nFVImageSize << endl;
                }
                if (pDlg->m_bRegister)
                {
                    pDlg->DoRegister(szFPTmp, nFPTmpSize, szFVTmp, nFVTmpSize, pBufFPImage, nFPImageSize, pBufFVImage, nFVImageSize);
                }
                else
                {
                    pDlg->DoVerify(szFPTmp, nFPTmpSize, szFVTmp, nFVTmpSize);
                }
            }
            Sleep(100);
        }
    }
    return 0;
}

void FVDevice::on_logged(char * s)
{

    database.addLog(atoi(s));
}



LRESULT FVDevice::m_Enroll(qint32 USERID)
{
    newUserId=USERID;
    if (NULL != m_hDevice)
    {
        m_strFingerID = QString::number(USERID);
        if (!m_bRegister)
        {
            m_bRegister = TRUE;
            m_nEnrollIndex = 0;
            qDebug() << "Doing register, please press your finger 3 times!" << endl;
        }
    }
    else
    {
        qDebug()<<"NULL = m_hDevice";
    }
    return S_OK;
}


void FVDevice::DoRegister(unsigned char* fpTmp, int cbFPTmp, unsigned char* fvTmp, int cbFVTmp, unsigned char * fpImg, int fpImgSize, unsigned char * fvImg, int fvImgSize)
{
    if (cbFPTmp > 0 && cbFVTmp > 0)	//fingerprint and fingervein capture success
    {
        if (m_nEnrollIndex > 0)
        {
            if (ZKFingerVein_Verify(m_hDevice, BIO_TYPE_FP, fpTmp, cbFPTmp, m_preRegFPTmps[m_nEnrollIndex - 1], m_nPreRegFPTmpSize[m_nEnrollIndex - 1]) <= 0 ||
                    ZKFingerVein_Verify(m_hDevice, BIO_TYPE_FV, fvTmp, cbFVTmp, m_preRegFVTmps[m_nEnrollIndex - 1], m_nPreRegFVTmpSize[m_nEnrollIndex - 1]) <= 0)
            {
                qDebug() << "Enroll failed!" << endl;
                m_bRegister = FALSE;
                m_nEnrollIndex = 0;
            }
        }
        memcpy(m_regFpImage[m_nEnrollIndex], fpImg, fpImgSize);
        m_nregFpImageSize[m_nEnrollIndex] = fpImgSize;
        memcpy(m_regFvImage[m_nEnrollIndex], fvImg, fvImgSize);
        m_nregFvImageSize[m_nEnrollIndex] = fvImgSize;
        memcpy(m_preRegFPTmps[m_nEnrollIndex], fpTmp, cbFPTmp);
        m_nPreRegFPTmpSize[m_nEnrollIndex] = cbFPTmp;
        memcpy(m_preRegFVTmps[m_nEnrollIndex], fvTmp, cbFVTmp);
        m_nPreRegFVTmpSize[m_nEnrollIndex] = cbFVTmp;
        m_nEnrollIndex++;
        if (m_nEnrollIndex >= 3)
        {

            const char * userName = m_strFingerID.toLatin1().data();
            char *buf = new char[strlen(userName) + 1];
            strcpy(buf, userName);

            int ret = 0;

            if (ZKFV_ERR_OK == (ret = ZKFingerVein_DBAdd(m_hDBCache, BIO_TYPE_FP, buf, m_preRegFPTmps, ENROLL_CNT)) &&
                    ZKFV_ERR_OK == (ret = ZKFingerVein_DBAdd(m_hDBCache, BIO_TYPE_FV, buf, m_preRegFVTmps, ENROLL_CNT)))
            {
                qDebug() << "Enroll succ!" << endl;
                emit regOver();
                //LJY//

                saveToDataBase(newUserId,m_preRegFPTmps,m_preRegFVTmps);
                //save Last register featue, to verify sample
                for (int i = 0; i < ENROLL_CNT; i++)
                {
                    m_nlastRegFPTmpSize[i] = m_nPreRegFPTmpSize[i];
                    m_nlastRegFVTmpSize[i] = m_nPreRegFVTmpSize[i];
                    memcpy(m_lastRegFPTmps[i], m_preRegFPTmps[i], m_nPreRegFPTmpSize[i]);
                    memcpy(m_lastRegFVTmps[i], m_preRegFVTmps[i], m_nPreRegFVTmpSize[i]);
                }
            }
            else
            {
                qDebug() << "ZKFingerVein_DBAdd failed, ret=	" << ret << endl;
            }
            m_bRegister = FALSE;
            m_nEnrollIndex = 0;
        }
        else
        {
            qDebug() << "You need press your finger	" << ENROLL_CNT - m_nEnrollIndex << "	times" << endl;
            qDebug()<<"restTimes"<<ENROLL_CNT - m_nEnrollIndex;
            emit restTimes(ENROLL_CNT - m_nEnrollIndex);
        }
    }

}

LRESULT FVDevice::m_Verify()
{
    if (NULL != m_hDevice)
    {
        if (m_bRegister)
        {
            m_bRegister = FALSE;
            qDebug() << "Start verify last register template" << endl;
        }
        m_bIdentify = FALSE;
    }
    return S_OK;
}

LRESULT FVDevice::m_Identify()
{
    if (NULL != m_hDevice)
    {
        if (m_bRegister)
        {
            m_bRegister = FALSE;
            qDebug() << "Please input your finger" << endl;
        }
        m_bIdentify = TRUE;
    }
    return S_OK;
}

LRESULT FVDevice::m_Clear()
{
    if (NULL != m_hDevice)
    {
        ZKFingerVein_DBClear(m_hDBCache, BIO_TYPE_FP);
        ZKFingerVein_DBClear(m_hDBCache, BIO_TYPE_FV);
        qDebug() << "Clear Finished" << endl;
    }
    else
    {
        qDebug() << "Please connect device first" << endl;
    }
    return S_OK;
}

int FVDevice::saveToDataBase(qint32 USERID, unsigned char **m_preRegFPTmps, unsigned char **m_preRegFVTmps)
{
    database.addData(USERID,m_preRegFPTmps,m_preRegFVTmps);
    return 0;
}



void HexDump(char *buf,int len,int addr) {
    int i,j,k;
    char binstr[80];

    for (i=0;i<len;i++) {
        if (0==(i%16)) {
            sprintf(binstr,"%08x -",i+addr);
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        } else if (15==(i%16)) {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
            sprintf(binstr,"%s  ",binstr);
            for (j=i-15;j<=i;j++) {
                sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
            }
            printf("%s\n",binstr);
        } else {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        }
    }
    if (0!=(i%16)) {
        k=16-(i%16);
        for (j=0;j<k;j++) {
            sprintf(binstr,"%s   ",binstr);
        }
        sprintf(binstr,"%s  ",binstr);
        k=16-k;
        for (j=i-k;j<i;j++) {
            sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
        }
        printf("%s\n",binstr);
    }
}








int FVDevice::loadFromDataBase()
{
    QVector<QVariantMap> RegedData=database.getAllData();
    int ret=0;
    while(RegedData.length())
    {
        QVariantMap oneData=RegedData.front();
        RegedData.pop_front();
        int idx=oneData.value("id").toInt();
        char * id=new char[32];
        itoa(idx,id,10);

        memcpy(m_preRegFPTmps[0],oneData.value("FP1").toByteArray().data(),2048);
        memcpy(m_preRegFPTmps[1],oneData.value("FP2").toByteArray().data(),2048);
        memcpy(m_preRegFPTmps[2],oneData.value("FP3").toByteArray().data(),2048);
        memcpy(m_preRegFVTmps[0],oneData.value("FV1").toByteArray().data(),2048);
        memcpy(m_preRegFVTmps[1],oneData.value("FV2").toByteArray().data(),2048);
        memcpy(m_preRegFVTmps[2],oneData.value("FV3").toByteArray().data(),2048);

        unsigned char fpTmp[MAX_TEMPLATE_SIZE] = { 0x0 };
        int cbFPTmp = MAX_TEMPLATE_SIZE;
        unsigned char fvTmp[MAX_TEMPLATE_SIZE] = { 0x0 };
        int cbFVTmp = MAX_TEMPLATE_SIZE;

        m_nEnrollIndex=0;
        while (1)
        {
            memcpy(fpTmp, m_preRegFPTmps[m_nEnrollIndex], 2048);
            m_nPreRegFPTmpSize[m_nEnrollIndex] = 2048;

            memcpy(fvTmp, m_preRegFVTmps[m_nEnrollIndex], 2048);
            m_nPreRegFVTmpSize[m_nEnrollIndex] = 2048;

            m_nEnrollIndex++;

            if (m_nEnrollIndex > 0)
            {
                int ret = 0;


                ZKFingerVein_Verify(m_hDevice, BIO_TYPE_FP, fpTmp, cbFPTmp, m_preRegFPTmps[m_nEnrollIndex - 1], m_nPreRegFPTmpSize[m_nEnrollIndex - 1]);

                ZKFingerVein_Verify(m_hDevice, BIO_TYPE_FV, fvTmp, cbFVTmp, m_preRegFVTmps[m_nEnrollIndex - 1], m_nPreRegFVTmpSize[m_nEnrollIndex - 1]);

            }
            if (m_nEnrollIndex >= 3)
            {
                int ret = 0;

                ret = ZKFingerVein_DBAdd(m_hDBCache, BIO_TYPE_FP, id, m_preRegFPTmps, ENROLL_CNT);

                ret = ZKFingerVein_DBAdd(m_hDBCache, BIO_TYPE_FV, id, m_preRegFVTmps, ENROLL_CNT);
                m_bRegister = FALSE;
                m_nEnrollIndex = 0;
                break;
            }
        }

    }

    return 0;
}


int FVDevice::GetIdentifyMode()
{
    int nSel = 1;
    if (-1 == nSel)
    {
        nSel = 0;
    }
    return nSel;
}


void FVDevice::DoVerify(unsigned char* fpTmp, int cbFPTmp, unsigned char* fvTmp, int cbFVTmp)
{
    qDebug()<<"DoVerify";
    if (m_bIdentify)
    {
        int identifyMode = GetIdentifyMode();

        char szFingerID[128] = { 0x0 };
        char * id=new char [128];
        QString strLog;

        int score = 0;
        int ret = 0;
        if (0 == identifyMode)
        {
            if (cbFPTmp > 0 && cbFVTmp > 0)
            {
                if (ZKFV_ERR_OK == (ret = ZKFingerVein_DBHybridIdentify(m_hDBCache, IDENTIFY_MODE_ANY, fpTmp, cbFPTmp, fvTmp, cbFVTmp, szFingerID, &score)))
                {
                    qDebug() << "Normal-Hybrid identify succ, tid= " << szFingerID << "	score=" << score;
                    memcpy(id,szFingerID,128);
                    emit logged(id);
                    qDebug() <<"emit logged(&szFingerID);";
                }
                else
                {
                    qDebug() << "Normal-Hybrid identify fail, ret= " << ret << endl;
                }
            }
            else
            {
                qDebug() << "Hybrid-Identify need capture fingerprint and fingervein succ!" << endl;
            }
        }
        else if (1 == identifyMode)
        {
            if (cbFPTmp > 0 && cbFVTmp > 0)
            {
                if (ZKFV_ERR_OK == (ret = ZKFingerVein_DBHybridIdentify(m_hDBCache, IDENTIFY_MODE_FAKE, fpTmp, cbFPTmp, fvTmp, cbFVTmp, szFingerID, &score)))
                {
                    qDebug() << "Fake-Hybrid identify succ, tid=" << szFingerID << "  score=" << score << endl;
                    memcpy(id,szFingerID,128);
                    emit logged(id);
                    qDebug() <<"emit logged(&szFingerID);";
                }
                else
                {
                    strLog+=QString("Fake-Hybrid identify fail, ret=" )+QString::number(ret);
                    qDebug() << strLog << endl;
                }
            }
            else
            {
                qDebug() << QString("Hybrid-Identify need capture fingerprint and fingervein succ!" )<< endl;
            }
        }
        else if (2 == identifyMode)
        {
            if (cbFPTmp > 0 && cbFVTmp > 0)
            {
                if (ZKFV_ERR_OK == (ret = ZKFingerVein_DBHybridIdentify(m_hDBCache, IDENTIFY_MODE_BOTH, fpTmp, cbFPTmp, fvTmp, cbFVTmp, szFingerID, &score)))
                {
                    strLog +="Security-Hybrid identify succ, tid="+QString(szFingerID)+" , score ="+QString::number(score);
                    qDebug() << strLog << endl;
                }
                else
                {
                    strLog+="Security-Hybrid identify fail, ret="+ QString::number(ret);
                    qDebug() << strLog << endl;
                }
            }
            else
            {
                qDebug() << "Hybrid-Identify need capture fingerprint and fingervein succ!" << endl;
            }
        }
        else
        {
            qDebug()<<"identifyMode = 3;";
            if (cbFPTmp > 0 && ZKFV_ERR_OK == (ret = ZKFingerVein_DBIdentify(m_hDBCache, BIO_TYPE_FP, fpTmp, cbFPTmp, szFingerID, &score)))
            {
                strLog += "Identify fingerprint success, tid = "+QString(szFingerID) +", score=" + QString::number(score)+"\r\n";
            }
            if (cbFVTmp > 0 && ZKFV_ERR_OK == (ret = ZKFingerVein_DBIdentify(m_hDBCache, BIO_TYPE_FV, fvTmp, cbFVTmp, szFingerID, &score)))
            {
                strLog += "Identify finger-vein success, tid = "+QString(szFingerID) +", score=" + QString::number(score)+"\r\n";
            }
            if (strLog.length())
            {
                qDebug() << "Identify failed" << endl;
            }
            else
            {

                qDebug() << strLog <<"XXXXXXXXXXXX"<< endl;
            }
        }
    }
    else
    {
        if (m_nlastRegFVTmpSize[0] <= 0)
        {
            qDebug() << "no fingervein registered" << endl;
            return;
        }
        QString strLog;
        if (cbFPTmp > 0)
        {
            strLog+="Verify fingerprint failed!\r\n";
            int score = 0;
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                if ((score = ZKFingerVein_Verify(m_hDevice, BIO_TYPE_FP, m_lastRegFPTmps[i], m_nlastRegFPTmpSize[i], fpTmp, cbFPTmp)) > 0)
                {
                    strLog+="Verify fingerprint succ, score = "+QString::number(score)+"\r\n";
                    break;
                }
            }
        }
        if (cbFVTmp > 0)
        {
            strLog += "Verify fingervein failed!\r\n";
            int score = 0;
            for (int i = 0; i < ENROLL_CNT; i++)
            {
                if ((score = ZKFingerVein_Verify(m_hDevice, BIO_TYPE_FV, m_lastRegFVTmps[i], m_nlastRegFVTmpSize[i], fvTmp, cbFVTmp)) > 0)
                {
                    strLog += "Verify fingervein succ, score = " + QString::number(score)+"\r\n";
                    break;
                }
            }
        }
        qDebug() << strLog << endl;
    }
}


LRESULT FVDevice::m_Init()
{
    qDebug()<<"m_Init";

    database.init();

    connect(this,SIGNAL(logged(char*)),this,SLOT(on_logged(char*)));

    m_bIdentify = TRUE;
    m_bRegister = FALSE;
    m_hDBCache = NULL;
    m_hDevice = NULL;
    m_bStopThread = FALSE;
    m_hThreadWork = NULL;

    m_preRegFPTmps = NULL;
    m_preRegFVTmps = NULL;
    m_lastRegFPTmps = NULL;
    m_lastRegFVTmps = NULL;
    m_regFpImage = NULL;
    m_regFvImage = NULL;

    return TRUE;
}


