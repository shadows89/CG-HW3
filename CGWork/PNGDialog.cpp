// PNGDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CGWork.h"
#include "PNGDialog.h"
#include "afxdialogex.h"


// PNGDialog dialog

IMPLEMENT_DYNAMIC(PNGDialog, CDialog)

PNGDialog::PNGDialog(CWnd* pParent /*=NULL*/)
	: CDialog(PNGDialog::IDD, pParent)
	, png_width(0)
	, png_height(0)
	, png_filename(_T(""))
{

}

PNGDialog::~PNGDialog()
{
}

void PNGDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, png_width);
	DDV_MinMaxInt(pDX, png_width, 1, 2550);
	DDX_Text(pDX, IDC_EDIT2, png_height);
	DDV_MinMaxInt(pDX, png_height, 1, 2550);
	DDX_Text(pDX, IDC_EDIT3, png_filename);
	DDV_MaxChars(pDX, png_filename, 32);
}


BEGIN_MESSAGE_MAP(PNGDialog, CDialog)
END_MESSAGE_MAP()


// PNGDialog message handlers
