// NewsVideoSearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewsVideoSearch.h"
#include "NewsVideoSearchDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewsVideoSearchDlg dialog

CNewsVideoSearchDlg::CNewsVideoSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewsVideoSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewsVideoSearchDlg)
	m_sPath = _T("G:\\NewsVidData");
	m_LabelFile = _T("G:\NewsVidData\\VideoNews.txt");
	bPlay = FALSE;
	bPause = false;
	bsegPause = false;
	bFindLabel = false ;
	m_FltSegGraph = NULL;
	m_FilterGraph = NULL;
	m_VidTimeT = 0.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNewsVideoSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewsVideoSearchDlg)
	DDX_Control(pDX, IDC_EDITPLAYNAME, m_PlayName);
	DDX_Control(pDX, IDC_EDITPRECISION, m_Precision);
	DDX_Control(pDX, IDC_EDITRECALL, m_Recall);
	DDX_Control(pDX, IDC_STATICSEG, m_VidSegWindow);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderGraph);
	DDX_Control(pDX, IDC_STATICVID, m_VideoWindow);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_BTNSEARCH, m_xSearch);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT1, m_sPath);
	DDX_Text(pDX, IDC_EDITLABELFILE, m_LabelFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewsVideoSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CNewsVideoSearchDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNSEARCH, OnBtnsearch)
	ON_BN_CLICKED(IDC_BTNFILEPROC, OnBtnfileproc)
	ON_BN_CLICKED(IDC_BTNEXPLORE, OnBtnexplore)
	ON_BN_CLICKED(IDC_BTNLABELOPEN, OnBtnlabelopen)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnRclickList1)
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_STATICVID, OnStaticvid)
	ON_BN_CLICKED(IDC_STATICSEG, OnStaticseg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewsVideoSearchDlg message handlers

BOOL CNewsVideoSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	m_VidSegWindow.ModifyStyle(0, WS_CLIPCHILDREN);
	m_VideoWindow.ModifyStyle(0, WS_CLIPCHILDREN);
	m_SliderGraph.SetRange(0, 1000);
	m_SliderGraph.SetPos(0);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	bTimer = FALSE;//对定时器初始化

	m_List.InsertColumn(0,"视频文件名",LVCFMT_CENTER,290);
    m_List.InsertColumn(1,"视频片花名",LVCFMT_CENTER,290);
    m_List.InsertColumn(2,"标注时间",LVCFMT_CENTER,60);
    m_List.InsertColumn(3,"发现时间",LVCFMT_CENTER,60);
	m_List.InsertColumn(4,"标注帧",LVCFMT_CENTER,55);
    m_List.InsertColumn(5,"发现帧",LVCFMT_CENTER,55);
	m_List.InsertColumn(6,"总差值",LVCFMT_CENTER,55);
	m_List.InsertColumn(7,"总帧数",LVCFMT_CENTER,55);
	m_List.InsertColumn(8,"帧率",LVCFMT_CENTER,45);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT); 

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNewsVideoSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNewsVideoSearchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNewsVideoSearchDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CALLBACK BrowseCallbackProc(HWND   hwnd,UINT   uMsg,LPARAM   lp,   LPARAM   pData)   
{   
    TCHAR   szDir[MAX_PATH];   
    switch(uMsg)   
	{   
	case   BFFM_INITIALIZED:   
		{   
			if   (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR),       szDir))   
				SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);   
			
			break;   
		}   
	case   BFFM_SELCHANGED:   
		{   
			//   Set   the   status   window   to   the   currently   selected   path.   
			if   (SHGetPathFromIDList((LPITEMIDLIST)   lp   ,szDir))   
				SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);   
			
			break;   
		}   
	default:   
		break;   
    }   
    return   0;   
}  

