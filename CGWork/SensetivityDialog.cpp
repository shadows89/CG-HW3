// SensetivityDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "SensetivityDialog.h"
#include "afxdialogex.h"


// SensetivityDialog dialog

IMPLEMENT_DYNAMIC(SensetivityDialog, CDialog)

SensetivityDialog::SensetivityDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SensetivityDialog::IDD, pParent)
	, fineness(20)
	, d(50)
	, sensetivity(50)
{
}

SensetivityDialog::~SensetivityDialog()
{
}

void SensetivityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Slider(pDX, IDC_SLIDER1, fineness);
	DDV_MinMaxInt(pDX, fineness, 2, 100);
	DDX_Slider(pDX, IDC_SLIDER2, d);
	DDV_MinMaxInt(pDX, d, 0, 100);
	DDX_Slider(pDX, IDC_SLIDER4, sensetivity);
	DDV_MinMaxInt(pDX, sensetivity, 0, 100);
}


BEGIN_MESSAGE_MAP(SensetivityDialog, CDialog)
	
	ON_BN_CLICKED(IDC_BUTTON1, &SensetivityDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// SensetivityDialog message handlers




void SensetivityDialog::OnBnClickedButton1()
{
	CDialog::OnOK();
}
