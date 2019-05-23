
// MQTTClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MQTTClient.h"
#include "MQTTClientDlg.h"
#include "afxdialogex.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
    public:
        CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
        enum { IDD = IDD_ABOUTBOX };
#endif

    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
        virtual BOOL OnInitDialog();

// 实现
    protected:
        DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetDlgItemText(IDC_EDIT,
                   "MQTTClient, V1.0\r\n"
                   "Copyright(C) 2019\r\n"
                   "Author:\tlauthrul\r\n"
                   "Email:\tlauthrul@foxmail.com\r\n");

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常:  OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

#define COLOR_WHITE     RGB(255,255,255)
#define COLOR_BLACK     RGB(0,0,0)
#define COLOR_RED       RGB(139,0,0)
#define COLOR_GREEN     RGB(0,139,0)
#define COLOR_BLUE      RGB(0,0,139)
#define COLOR_CYAN      RGB(0,139,139)

enum
{
    MSG_MQTT_CONNECTED = WM_USER + 1000,
    MSG_MQTT_CONNECTION_LOST,
    MSG_MQTT_MESSAGE_ARRIVED,
    MSG_MQTT_DELIVERY_COMPLETE,
    MSG_MQTT_ACTION_FAILURE,
    MSG_MQTT_ACTION_SUCCESS,
};

static struct SCache
{
    CPoint ptContentMenu;
    int nUnsubscribeIndex;

    SCache()
    {
        Reset();
    }
    void Reset()
    {
        ptContentMenu = { 0, 0 };
        nUnsubscribeIndex = -1;
    }
} sCache;

CString Now()
{
    CString str;
    CTime tm;
    tm = CTime::GetCurrentTime();
    str = tm.Format("%Y-%m-%d %X");
    return str;
}

CString ConvertUTF8ToANSI(LPCTSTR utf8_str)
{
    int len = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, -1, NULL, 0);
    WCHAR* wszbuf = new WCHAR[len + 1];
    memset(wszbuf, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, wszbuf, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszbuf, -1, NULL, 0, NULL, NULL);
    CHAR* szbuf = new CHAR[len + 1];
    memset(szbuf, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszbuf, -1, szbuf, len, NULL, NULL);

    CString ret(szbuf);
    delete[] wszbuf;
    delete[] szbuf;
    return ret;
}

CString ConvertANSIToUTF8(LPCTSTR ansi_str)
{
    int len = ::MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, -1, NULL, 0);
    WCHAR* wszbuf = new WCHAR[len + 1];
    memset(wszbuf, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, ansi_str, -1, wszbuf, len);

    len = WideCharToMultiByte(CP_UTF8, 0, wszbuf, -1, NULL, 0, NULL, NULL);
    CHAR* szbuf = new CHAR[len + 1];
    memset(szbuf, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wszbuf, -1, szbuf, len, NULL, NULL);

    CString ret(szbuf);
    delete[] wszbuf;
    delete[] szbuf;
    return ret;
}


//////////////////////////////////////////////////////////////////////////
CMQTTCallback::CMQTTCallback() : m_pOwner(NULL)
{

}

CMQTTCallback::CMQTTCallback(CMQTTClientDlg* pOwner) : m_pOwner(pOwner)
{

}

CMQTTCallback::~CMQTTCallback()
{

}

void CMQTTCallback::SetOwner(CMQTTClientDlg* pOwner)
{
    m_pOwner = pOwner;
}

void CMQTTCallback::connected(const mqtt::string& cause)
{
    // TODO: on_success() will be triggered before this callback after connect successful. so just ignore this, and handle on on_success()
}

void CMQTTCallback::connection_lost(const mqtt::string& cause)
{
    if (m_pOwner != nullptr)
    {
        CString strText;
        strText.AppendFormat("\n%s [System]: connection lost! cause: %s", Now().GetString(), cause.c_str());
        m_pOwner->SetMessage(strText, COLOR_RED);
        m_pOwner->SetConnectionState(FALSE);
    }
}