/*void CNewsVideoSearchDlg::OnBtnopen() 
{
	// TODO: Add your control notification handler code here
	char   *m_path;   
    char   pszDisplayName[MAX_PATH];     
    BROWSEINFO   BrowseInfo;     
    LPITEMIDLIST   pidlBrowse;     
    LPMALLOC   pMalloc;     
	int i;
	
	memcpy(pszDisplayName, m_sPath.GetBuffer(0), m_sPath.GetLength());
	pszDisplayName[m_sPath.GetLength()] = 0;
	
    if(!SUCCEEDED(SHGetMalloc(&pMalloc))   )     
		return;     
    BrowseInfo.hwndOwner   =   m_hWnd;     
    BrowseInfo.pidlRoot   =   0;     
    BrowseInfo.pszDisplayName   =   pszDisplayName;     
    BrowseInfo.lpszTitle   =   "请指定路径";     
    BrowseInfo.ulFlags   =   0x40|BIF_RETURNONLYFSDIRS;//|BIF_DONTGOBELOWDOMAIN;     
    BrowseInfo.lpfn   =   BrowseCallbackProc; 
    //BrowseInfo.lpfn   =   NULL; 
    BrowseInfo.lParam   =   0;     
    BrowseInfo.iImage   =   0;     
    pidlBrowse   =   SHBrowseForFolder(&BrowseInfo);     
    if(pidlBrowse!=NULL   )     
    {     
		if(SHGetPathFromIDList(pidlBrowse, pszDisplayName)   )     
		{     
			m_path   =   pszDisplayName;     
		}     
		pMalloc->Free(pidlBrowse);     
	} 
    else
		return;
	
	pMalloc->Release(); 
	m_List.DeleteAllItems();
	UpdateData(1);
	
	CFileFind tempFind; 
	char tempFileFind[MAX_PATH]; 
	CStringArray folderarray;
	
	sprintf(tempFileFind,"%s\\*.*",m_path);
    folderarray.Add(tempFileFind);
	
	while(folderarray.GetSize()>0)
	{
		CString nextpath=folderarray.GetAt(0);
		//TRACE("555%s\n",nextpath);
		BOOL	IsFinded=(BOOL)tempFind.FindFile(nextpath);
		folderarray.RemoveAt(0);
		while(IsFinded)
		{
			
			IsFinded=tempFind.FindNextFile();
			if(tempFind.IsDirectory() && !tempFind.IsDots())
			{
				//TRACE("%s\n",tempFind.GetFilePath());
				folderarray.Add(tempFind.GetFilePath()+"\\*.*");
			}
			else
			{				
				CString filepath=	tempFind.GetFilePath();
				CString filelen  = tempFind.GetLength();
				filepath.MakeLower();
				if(filepath.Right(4)==".mpg" ||
					filepath.Right(5)==".mpeg")
				{
					if (filepath.Find("seg") == -1) 
					{
						int iRow = m_List.InsertItem(9999999l,filepath,0);
						//m_List.SetItemText(iRow,1,filelen);
						mVideo.push_back(filepath);
					}
					else
					{
						mVideoSeg.push_back(filepath);
//						int tmp = FindDatePos(filepath);
//						CString clipChanlName = filepath.Left(tmp);
//						int listnum = m_List.GetItemCount();
//						for (i=0;i<listnum;i++)
//						{
//							CString videoName = m_List.GetItemText(i,0);
//							if (videoName.Find(clipChanlName) != -1) 
//							{
//								m_List.SetItemText(i,1,filepath);
//							}
// 						}
					}

				}				
			}			
		}
		tempFind.Close();		
	}
	GetSegChanlName();
	m_sPath.Format("%s", m_path);
	UpdateData(0);
}*/

void CNewsVideoSearchDlg::OnBtnsearch() 
{
	// TODO: Add your control notification handler code here
	m_xSearch.EnableWindow(0);

	AfxBeginThread((AFX_THREADPROC)SearchSeg, this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED);
	
//	CVideoThread *p = new CVideoThread();
//	p->m_pDlg = this;
//	p->CreateThread();

}

int CNewsVideoSearchDlg::ReadLabelFile(char* strFilePath)
{
	labeldata lbd;
	std::fstream fs;

	fs.open(strFilePath,std::ios::in);
	if (!fs) 
	{
		return false;
	}
	int   intTmp = -1;
	float floTmp = -0.1;
	std::string strtmp;
	CString     cstrTemp;
	m_labelNum = 0;

	while( !fs.eof() )
	{
		fs >> strtmp;
		cstrTemp = strtmp.c_str();
		if (cstrTemp.Find("mpg"))
		{
			//bFind = true;
			lbd.vidFName = cstrTemp;
			fs >> strtmp;
			sscanf(strtmp.c_str(), "%d", &intTmp);
			lbd.framePos = intTmp;
			intTmp = -1;

			fs >> strtmp;
			sscanf(strtmp.c_str(), "%f", &floTmp);
			lbd.timePos = floTmp;
			floTmp = -0.1;

			fs >> strtmp;
			sscanf(strtmp.c_str(), "%d", &intTmp);
			lbd.totalDif = intTmp;
			intTmp = -1;
			mLabelData.push_back(lbd);

			m_labelNum++;

		}
		else
			break;
	} 
	
	int intTemp = mLabelData.size();
	return intTemp;


}

