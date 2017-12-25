// NewsVideoSearch.h : main header file for the NEWSVIDEOSEARCH application
//

#if !defined(AFX_NEWSVIDEOSEARCH_H__EF4922D6_2849_4B95_A6D0_5B657FD256A6__INCLUDED_)
#define AFX_NEWSVIDEOSEARCH_H__EF4922D6_2849_4B95_A6D0_5B657FD256A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNewsVideoSearchApp:
// See NewsVideoSearch.cpp for the implementation of this class
//

class CNewsVideoSearchApp : public CWinApp
{
public:
	CNewsVideoSearchApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewsVideoSearchApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNewsVideoSearchApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSVIDEOSEARCH_H__EF4922D6_2849_4B95_A6D0_5B657FD256A6__INCLUDED_)
