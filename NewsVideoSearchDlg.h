// NewsVideoSearchDlg.h : header file
//

#if !defined(AFX_NEWSVIDEOSEARCHDLG_H__4B577A13_9BB7_45A8_B85E_E3EF80556369__INCLUDED_)
#define AFX_NEWSVIDEOSEARCHDLG_H__4B577A13_9BB7_45A8_B85E_E3EF80556369__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNewsVideoSearchDlg dialog
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <string>
#include "CDXGraph.h"
#include <io.h>
//#include "CFilterGraph.h"
//#include <iostream>

class CNewsVideoSearchDlg : public CDialog
{
// Construction
public:
	CString TruncPath(CString pathname);
	void DestroyGraph();
	void CreateGraph(CString vidName);
	static unsigned int SearchSeg(void *p);
	//int GetFrameDataCnt(CString segname, std::vector<int>& frmdata);
	int GetFrameDataCnt(CString segname, int *frmdata);
	float GetFrameRate(CString segName);
	int SimilarDetect(int vidFrmNum, int segFrmNum, int *vidFrmCnt, int *segFrmCnt,int* diff);
	int		GetSegChanlName();
	CString FindItsSeg(CString videoName);
	void	_ThreadExit();
	void WriteSearchRes(CString FName,CString videoName, double res, int index);
	void WriteFileHeader(CString FName);
	int FindDatePos(CString strVideoName);
	int ReadLabelFile(char* strFilePath);
	CNewsVideoSearchDlg(CWnd* pParent = NULL);	// standard constructor
	typedef struct
	{
		CString     vidFName;
		int			framePos;
		float		timePos;
		int         totalDif;
	}labeldata;

	std::vector<CString> mVideo;
	std::vector<CString> mVideoSeg;
	std::vector<CString> mSegChanl;
	std::vector<int>     msegFrameCnt;
	std::vector<labeldata> mLabelData;

	CDXGraph* m_FilterGraph;
	CDXGraph* m_FltSegGraph;
	double    m_VidTimeT;
	int       m_frmDiff;
	int       m_labelNum;
	//设置定时器
	BOOL bTimer;
	//设置播放标志
	BOOL bPlay;
	bool bPause;
	bool bsegPause;
	bool bFindLabel;
// Dialog Data
	//{{AFX_DATA(CNewsVideoSearchDlg)
	enum { IDD = IDD_NEWSVIDEOSEARCH_DIALOG };
	CEdit	m_PlayName;
	CEdit	m_Precision;
	CEdit	m_Recall;
	CStatic	m_VidSegWindow;
	CSliderCtrl	m_SliderGraph;
	CStatic	m_VideoWindow;
	CProgressCtrl	m_progress;
	CButton	m_xSearch;
	CListCtrl	m_List;
	CString	m_sPath;
	CString	m_LabelFile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewsVideoSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNewsVideoSearchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnsearch();
	afx_msg void OnBtnfileproc();
	afx_msg void OnBtnexplore();
	afx_msg void OnBtnlabelopen();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnStaticvid();
	afx_msg void OnStaticseg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSVIDEOSEARCHDLG_H__4B577A13_9BB7_45A8_B85E_E3EF80556369__INCLUDED_)