int CNewsVideoSearchDlg::FindDatePos(CString strVideoName)
{
	CString str = strVideoName;
	int tmp = str.Find(".mpg");
	//PTV_NEWS_20091215_122627_005.mpg, 连续八个数字
	for (int i=0;i<tmp;i++)
	{
		if (str.GetAt(i  ) >= 0x30 && str.GetAt(i  ) <= 0x39 &&
			str.GetAt(i+1) >= 0x30 && str.GetAt(i+1) <= 0x39 &&
			str.GetAt(i+2) >= 0x30 && str.GetAt(i+2) <= 0x39 &&
			str.GetAt(i+3) >= 0x30 && str.GetAt(i+3) <= 0x39 &&
			str.GetAt(i+4) >= 0x30 && str.GetAt(i+4) <= 0x39 &&
			str.GetAt(i+5) >= 0x30 && str.GetAt(i+5) <= 0x39 &&
			str.GetAt(i+6) >= 0x30 && str.GetAt(i+6) <= 0x39 &&
			str.GetAt(i+7) >= 0x30 && str.GetAt(i+7) <= 0x39 )
			return i-1;		
	}
	//CCTV_9-19-32-47-001segCCTVInternation1.mpg,日期格式
	for (i=0;i<tmp;i++)
	{
		if (str.GetAt(i  ) >= 0x30 && str.GetAt(i  ) <= 0x32 &&
			str.GetAt(i+1) >= 0x30 && str.GetAt(i+1) <= 0x39 &&
			str.GetAt(i+3) >= 0x30 && str.GetAt(i+3) <= 0x35 &&
			str.GetAt(i+4) >= 0x30 && str.GetAt(i+4) <= 0x39 &&
			str.GetAt(i+6) >= 0x30 && str.GetAt(i+6) <= 0x35 &&
			str.GetAt(i+7) >= 0x30 && str.GetAt(i+7) <= 0x39 )
			return i-1; 
	}
}

void CNewsVideoSearchDlg::WriteFileHeader(CString FName)
{
	CFile* m_ResFile;
	m_ResFile = new CFile;
	if (!m_ResFile->Open(FName,CFile::modeCreate|CFile::modeWrite)) 
	{
		AfxMessageBox("文件打开失败！");
	}
	CString str = "";
	char Ret[] = {0x0d,0x0a,0};
	str = str + "视频片断\t" + "                     视频片花\t                    " + "标注位置\t" + "计算位置\t" + Ret;
	if (m_ResFile!=NULL)
	{
		m_ResFile->Write(str,str.GetLength());
	}
}

void CNewsVideoSearchDlg::WriteSearchRes(CString FName,CString videoName, double res, int index)
{
	CFile* m_ResFile;
	m_ResFile = new CFile;
	if (!m_ResFile->Open(FName,CFile::modeWrite)) 
	{
		AfxMessageBox("文件打开失败！");
	}
	CString str="",strTemp="";
	char Ret[] = {0x0d,0x0a,0};
	strTemp.Format("%3d",index);
	str += strTemp + "\t";
	str += videoName + "\t";
	strTemp.Format("%2.3f",res);
	str += strTemp + "\t";	
	str  = str + Ret;
	if (m_ResFile!=NULL) 
	{
		m_ResFile->Write(str,str.GetLength());
	}
}

void CNewsVideoSearchDlg::_ThreadExit()
{
	m_xSearch.EnableWindow(1);
}

