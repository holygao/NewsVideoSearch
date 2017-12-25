// CFilterGraph.h: interface for the CFilterGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERGRAPH_H__19903DB4_110C_4DC9_8A9C_A839009CAC29__INCLUDED_)
#define AFX_FILTERGRAPH_H__19903DB4_110C_4DC9_8A9C_A839009CAC29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include "streams.h"

//#define	ES_ST_NOTIFY_MEDIA_TYPE	EC_USER + 50
#define WM_GRAPHNOTIFY  (WM_USER+20)

#define MPEG2FRAMELEN			188


class CComEnviroment  
{
public:
	CComEnviroment();			// 初始化COM环境
	virtual ~CComEnviroment();	// 清除COM环境
};


// Filter graph control in this application
class CFilterGraph  
{
public:
	// graph builder interface
	IGraphBuilder   *m_pGB;
	// media control interface
	IMediaControl   *m_pMC;
	// video window interface
	IVideoWindow    *m_pVW;
	// media event interface
	IMediaEventEx   *m_pME;
	// media seeking interface
	IMediaSeeking   *m_pMS;
	// basic audio interface
	IBasicAudio     *m_pBA;
	// Filter running flag
	bool             m_bRun;
	// Filter init flag
	bool             m_bInit;

	HWND             m_hOwner;
	HWND             m_hEventWnd;

	HANDLE			m_hBufFree;
	CMediaType		m_Videomt;
	CMediaType		m_Audiomt;

	IBaseFilter		*m_pSource;

	IBaseFilter		*m_pDemultiplexer;
	IBaseFilter		*m_pVideoDecoder;
	IBaseFilter		*m_pAudioDecoder;

	IBaseFilter		*m_pVMR9;

	BOOL			m_bOnlyAudio;

	BOOL			m_bVideoMedia;
	BOOL			m_bAudioMedia;

	IUnknown		*m_pSourceInterface;
	IUnknown		*m_pTranInterface;

protected:
	int   m_nLeft;
	int   m_nTop;
	int   m_nWidth;
	int   m_nHeight;
	DWORD m_dwGraphRegister;

public:
	CFilterGraph();
	virtual ~CFilterGraph();

public:
	void SetDataEnd();
	void SetFrameLen(int nFrameLen);
	void SetPID(WORD VPID, WORD APID);
	void SetMediaType();
	BOOL GetMediaType(BYTE *pbBuffer, DWORD dwDataLen, WORD VPID, WORD APID);
	HRESULT Stop();
	STDMETHODIMP WriteData(BYTE *pData, int nLen);
	bool GetFullScreen(void);
	bool SetFullScreen(BOOL inEnabled);

	HRESULT ConnectTranToDecoder(IGraphBuilder *pGraph, IBaseFilter *pFilter, IBaseFilter *pVideo, CString strPinName);
	HRESULT ConnectAllFilters();
	HRESULT ConnectFilters(IGraphBuilder *pGraph,IPin *pOut,IBaseFilter *pDest);
	HRESULT GetUnconnectedPin(IBaseFilter *pFilter,PIN_DIRECTION PinDir,IPin **ppPin);
	HRESULT ConnectFilters(IGraphBuilder *pGraph,IBaseFilter *pSrc,IBaseFilter *pDest);
	HRESULT QueryMyInterface();
	HRESULT AddFilterByCLSID(IGraphBuilder *pGraph,const GUID& clsid,LPCWSTR wszName,IBaseFilter **ppF);
	bool ResizeVideoWindow(long inLeft, long inTop, long inWidth, long inHeight);
	bool SetCurrentPosition(double inPosition);
	IMediaEventEx* GetEventHandle(void);
	bool SetNotifyWindow(HWND inWindow);
	bool SetDisplayWindow();
	bool IsInit(void);
	bool IsRunning(void);
	void SetVideoWndOwner(HWND inWnd);
	void AdjustLength(LONGLONG inAdd);
	bool StartGraph(void);
	bool BuildGraph(void);
	void SetWindowPosition(int nLeft, int nTop, int nWidth, int nHeight);

	void ReleaseGraph(void);
	bool ResetGraph(void); // Re-build filter graph

	// Filter graph view tools
	HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void RemoveGraphFromRot(DWORD pdwRegister);
};

#endif // !defined(AFX_FILTERGRAPH_H__19903DB4_110C_4DC9_8A9C_A839009CAC29__INCLUDED_)
