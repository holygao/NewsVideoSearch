; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNewsVideoSearchDlg
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "NewsVideoSearch.h"

ClassCount=4
Class1=CNewsVideoSearchApp
Class2=CNewsVideoSearchDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CVideoThread
Resource3=IDD_NEWSVIDEOSEARCH_DIALOG

[CLS:CNewsVideoSearchApp]
Type=0
HeaderFile=NewsVideoSearch.h
ImplementationFile=NewsVideoSearch.cpp
Filter=N

[CLS:CNewsVideoSearchDlg]
Type=0
HeaderFile=NewsVideoSearchDlg.h
ImplementationFile=NewsVideoSearchDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_STATICVID

[CLS:CAboutDlg]
Type=0
HeaderFile=NewsVideoSearchDlg.h
ImplementationFile=NewsVideoSearchDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_NEWSVIDEOSEARCH_DIALOG]
Type=1
Class=CNewsVideoSearchDlg
ControlCount=19
Control1=IDC_BTNFILEPROC,button,1342242816
Control2=IDC_BTNSEARCH,button,1342242816
Control3=IDC_LIST1,SysListView32,1350631425
Control4=IDC_EDIT1,edit,1484849280
Control5=IDC_PROGRESS1,msctls_progress32,1350565888
Control6=IDC_STATIC,static,1342308353
Control7=IDC_BTNEXPLORE,button,1342242816
Control8=IDC_EDITLABELFILE,edit,1484849280
Control9=IDC_BTNLABELOPEN,button,1342242816
Control10=IDC_STATICSEG,static,1342177540
Control11=IDC_STATICVID,static,1342177540
Control12=IDC_SLIDER1,msctls_trackbar32,1342242840
Control13=IDC_EDITVIDTIME,edit,1484849280
Control14=IDC_EDITVIDTIMET,edit,1484849280
Control15=IDC_EDITRECALL,edit,1484849280
Control16=IDC_EDITPRECISION,edit,1484849280
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_EDITPLAYNAME,edit,1484849281

[CLS:CVideoThread]
Type=0
HeaderFile=VideoThread.h
ImplementationFile=VideoThread.cpp
BaseClass=CWinThread
Filter=N
VirtualFilter=TC