int CNewsVideoSearchDlg::GetSegChanlName()
{
	std::vector<CString>::iterator iter;
	CString strTemp  = "";
	CString strTemp1 = "";
	char chTemp = NULL;
	int intTemp  = 0;
	int intTemp1 = 0;
	int intTemp2 = 0;

	for (iter = mVideoSeg.begin();iter != mVideoSeg.end();iter++)
	{
		strTemp = *iter;
		intTemp = strTemp.ReverseFind('\\');
		if (intTemp == -1)
			continue;
		intTemp2 = strTemp.GetLength();
		strTemp1 = strTemp.Right(intTemp2 - intTemp -1);
		intTemp = FindDatePos(strTemp1);
 		strTemp = strTemp1.Left(intTemp);

		chTemp = strTemp.GetAt(intTemp-1);
		if (chTemp == '-' || chTemp == '_') 
		{
			intTemp--;
			strTemp = strTemp.Left(intTemp);
		}
		chTemp = strTemp.GetAt(intTemp-1);
		if (chTemp == '-' || chTemp == '_') 
		{
			intTemp--;
			strTemp = strTemp.Left(intTemp);
		}
		mSegChanl.push_back(strTemp);
	}
	if (mSegChanl.size() == mVideoSeg.size())
	{
		return mSegChanl.size();
	}
}

