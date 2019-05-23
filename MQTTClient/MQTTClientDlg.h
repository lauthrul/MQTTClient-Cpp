
// MQTTClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////
class CMQTTClientDlg;
class CMQTTCallback : public mqtt::callback, public mqtt::iaction_listener
{
public:
	CMQTTCallback();
	CMQTTCallback(CMQTTClientDlg* pOwner);
	virtual ~CMQTTCallback();

public:
	void SetOwner(CMQTTClientDlg* pOwner);

protected:
	// implement of mqtt::callback

	void connected(const mqtt::string& cause) override;

	void connection_lost(const mqtt::string& cause) override;

	void message_arrived(mqtt::const_message_ptr msg) override;

	void delivery_complete(mqtt::delivery_token_ptr token) override;

	// implement of mqtt::iaction_listener

	void on_failure(const mqtt::token& tok) override;

	void on_success(const mqtt::token& tok) override;

private:
	CMQTTClientDlg* m_pOwner;
};

//////////////////////////////////////////////////////////////////////////
// CMQTTClientDlg 对话框
class CMQTTClientDlg : public CDialogEx
{
// 构造
public:
	CMQTTClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MQTTCLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonSubscribe();
	afx_msg void OnBnClickedButtonPublish();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnContextmenuUnsubscribe();
	afx_msg void OnContextmenuCleancontent();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnMainmenuOpen();
    afx_msg void OnMainmenuSave();
    afx_msg void OnMainmenuSaveas();
    afx_msg void OnMainmenuAbout();
	DECLARE_MESSAGE_MAP()

public:
	void SetConnectionState(BOOL connection);
	void SetMessage(CString msg, DWORD color);
    bool GetConfig(SConfig& config);
    bool SetConfig(const SConfig& config);
    void SetTitle(LPCTSTR fileName);
    bool LoadConfigFile(LPCTSTR fileName);
    bool SaveConfigFile(bool saveas);

public:
	CEdit                  m_edtHost;
	CEdit                  m_edtPort;
	CEdit                  m_edtClientID;
	CButton                m_chkCleanSession;
	CEdit                  m_edtUserName;
	CEdit                  m_edtPasswrod;
	CEdit                  m_edtKeepAlive;
	CEdit                  m_edtLastWillTopic;
	CComboBox              m_cmbLastWillQos;
	CButton                m_chkLastWillRetain;
	CEdit                  m_edtLastWillPayload;
	CButton                m_btnConnect;
	CButton                m_btnDisConnect;
	CEdit                  m_edtSubscribeTopic;
	CButton                m_btnSubscribe;
	CComboBox              m_cmbSubscribeQos;
	CListBox               m_lstSubscriptions;
	CRichEditCtrl          m_edtMessages;
	CEdit                  m_edtPublishTopic;
	CComboBox              m_cmbPublishQos;
	CButton                m_chkPublishRetain;
	CEdit                  m_edtPublishPayload;
	CButton                m_btnPublish;

	mqtt::async_client_ptr m_pMqtt;
	CMQTTCallback          m_cbMqtt;

    CString                m_strCfgFile;
};