void CMQTTCallback::message_arrived(mqtt::const_message_ptr msg)
{
    if (m_pOwner != nullptr)
    {
        CString strText;
        strText.AppendFormat("\n%s [Received]: ", Now().GetString());
        strText.AppendFormat("\n\t Topic: %s", msg->get_topic_ref().c_str());
        strText.AppendFormat("\n\t Payload: %s", msg->get_payload_ref().c_str());
        m_pOwner->SetMessage(ConvertUTF8ToANSI(strText), COLOR_BLUE);
    }
}

void CMQTTCallback::delivery_complete(mqtt::delivery_token_ptr token)
{

}

void CMQTTCallback::on_failure(const mqtt::token& tok)
{
    if (m_pOwner != nullptr)
    {
        CString strText;
        CString strAction = (const char*)tok.get_user_context();
        strText.AppendFormat("\n%s [System]: %s fail! code: %d", Now().GetString(), strAction.GetString(), tok.get_return_code());
        m_pOwner->SetMessage(strText, COLOR_RED);
        if (strAction == "connect")
        {
            m_pOwner->SetConnectionState(FALSE);
        }
        else if (strAction == "disconnect")
        {
            m_pOwner->SetConnectionState(TRUE);
        }
    }
}

void CMQTTCallback::on_success(const mqtt::token& tok)
{
    if (m_pOwner != nullptr)
    {
        CString strText;
        CString strAction = (const char*)tok.get_user_context();
        strText.AppendFormat("\n%s [System]: %s success!", Now().GetString(), strAction.GetString());
        m_pOwner->SetMessage(strText, COLOR_GREEN);
        if (strAction == "connect")
        {
            m_pOwner->SetConnectionState(TRUE);
            m_pOwner->m_lstSubscriptions.ResetContent();
        }
        else if (strAction == "disconnect")
        {
            m_pOwner->SetConnectionState(FALSE);
        }
        else if (strAction == "subscribe")
        {
            auto top = tok.get_topics();
            if (top && !top->empty())
                m_pOwner->m_lstSubscriptions.AddString((*top)[0].c_str());
        }
        else if (strAction == "unsubscribe")
        {
            m_pOwner->m_lstSubscriptions.DeleteString(sCache.nUnsubscribeIndex);
            sCache.Reset();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// CMQTTClientDlg 对话框



CMQTTClientDlg::CMQTTClientDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_MQTTCLIENT_DIALOG, pParent)
    , m_pMqtt(nullptr)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_cbMqtt.SetOwner(this);
}

void CMQTTClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_HOST, m_edtHost);
    DDX_Control(pDX, IDC_EDIT_PORT, m_edtPort);
    DDX_Control(pDX, IDC_EDIT_CLIENTID, m_edtClientID);
    DDX_Control(pDX, IDC_CHECK_CLEANSESSION, m_chkCleanSession);
    DDX_Control(pDX, IDC_EDIT_USERNAME, m_edtUserName);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_edtPasswrod);
    DDX_Control(pDX, IDC_EDIT_KEEPALIVE, m_edtKeepAlive);
    DDX_Control(pDX, IDC_EDIT_LASTWILLTOPIC, m_edtLastWillTopic);
    DDX_Control(pDX, IDC_COMBO_LASTWILLQOS, m_cmbLastWillQos);
    DDX_Control(pDX, IDC_CHECK_LASTWILLRETAIN, m_chkLastWillRetain);
    DDX_Control(pDX, IDC_EDIT_LASTWILLPAYLOAD, m_edtLastWillPayload);
    DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
    DDX_Control(pDX, IDC_BUTTON_DISCONNECT, m_btnDisConnect);
    DDX_Control(pDX, IDC_EDIT_SUBCRIBETOPIC, m_edtSubscribeTopic);
    DDX_Control(pDX, IDC_COMBO_SUBSCRIBEQOS, m_cmbSubscribeQos);
    DDX_Control(pDX, IDC_BUTTON_SUBSCRIBE, m_btnSubscribe);
    DDX_Control(pDX, IDC_LIST_SUBSCRIPTIONS, m_lstSubscriptions);
    DDX_Control(pDX, IDC_EDIT_MESSAGES, m_edtMessages);
    DDX_Control(pDX, IDC_EDIT_PUBLISHTOPIC, m_edtPublishTopic);
    DDX_Control(pDX, IDC_COMBO_PUBLISHQOS, m_cmbPublishQos);
    DDX_Control(pDX, IDC_CHECK_PUBLISHRETAIN, m_chkPublishRetain);
    DDX_Control(pDX, IDC_EDIT_PUBLISHPAYLOAD, m_edtPublishPayload);
    DDX_Control(pDX, IDC_BUTTON_PUBLISH, m_btnPublish);
}