unsigned int CNewsVideoSearchDlg::SearchSeg(void *p)
{
	CNewsVideoSearchDlg *pDlg = (CNewsVideoSearchDlg* )p;		
	
	int nVidNum = pDlg->m_List.GetItemCount();
	int nSegNum = pDlg->mVideoSeg.size();
	CString vidPath, segPath, strTemp, strTemp1;
	int *vidFrmCnt;
	int *segFrmCnt;
	int frameDiff = 0;
//	std::vector<int> vidFrmCnt;
// 	std::vector<int> segFrmCnt;
	vidFrmCnt = new int[3000];
	segFrmCnt = new int[300];
	pDlg->m_progress.SetRange(0,nVidNum-1);

	LPTSTR labFName=NULL;
	labFName=new TCHAR[pDlg->m_LabelFile.GetLength()+1];
	_tcscpy(labFName, pDlg->m_LabelFile);
	int lbdnum = pDlg->ReadLabelFile(labFName);

		for(int i = 0;i<nVidNum;i++)
		{
			pDlg->m_frmDiff = 0;
			CString sVideo = pDlg->m_List.GetItemText(i,0);
			

//			for (int k = 0;k<pDlg->m_labelNum;k++)
//			{
//				strTemp = pDlg->mLabelData[k].vidFName;
// 			}
			strTemp1 = pDlg->TruncPath(sVideo);
			
			for (int j = 0;j<pDlg->m_labelNum;j++)
			{
				strTemp = pDlg->mLabelData[j].vidFName;
				if (strTemp1.CompareNoCase(strTemp) == 0)
				{
					pDlg->bFindLabel = true;
					strTemp.Format("%2.3f",pDlg->mLabelData[j].timePos);
					pDlg->m_List.SetItemText(i,2,strTemp);//标注时间
					strTemp = "";
					strTemp.Format("%d",pDlg->mLabelData[j].framePos);
					pDlg->m_List.SetItemText(i,4,strTemp);//标注帧
					strTemp = "";
					break;
				}
				else
					pDlg->bFindLabel = false;
			}
			if (pDlg->bFindLabel == false)  
			{			
				pDlg->m_List.SetItemText(i,2,"NoF");//没有标注时间
				pDlg->m_List.SetItemText(i,4,"NoF");//没有标注帧
 			}
			segPath = pDlg->FindItsSeg(sVideo); 
			if (segPath != "") 
			{
				int vRtn = pDlg->GetFrameDataCnt(sVideo,vidFrmCnt);//返回值为该段视频帧的总数
				pDlg->m_List.SetItemText(i,1,segPath);//对应片花名
				int sRtn = pDlg->GetFrameDataCnt(segPath,segFrmCnt);
				int frmpos = pDlg->SimilarDetect(vRtn,sRtn,vidFrmCnt,segFrmCnt,&frameDiff);
				strTemp.Format("%d",frmpos);
				pDlg->m_List.SetItemText(i,5,strTemp);//发现帧位置
				strTemp = "";
				strTemp.Format("%d",pDlg->m_frmDiff);
				pDlg->m_List.SetItemText(i,6,strTemp);//帧差值
				float frmrate = pDlg->GetFrameRate(segPath);
				float timepos = 0.0;
				if((abs(frmrate) > 0.0000000001) && (vRtn != 0))
				{
					timepos = (float)vRtn/frmrate;
					timepos = (float)frmpos/vRtn*timepos;
					strTemp.Format("%2.3f",timepos);
					pDlg->m_progress.SetPos(i);
					pDlg->m_List.SetItemText(i,3,strTemp);//发现时间
					strTemp = "";
					strTemp.Format("%d",vRtn);
					pDlg->m_List.SetItemText(i,7,strTemp);//总帧数
					strTemp = "";
					strTemp.Format("%2.1f",frmrate);
					pDlg->m_List.SetItemText(i,8,strTemp);//帧速率
				}
			}
			else
			{
				continue;
			}
		}

	delete []vidFrmCnt;
	delete []segFrmCnt;
	pDlg->m_xSearch.EnableWindow(1);
	return 0;
}
//int CNewsVideoSearchDlg::GetFrameDataCnt(CString segname, std::vector<int>& frmdata)
int CNewsVideoSearchDlg::GetFrameDataCnt(CString segname, int *frmdata)
{
	std::vector<int> frmDataVec;
	std::vector<int>::iterator iter;
	unsigned char* filedata;
	CFile file;	
	int filelen = 0;
	if (!file.Open(segname,CFile::modeRead|CFile::shareDenyWrite))
	{
		AfxMessageBox("file open error!");
	}
	else
		filelen = file.GetLength();
	filedata = new unsigned char[filelen];		
	file.Read(filedata,filelen);
	file.Close();

	UINT startpos,i,j,k,tmp;
	CString str;
	int framenum = 0;
	int framepos[2];
	int posindi = 0;
	char chTemp;
	framepos[0] = framepos[1] = 0;

	for(i=0; i<188; i++) 
	{ 		
		if((filedata[i]==0x47)&&(filedata[i+188]==0x47)&&(filedata[i+188*2]==0x47))
		{
			startpos = i;
			break;         //满足条件后跳出循环
		}		
	}

	bool bFind = false;
	for(i= startpos;i<filelen;i=i+188)
	{   
		for (j = 0;j<188;j++)
		{
			if(filedata[i+j  ]==0x00 && filedata[i+j+1]==0x00&&
			   filedata[i+j+2]==0x01 && filedata[i+j+3]==0x00)
			{
				//framenum++;
				chTemp = filedata[i+j+5]&0x38;
				if (chTemp == 0x08)
				{
					startpos = i;
					bFind = true;
					break;
				}				
			}
		}
		if (bFind)
			break;
		
	}
	for(i= startpos;i<filelen;i=i+188)
	{	   
		for (j=0;j<188;j++)
		{
			if(filedata[i+j  ] == 0x00 && filedata[i+j+1]==0x00&&
			   filedata[i+j+2] == 0x01 && filedata[i+j+3]==0x00)
			{
				framenum++;
				framepos[posindi] = i+j;
				if (framenum > 0)
				{
					tmp = framepos[posindi] - framepos[1 - posindi];
					frmDataVec.push_back(tmp); 
					tmp = 0;
				}
				posindi = 1 - posindi;
				break;				
			}			
		}		
	}
	delete []filedata;
	k=0;
	for (iter = frmDataVec.begin(); iter !=  frmDataVec.end(); iter++) 
	{
		frmdata[k] = *iter;
		k++;
	}
	if (framenum) 
		return framenum;
		//return frmDataVec;
	else
		return 0;
	
}

CString CNewsVideoSearchDlg::FindItsSeg(CString videoName)
{
	CString strTemp = "";
	CString strTemp1 = "";
	int segNum = mVideoSeg.size();
	int chlNum = mSegChanl.size();
	if (segNum != chlNum)
	{
		return "";
	}
	//iterSeg = pDlg->mVideoSeg.begin();
//	for (iterSeg = pDlg->mVideoSeg.begin();iterSeg != pDlg->mVideoSeg.end();iterSeg++)
	for (int i=0;i<chlNum;i++) 
	{
		strTemp  = mSegChanl[i];
		//strTemp1 = pDlg->mVideoSeg[i];
		//if ( (videoName.Find(strTemp) != -1) && (strTemp1.Find(strTemp) != -1) )
		if(videoName.Find(strTemp) != -1)
		{
			//iterSeg = pDlg->mVideoSeg.begin();
			//return iterSeg[i];
			return mVideoSeg[i];
		}
		//else
			//iterSeg = pDlg->mVideoSeg.begin();
			//return "";
	}	
}

