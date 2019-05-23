#include "stdafx.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////

int Str2Int(LPCTSTR value)
{
    return atoi(value);
}

CString Int2Str(int value)
{
    CString str;
    str.Format("%d", value);
    return str;
}

bool LoadConfig(SConfig& config, LPCTSTR file)
{
#define BUF_SIZE 1024
    TCHAR szBuf[BUF_SIZE + 1] = { 0 };

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_HOST, "", szBuf, BUF_SIZE, file);
    config.strHost = szBuf;

    config.nPort = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_PORT, 1883, file);

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_CLIENT_ID, "", szBuf, BUF_SIZE, file);
    config.strClientID = szBuf;

    config.bCleanSession = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_CLEAN_SESSION, 0, file);

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_USER_NAME, "", szBuf, BUF_SIZE, file);
    config.strUserName = szBuf;

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_PASSWORD, "", szBuf, BUF_SIZE, file);
    config.strPasswrod = szBuf;

    config.nKeepAlive = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_KEEP_ALIVE, 60, file);

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_LAST_WILL_TOPIC, "", szBuf, BUF_SIZE, file);
    config.strLastWillTopic = szBuf;

    config.nLastWillQos = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_LAST_WILL_QOS, 0, file);

    config.bLastWillRetain = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_LAST_WILL_RETAIN, 1, file);

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_LAST_WILL_PAYLOAD, "", szBuf, BUF_SIZE, file);
    config.strLastWillPayload = szBuf;

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_SUBSCRIBE_TOPIC, "", szBuf, BUF_SIZE, file);
    config.strSubscribeTopic = szBuf;

    config.nSubscribeQos = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_SUBSCRIBE_QOS, 0, file);


    GetPrivateProfileString(CFG_MODULE, CFG_KEY_PUBLISH_TOPIC, "", szBuf, BUF_SIZE, file);
    config.strPublishTopic = szBuf;

    config.nPublishQos = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_PUBLISH_QOS, 0, file);

    config.bPublishRetain = GetPrivateProfileInt(CFG_MODULE, CFG_KEY_PUBLISH_RETAIN, 1, file);

    GetPrivateProfileString(CFG_MODULE, CFG_KEY_PUBLISH_PAYLOAD, "", szBuf, BUF_SIZE, file);
    config.strPublishPayload = szBuf;

    return true;
}

bool SaveConfig(const SConfig& config, LPCTSTR file)
{
    BOOL ret = TRUE;
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_HOST, config.strHost, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_PORT, Int2Str(config.nPort), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_CLIENT_ID, config.strClientID, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_CLEAN_SESSION, Int2Str(config.bCleanSession), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_USER_NAME, config.strUserName, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_PASSWORD, config.strPasswrod, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_KEEP_ALIVE, Int2Str(config.nKeepAlive), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_LAST_WILL_TOPIC, config.strLastWillTopic, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_LAST_WILL_QOS, Int2Str(config.nLastWillQos), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_LAST_WILL_RETAIN, Int2Str(config.bLastWillRetain), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_LAST_WILL_PAYLOAD, config.strLastWillPayload, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_SUBSCRIBE_TOPIC, config.strSubscribeTopic, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_SUBSCRIBE_QOS, Int2Str(config.nSubscribeQos), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_PUBLISH_TOPIC, config.strPublishTopic, file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_PUBLISH_QOS, Int2Str(config.nPublishQos), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_PUBLISH_RETAIN, Int2Str(config.bPublishRetain), file);
    ret &= WritePrivateProfileString(CFG_MODULE, CFG_KEY_PUBLISH_PAYLOAD, config.strPublishPayload, file);
    return ret;
}
