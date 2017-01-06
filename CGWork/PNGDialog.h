#pragma once


// PNGDialog dialog

class PNGDialog : public CDialog
{
	DECLARE_DYNAMIC(PNGDialog)

public:
	PNGDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~PNGDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int png_width;
	int png_height;
	CString png_filename;
};