int CNewsVideoSearchDlg::SimilarDetect(int vidFrmNum, int segFrmNum, int *vidFrmCnt, int *segFrmCnt,int* diff)
{
	int winlength=0;
	int *totaldiffer;
	if (vidFrmNum-segFrmNum > 0)
	{
		totaldiffer = new int[vidFrmNum-segFrmNum];
	}
	else
		return 0;
		//AfxMessageBox("视频文件小于片花文件!");
	winlength = segFrmNum;
	for (int i=0;i<vidFrmNum-winlength;i++)
	{
		totaldiffer[i] = 0;
		for (int j=0;j<winlength;j++)
		{
			totaldiffer[i] = totaldiffer[i] + abs(vidFrmCnt[i+j] - segFrmCnt[j]);		
		}
	}
	
	int mindiffer=0,similarframepos=0;
	mindiffer = totaldiffer[0];
	for (int m=1;m<vidFrmNum-winlength;m++)
	{
		if (totaldiffer[m]<mindiffer)
		{
			mindiffer = totaldiffer[m];
			similarframepos = m;
		}
	}
	m_frmDiff = mindiffer;
	diff = &mindiffer;
	delete []totaldiffer;
	return similarframepos;
}

float CNewsVideoSearchDlg::GetFrameRate(CString segName)
{
	char frmRate = NULL;
	float frameRate = 0.0;
	bool bFind = false;

	unsigned char* filedata;
	CFile file;	
	int filelen = 0;
	if (!file.Open(segName,CFile::modeRead|CFile::shareDenyWrite))
	{
		AfxMessageBox("file open error!");
	}
	else
	{
		filelen = file.GetLength();
		filedata = new unsigned char[filelen];		
		file.Read(filedata,filelen);
		file.Close();
	}

	for (int i=0;i<filelen;i++) 
	{
		if((filedata[i  ]==0x00) && (filedata[i+1]==0x00)&&
		   (filedata[i+2]==0x01) && (filedata[i+3]==0xb3))
		{
			frmRate = filedata[i+7]&0x0F;
			switch(frmRate) 
			{
			case 0x01:
				frameRate = 23.976;
				return frameRate;
				//bFind = true;
				break;
			case 0x02:
				frameRate = 24; 
				return frameRate;
				//bFind = true;
				break;
			case 0x03:
				frameRate = 25; 
				return frameRate;
			//	bFind = true;
				break;
			case 0x04:
				frameRate = 29.97;
				return frameRate;
				//bFind = true;
				break;
			case 0x05:
				frameRate = 30;
				return frameRate;
				//bFind = true;
				break;
			case 0x06:
				frameRate = 50;
				return frameRate;
				//bFind = true;
				break;
			case 0x07:
				frameRate = 59.94;
				return frameRate;
				//bFind = true;
				break;
			case 0x08:
				frameRate = 60;
				return frameRate;
				//bFind = true;
				break;			
			}
//			if (bFind == true)
// 				return frameRate;
		}
	}
	return frameRate;
	if (!frameRate) 
		return 0.0;
	delete filedata;
}