BEGIN_MESSAGE_MAP(CMQTTClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMQTTClientDlg::OnBnClickedButtonConnect)
    ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CMQTTClientDlg::OnBnClickedButtonDisconnect)
    ON_BN_CLICKED(IDC_BUTTON_SUBSCRIBE, &CMQTTClientDlg::OnBnClickedButtonSubscribe)
    ON_BN_CLICKED(IDC_BUTTON_PUBLISH, &CMQTTClientDlg::OnBnClickedButtonPublish)
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_UNSUBSCRIBE_UNSUBSCRIBE, &CMQTTClientDlg::OnContextmenuUnsubscribe)
    ON_COMMAND(ID_CONTENTMENU_CLEANCONTENT, &CMQTTClientDlg::OnContextmenuCleancontent)
    ON_WM_DROPFILES()
    ON_COMMAND(ID_MAINMENU_OPEN, &CMQTTClientDlg::OnMainmenuOpen)
    ON_COMMAND(ID_MAINMENU_SAVE, &CMQTTClientDlg::OnMainmenuSave)
    ON_COMMAND(ID_MAINMENU_SAVEAS, &CMQTTClientDlg::OnMainmenuSaveas)
    ON_COMMAND(ID_MAINMENU_ABOUT, &CMQTTClientDlg::OnMainmenuAbout)
END_MESSAGE_MAP()


// CMQTTClientDlg 消息处理程序

