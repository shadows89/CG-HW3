#pragma once


// SensetivityDialog dialog

class SensetivityDialog : public CDialog
{
	DECLARE_DYNAMIC(SensetivityDialog)

public:
	SensetivityDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SensetivityDialog();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SMALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedButton1();
	int fineness;
	int d;
	int sensetivity;
};