void CNewsVideoSearchDlg::OnBtnfileproc() 
{
	// TODO: Add your control notification handler code here
	CFileFind tempFind; 
	char tempFileFind[MAX_PATH]; 
	CStringArray folderarray;
	
	sprintf(tempFileFind,"%s\\*.*",m_sPath);
    folderarray.Add(tempFileFind);
	
	while(folderarray.GetSize()>0)
	{
		CString nextpath=folderarray.GetAt(0);
		//TRACE("555%s\n",nextpath);
		BOOL	IsFinded=(BOOL)tempFind.FindFile(nextpath);
		folderarray.RemoveAt(0);
		while(IsFinded)
		{			
			IsFinded=tempFind.FindNextFile();
			if(tempFind.IsDirectory() && !tempFind.IsDots())
			{
				//TRACE("%s\n",tempFind.GetFilePath());
				folderarray.Add(tempFind.GetFilePath()+"\\*.*");
			}
			else
			{				
				CString filepath = tempFind.GetFilePath();
				CString filelen  = tempFind.GetLength();
				filepath.MakeLower();
				if(filepath.Right(4)==".mpg" ||
					filepath.Right(5)==".mpeg")
				{
					if (filepath.Find("seg") == -1) 
					{
						int iRow = m_List.InsertItem(9999999l,filepath,0);
						//m_List.SetItemText(iRow,1,filelen);
						mVideo.push_back(filepath);
					}
					else
					{
						mVideoSeg.push_back(filepath);
//						int tmp = FindDatePos(filepath);
//						CString clipChanlName = filepath.Left(tmp);
//						int listnum = m_List.GetItemCount();
//						for (int i=0;i<listnum;i++)
//						{
//							CString videoName = m_List.GetItemText(i,0);
//							if (videoName.Find(clipChanlName) != -1) 
//							{
//								m_List.SetItemText(i,1,filepath);
//							}
//  						}
					}

				}				
			}			
		}
		tempFind.Close();		
	}

	GetSegChanlName();
	//m_sPath.Format("%s", m_path);
	UpdateData(0);
}

void CNewsVideoSearchDlg::OnBtnexplore() 
{
	// TODO: Add your control notification handler code here
	BROWSEINFO bi;
	LPMALLOC pMalloc;
	int bFile = 0;

	if (SUCCEEDED(SHGetMalloc(&pMalloc)))//Allocate Memory
	{
		ZeroMemory(&bi, sizeof(bi));
		_TCHAR pszDirName[MAX_PATH];
		LPITEMIDLIST pidl;
		bi.hwndOwner = NULL;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszDirName;
		bi.lpszTitle = _T("选择图像数据所在目录");
		bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS|BIF_DONTGOBELOWDOMAIN;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = IDR_MAINFRAME;
		
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)//Pop up a dialog
		{
			if (::SHGetPathFromIDList(pidl, pszDirName))//Obtain the directory name
			{
				m_sPath	= pszDirName;
			}
			pMalloc->Free(pidl);			
			pMalloc->Release();
		}
		else
		{
		    pMalloc->Release();
			return;
		}
		GetDlgItem(IDC_EDIT1)->SetWindowText(m_sPath);
	}
	else 
		return;
}

void CNewsVideoSearchDlg::OnBtnlabelopen() 
{
	// TODO: Add your control notification handler code here
	CFileDialog fd(TRUE, NULL, NULL, OFN_HIDEREADONLY, "txt文件(*.txt)|*.txt||",0);
	if (fd.DoModal()!=IDOK)
	{
		return;
	}
	m_LabelFile = fd.GetPathName();
	UpdateData(0);
}

void CNewsVideoSearchDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nIndex = pNMListView->iItem;
	CRect itemrect;
	CWnd *m_wnd = GetDlgItem(IDC_STATICVID);
	m_wnd->GetWindowRect(itemrect);
	CString videoName;
	CString strTemp = "";
	videoName = m_List.GetItemText(nIndex,0);
	CreateGraph(videoName);
	CString playname = TruncPath(videoName);
	SetDlgItemText(IDC_EDITPLAYNAME,playname);
	double timePos;
	if (nIndex != -1)
	{		
		strTemp = m_List.GetItemText(nIndex,3); //发现时间
		if (strTemp == "")
		timePos = 0;
		timePos = atof(strTemp);
		double duration;
		m_FilterGraph->GetDuration(&duration); //单个数值用传引用方式
		m_VidTimeT = duration;
		strTemp.Format("%2.3f",duration);
		SetDlgItemText(IDC_EDITVIDTIMET,strTemp);
		m_FilterGraph->SetStartStopPosition(timePos, duration);
		m_FilterGraph->Run();
        //m_FilterGraph->SetCurrentPosition(timePos);
		bPlay = TRUE;

		//启动定时器
		if (!bTimer)
		{
			bTimer = TRUE;
			this->SetTimer(1, 100, NULL);//SetTimer(1,100,NULL);
		}
		
		//镜头信息指针复位
		//result=result-nIndex;	
	}	
	*pResult = 0;
}

void CNewsVideoSearchDlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nIndex = pNMListView->iItem;
	CRect itemrect;
	CWnd *m_wnd = GetDlgItem(IDC_STATICVID);
	m_wnd->GetWindowRect(itemrect);
	CString vidsegName;
	vidsegName = m_List.GetItemText(nIndex,1);

	if(m_FltSegGraph)
	{
		// Stop the filter graph first
		m_FltSegGraph->Stop();
		m_FltSegGraph->SetNotifyWindow(NULL);		
		delete m_FltSegGraph;
		m_FltSegGraph = NULL;
	}
	m_FltSegGraph = new CDXGraph();
	if (m_FltSegGraph->Create())
	{
		m_FltSegGraph->RenderFile(vidsegName);
		m_FltSegGraph->SetDisplayWindow(m_VidSegWindow.GetSafeHwnd());
		m_FltSegGraph->SetNotifyWindow(this->GetSafeHwnd());
		m_FltSegGraph->Run();
		
	}
		//m_FilterGraph->Run();
	bPlay = TRUE;	
	bsegPause = false;;
	*pResult = 0;
}

void CNewsVideoSearchDlg::CreateGraph(CString vidName)
{
	DestroyGraph();
	
	m_FilterGraph = new CDXGraph();
	if (m_FilterGraph->Create())
	{
		// Render the source clip
		m_FilterGraph->RenderFile(vidName);
		// Set video window and notification window
		m_FilterGraph->SetDisplayWindow(m_VideoWindow.GetSafeHwnd());
		//	m_FilterGraph->SetDisplayWindow(NULL);
		m_FilterGraph->SetNotifyWindow(this->GetSafeHwnd());
		// Show the first frame
		m_FilterGraph->Pause();
		
	}
}

void CNewsVideoSearchDlg::DestroyGraph(void)
{
	if(m_FilterGraph)
	{
		// Stop the filter graph first
		m_FilterGraph->Stop();
		m_FilterGraph->SetNotifyWindow(NULL);
		
		
		delete m_FilterGraph;
		m_FilterGraph = NULL;
	}
}

void CNewsVideoSearchDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CNewsVideoSearchDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CString strTemp = "";
	if (bTimer && m_FilterGraph)
	{
		double pos = 0, duration = 1.;
		m_FilterGraph->GetCurrentPosition(&pos);
		strTemp.Format("%2.3f",pos);
		SetDlgItemText(IDC_EDITVIDTIME,strTemp);
		//m_FilterGraph->GetDuration(&duration);
		// Get the new position, and update the slider
		int newPos = int(pos * 1000 / m_VidTimeT);
		if (m_SliderGraph.GetPos() != newPos)
		{
			m_SliderGraph.SetPos(newPos);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CNewsVideoSearchDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar->GetSafeHwnd() == m_SliderGraph.GetSafeHwnd())
	{
		if (m_FilterGraph)
		{
			// Calculate the current seeking position
			double duration = 1.;
			m_FilterGraph->GetDuration(&duration);
			double pos = duration * m_SliderGraph.GetPos() / 1000.;
			m_FilterGraph->SetCurrentPosition(pos);
		}		
	}
	else
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}

void CNewsVideoSearchDlg::OnStaticvid() //单击暂停，再单击播放
{
	// TODO: Add your control notification handler code here
	if (m_FilterGraph)
	{
		bPause = !bPause;
		if(bPause)
		{
			m_FilterGraph->Pause();
		}
		else
			m_FilterGraph->Run();
	}
}


CString CNewsVideoSearchDlg::TruncPath(CString pathname)
{
	int intTmp1 = 0;
	int intTmp2 = 0;
	intTmp1 = pathname.ReverseFind('\\');
	intTmp2 = pathname.GetLength();
	CString filename = pathname.Right(intTmp2 - intTmp1 -1);
	return filename;
}

void CNewsVideoSearchDlg::OnStaticseg() //单击暂停，再单击播放
{
	// TODO: Add your control notification handler code here
	if (m_FltSegGraph)
	{
		bPause = !bPause;
		if(bPause)
		{
			m_FltSegGraph->Pause();
		}
		else
			m_FltSegGraph->Run();
	}
}
