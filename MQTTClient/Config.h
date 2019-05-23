#pragma once

struct SConfig
{
    CString                                 strHost;
    int                                     nPort;
    CString                                 strClientID;
    bool                                    bCleanSession;
    CString                                 strUserName;
    CString                                 strPasswrod;
    int                                     nKeepAlive;
    CString                                 strLastWillTopic;
    int                                     nLastWillQos;
    bool                                    bLastWillRetain;
    CString                                 strLastWillPayload;
    CString                                 strSubscribeTopic;
    int                                     nSubscribeQos;
    CString                                 strPublishTopic;
    int                                     nPublishQos;
    bool                                    bPublishRetain;
    CString                                 strPublishPayload;
};

#define CFG_MODULE                          "Config"

#define     CFG_KEY_HOST                    "Host"
#define     CFG_KEY_PORT                    "Port"
#define     CFG_KEY_CLIENT_ID               "ClientID"
#define     CFG_KEY_CLEAN_SESSION           "CleanSession"

#define     CFG_KEY_USER_NAME               "UserName"
#define     CFG_KEY_PASSWORD                "Password"
#define     CFG_KEY_KEEP_ALIVE              "KeepAlive"

#define     CFG_KEY_LAST_WILL_TOPIC         "LastWillTopic"
#define     CFG_KEY_LAST_WILL_QOS           "LastWillQos"
#define     CFG_KEY_LAST_WILL_RETAIN        "LastWillRetain"
#define     CFG_KEY_LAST_WILL_PAYLOAD       "LastWillPayload"

#define     CFG_KEY_SUBSCRIBE_TOPIC         "SubscribeTopic"
#define     CFG_KEY_SUBSCRIBE_QOS           "SubscribeQos"

#define     CFG_KEY_PUBLISH_TOPIC           "PublishTopic"
#define     CFG_KEY_PUBLISH_QOS             "PublishQos"
#define     CFG_KEY_PUBLISH_RETAIN          "PublishRetain"
#define     CFG_KEY_PUBLISH_PAYLOAD         "PublishPayload"

extern int      Str2Int(LPCTSTR value);
extern CString  Int2Str(int value);
extern bool     LoadConfig(SConfig& config, LPCTSTR file);
extern bool     SaveConfig(const SConfig& config, LPCTSTR file);