BOOL CMQTTClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);         // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    // TODO: 在此添加额外的初始化代码
    m_edtHost.SetWindowText(_T("broker.hivemq.com"));
    m_edtPort.SetWindowText(_T("1883"));
    m_edtClientID.SetWindowText(_T("MQTTClient"));
    m_edtKeepAlive.SetWindowText(_T("60"));
    m_edtSubscribeTopic.SetWindowText(_T("testtopic/#"));
    m_edtPublishTopic.SetWindowText(_T("testtopic/"));
    m_chkCleanSession.SetCheck(TRUE);
    m_cmbLastWillQos.SetCurSel(0);
    m_cmbSubscribeQos.SetCurSel(0);
    m_cmbPublishQos.SetCurSel(0);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CMQTTClientDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        bool bCtrl = GetKeyState(VK_CONTROL) & 0x8000;
        bool bShift = GetKeyState(VK_SHIFT) & 0x8000;

        switch (pMsg->wParam)
        {
            case VK_ESCAPE:
                return TRUE;
            case 'S':
                {
                    if (bCtrl && !bShift) // Ctrl + S
                    {
                        SaveConfigFile(false);
                        return TRUE;
                    }
                    else if (bCtrl && bShift) // Ctrl + Shift + S
                    {
                        SaveConfigFile(true);
                        return TRUE;
                    }
                }
            case 'O':
                {
                    if (bCtrl && !bShift) // Ctrl + O
                    {
                        OnMainmenuOpen();
                        return TRUE;
                    }
                }
                break;
            default:
                break;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CMQTTClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMQTTClientDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMQTTClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CMQTTClientDlg::SetConnectionState(BOOL connection)
{
    m_btnConnect.EnableWindow(!connection);
    m_btnDisConnect.EnableWindow(connection);
    m_btnSubscribe.EnableWindow(connection);
    m_btnPublish.EnableWindow(connection);
}

void CMQTTClientDlg::SetMessage(CString msg, DWORD color)
{
    m_edtMessages.SetSel(-1, -1);
    long lcnt_before = m_edtMessages.GetLineCount();
    m_edtMessages.ReplaceSel(msg);
    long lcnt_after = m_edtMessages.GetLineCount();

    CHARFORMAT2 cFmt;
    ZeroMemory(&cFmt, sizeof(CHARFORMAT2));
    cFmt.cbSize = sizeof(CHARFORMAT2);
    cFmt.crTextColor = color;
    cFmt.dwEffects = 0;
    cFmt.dwMask = CFM_COLOR;
    long start = m_edtMessages.LineIndex(lcnt_before);
    long end = m_edtMessages.LineIndex(lcnt_after) - 1;
    m_edtMessages.SetSel(start, end);
    m_edtMessages.SetSelectionCharFormat(cFmt);
    m_edtMessages.Clear();

    m_edtMessages.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

bool CMQTTClientDlg::GetConfig(SConfig& config)
{
    CString strText;

    m_edtHost.GetWindowText(config.strHost);

    m_edtPort.GetWindowText(strText);
    config.nPort = Str2Int(strText);

    m_edtClientID.GetWindowText(config.strClientID);

    config.bCleanSession = m_chkCleanSession.GetCheck();

    m_edtUserName.GetWindowText(config.strUserName);

    m_edtPasswrod.GetWindowText(config.strPasswrod);

    m_edtKeepAlive.GetWindowText(strText);
    config.nKeepAlive = Str2Int(strText);

    m_edtLastWillTopic.GetWindowText(config.strLastWillTopic);

    config.nLastWillQos = m_cmbLastWillQos.GetCurSel();

    config.bLastWillRetain = m_chkLastWillRetain.GetCheck();

    m_edtLastWillPayload.GetWindowText(config.strLastWillPayload);

    m_edtSubscribeTopic.GetWindowText(config.strSubscribeTopic);

    config.nSubscribeQos = m_cmbSubscribeQos.GetCurSel();

    m_edtPublishTopic.GetWindowText(config.strPublishTopic);

    config.nPublishQos = m_cmbPublishQos.GetCurSel();

    config.bPublishRetain = m_chkPublishRetain.GetCheck();

    m_edtPublishPayload.GetWindowText(config.strPublishPayload);

    return true;
}

bool CMQTTClientDlg::SetConfig(const SConfig& config)
{
    m_edtHost.SetWindowText(config.strHost);
    m_edtPort.SetWindowText(Int2Str(config.nPort));
    m_edtClientID.SetWindowText(config.strClientID);
    m_chkCleanSession.SetCheck(config.bCleanSession);
    m_edtUserName.SetWindowText(config.strUserName);
    m_edtPasswrod.SetWindowText(config.strPasswrod);
    m_edtKeepAlive.SetWindowText(Int2Str(config.nKeepAlive));
    m_edtLastWillTopic.SetWindowText(config.strLastWillTopic);
    m_cmbLastWillQos.SetCurSel(config.nLastWillQos);
    m_chkLastWillRetain.SetCheck(config.bLastWillRetain);
    m_edtLastWillPayload.SetWindowText(config.strLastWillPayload);
    m_edtSubscribeTopic.SetWindowText(config.strSubscribeTopic);
    m_cmbSubscribeQos.SetCurSel(config.nSubscribeQos);
    m_edtPublishTopic.SetWindowText(config.strPublishTopic);
    m_cmbPublishQos.SetCurSel(config.nPublishQos);
    m_chkPublishRetain.SetCheck(config.bPublishRetain);
    m_edtPublishPayload.SetWindowText(config.strPublishPayload);
    return true;
}

void CMQTTClientDlg::SetTitle(LPCTSTR fileName)
{
    CString strTitle = CString("MQTTClient - ") + fileName;
    SetWindowText(strTitle);
}

bool CMQTTClientDlg::LoadConfigFile(LPCTSTR fileName)
{
    SConfig config;
    if (!::LoadConfig(config, fileName))
    {
        AfxMessageBox(_T("Load config file fail"));
        return false;
    }

    m_strCfgFile = fileName;
    SetTitle(m_strCfgFile);
    SetConfig(config);
    return true;
}

bool CMQTTClientDlg::SaveConfigFile(bool saveas)
{
    SConfig config;
    GetConfig(config);

    if (m_strCfgFile.IsEmpty() || saveas)
    {
        OPENFILENAME ofn = { 0 };
        char szFileName[MAX_PATH] = { 0 };

        ofn.lStructSize = sizeof(ofn);
        ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
        ofn.hInstance = GetModuleHandle(0);
        ofn.hwndOwner = m_hWnd;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFile = (LPSTR)szFileName;
        ofn.nFilterIndex = 1;
        ofn.lpstrFilter = "Config Files(*.ini)\0*.ini\0All Files(*.*)\0*.*\0";

        if (!GetSaveFileName(&ofn))
            return false;

        m_strCfgFile = ofn.lpstrFile;
        SetTitle(m_strCfgFile);
    }
    if (m_strCfgFile.IsEmpty())
    {
        AfxMessageBox(_T("Can't save to an empty target file"));
        return false;
    }

    if (::SaveConfig(config, m_strCfgFile))
        AfxMessageBox(_T("Save success"));
}

void CMQTTClientDlg::OnBnClickedButtonConnect()
{
    CString strHost, strPort, strClientID, strUser, strPassword, strKeepAlive, strLastWillTopic, strLastWillPayload;

    m_edtHost.GetWindowText(strHost);
    m_edtPort.GetWindowText(strPort);
    m_edtClientID.GetWindowText(strClientID);
    m_edtUserName.GetWindowText(strUser);
    m_edtPasswrod.GetWindowText(strPassword);
    m_edtKeepAlive.GetWindowText(strKeepAlive);
    m_edtLastWillTopic.GetWindowText(strLastWillTopic);
    m_edtLastWillPayload.GetWindowText(strLastWillPayload);

    strHost = "tcp://" + strHost + ":" + strPort;

    m_pMqtt = std::make_shared<mqtt::async_client>(strHost.GetString(), strClientID.GetString());
    m_pMqtt->set_callback(m_cbMqtt);

    mqtt::connect_options opt;
    opt.set_keep_alive_interval(atoi(strKeepAlive.GetString()));
    opt.set_clean_session(m_chkCleanSession.GetCheck());
    if (!strUser.IsEmpty()) opt.set_user_name(strUser.GetString());
    if (!strPassword.IsEmpty()) opt.set_password(strPassword.GetString());

    if (!strLastWillTopic.IsEmpty() && !strLastWillPayload.IsEmpty())
    {
        mqtt::message willmsg(strLastWillTopic.GetString(),
                              strLastWillPayload.GetString(),
                              m_cmbLastWillQos.GetCurSel(),
                              m_chkLastWillRetain.GetCheck());
        mqtt::will_options will(willmsg);
        opt.set_will(will);
    }

    m_pMqtt->connect(opt, "connect", m_cbMqtt);
    SetConnectionState(FALSE);
}

void CMQTTClientDlg::OnBnClickedButtonDisconnect()
{
    if (m_pMqtt->is_connected())
        m_pMqtt->disconnect("disconnect", m_cbMqtt);
}


void CMQTTClientDlg::OnBnClickedButtonSubscribe()
{
    CString strTopic;
    m_edtSubscribeTopic.GetWindowText(strTopic);
    if (!strTopic.IsEmpty())
        m_pMqtt->subscribe(strTopic.GetString(), m_cmbSubscribeQos.GetCurSel(), "subscribe", m_cbMqtt);
}


void CMQTTClientDlg::OnBnClickedButtonPublish()
{
    CString strTopic, strPayload;
    m_edtPublishTopic.GetWindowText(strTopic);
    m_edtPublishPayload.GetWindowText(strPayload);
    if (!strTopic.IsEmpty()/* && !strPayload.IsEmpty()*/)
    {
        mqtt::message_ptr msg = mqtt::make_message(strTopic.GetString(),
                                strPayload.GetString(),
                                m_cmbPublishQos.GetCurSel(),
                                m_chkPublishRetain.GetCheck());
        m_pMqtt->publish(msg, "publish", m_cbMqtt);

        CString strText;
        strText.AppendFormat("\n%s [Publish]: ", Now().GetString());
        strText.AppendFormat("\n\t Topic: %s", msg->get_topic_ref().c_str());
        strText.AppendFormat("\n\t Payload: %s", msg->get_payload_ref().c_str());
        SetMessage(strText, COLOR_CYAN);
    }
}


void CMQTTClientDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    CMenu popMenu;
    popMenu.LoadMenu(IDR_MENU);
    CMenu* pSubMenu = popMenu.GetSubMenu(0);

    BOOL bShow = FALSE;
    CRect rcSubscriptions, rcMessages;
    m_lstSubscriptions.GetWindowRect(rcSubscriptions);
    m_edtMessages.GetWindowRect(rcMessages);
    if (rcSubscriptions.PtInRect(point))
    {
        pSubMenu->RemoveMenu(1, MF_BYPOSITION);
        bShow = TRUE;
    }
    if (rcMessages.PtInRect(point))
    {
        pSubMenu->RemoveMenu(0, MF_BYPOSITION);
        bShow = TRUE;
    }

    if (bShow)
    {
        pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
        pSubMenu->Detach();
        popMenu.DestroyMenu();
    }

    sCache.ptContentMenu = point;
}


void CMQTTClientDlg::OnContextmenuUnsubscribe()
{
    CPoint pt(sCache.ptContentMenu);
    m_lstSubscriptions.ScreenToClient(&pt);

    BOOL bOutside;
    int nIndex = m_lstSubscriptions.ItemFromPoint(pt, bOutside);
    if (!bOutside && nIndex >= 0 && nIndex <= m_lstSubscriptions.GetCount())
    {
        CString strTopic;
        m_lstSubscriptions.GetText(nIndex, strTopic);
        CString strMsg;
        strMsg.Format("You are about to unsubscribe topic [%s], are you sure?", strTopic);
        if (AfxMessageBox(strMsg, MB_OKCANCEL) == IDOK)
        {
            sCache.nUnsubscribeIndex = nIndex;
            m_pMqtt->unsubscribe(strTopic.GetString(), "unsubscribe", m_cbMqtt);
        }
    }
}


void CMQTTClientDlg::OnContextmenuCleancontent()
{
    m_edtMessages.SetWindowText(_T(""));
}


void CMQTTClientDlg::OnDropFiles(HDROP hDropInfo)
{
    UINT count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    if (count <= 0) return;
    if (count > 1)
    {
        AfxMessageBox(_T("Please select only one config file"));
        return;
    }

    char filePath[MAX_PATH + 1] = { 0 };
    DragQueryFile(hDropInfo, 0, filePath, MAX_PATH);
    DragFinish(hDropInfo);

    CString ext = PathFindExtension(filePath);
    if (ext != _T(".ini"))
    {
        AfxMessageBox(_T("No supported file type"));
        return;
    }


    LoadConfigFile(filePath);

    CDialogEx::OnDropFiles(hDropInfo);
}


void CMQTTClientDlg::OnMainmenuOpen()
{
    TCHAR szFile[MAX_PATH] = { 0 };

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof (ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = _T("Config Files(*.ini)\0*.ini\0All Files(*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileName(&ofn))
        LoadConfigFile(ofn.lpstrFile);
}


void CMQTTClientDlg::OnMainmenuSave()
{
    SaveConfigFile(false);
}


void CMQTTClientDlg::OnMainmenuSaveas()
{
    SaveConfigFile(true);
}


void CMQTTClientDlg::OnMainmenuAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}
