// CFilterGraph.cpp: implementation of the CFilterGraph class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CFilterGraph.h"
#include "StreamStruct.h"
#include "VideoFrame.h"
#include "Dvdmedia.h"

#include <initguid.h>


#include "IPSDemultip.h"
#include "IPsSource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MPEG2_PACK_SIZE        2048

#define SAFE_DELETE(x)  { if (x) delete x; x = NULL; }
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

//CComPtr<IPsSource> g_pSourceInterface;
//CComPtr<IPSDemultip> g_pTranInterface;

const CLSID CLSID_MPEG2_PSSource = {0xda3ca923, 0xa12d, 0x4ff4, 0x99, 0xc2, 0xe8, 0xaf, 0xfd, 0xaa, 0xbd, 0x4e};

//MPEG2-PSDemultiplexer
const CLSID CLSID_Demultiplexer =  {0x53366c55, 0xf37c, 0x48b5, 0xac, 0xd5, 0xec, 0xb2, 0x2b, 0x67, 0xd8, 0x26};

//Elecard
const CLSID CLSID_VideoDecoder =   {0xF50B3F13, 0x19C4, 0x11CF, 0xAA, 0x9A, 0x02, 0x60, 0x8C, 0x9B, 0xAB, 0xA2};

//MoonLight
const CLSID CLSID_AudioDecoder =   {0xE32C3B01, 0xC81B, 0x4D01, 0x8A, 0xD4, 0x2B, 0x93, 0xF7, 0xFA, 0x54, 0x4C};

//ffshow
//const CLSID CLSID_AudioDecoder =  {0x0F40E1E5,0x4F79,0x4988,0xB1,0xA9,0xCC,0x98,0x79,0x4E,0x6B,0x55};

const CLSID CLSID_VMR9 =           {0x51B4ABF3, 0x748F, 0x4E3B, 0xA2, 0x76, 0xC8, 0x28, 0x33, 0x0E, 0x92, 0x6A};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComEnviroment::CComEnviroment()
{
	CoInitialize(NULL);
}

CComEnviroment::~CComEnviroment()
{
	CoUninitialize();
}

CFilterGraph::CFilterGraph()
{
	// Filter graph running status
	m_bRun  = false;
	m_bInit = false;

	// Interfaces
	m_pGB   = NULL;
	m_pMC   = NULL;
	m_pVW   = NULL;
	m_pME   = NULL;
	m_pMS   = NULL;
	m_pBA   = NULL;

	//m_IPsSource = NULL;

	m_hOwner   = NULL;
	m_hEventWnd = NULL;
	m_nLeft    = 0;
	m_nTop     = 0;
	m_nWidth   = 100;
	m_nHeight  = 100;
	m_dwGraphRegister = 0;

	m_bVideoMedia = FALSE;
	m_bAudioMedia = FALSE;
	m_bOnlyAudio = FALSE;
}

CFilterGraph::~CFilterGraph()
{
	m_bRun = false;
	ReleaseGraph();
}

void CFilterGraph::ReleaseGraph(void)
{
	// Stop filter graph first
	HRESULT  hr;
	if (m_pMC != NULL)
	{
		hr = m_pMC->Stop(); 
	}

	SetNotifyWindow(NULL);
    // Relinquish ownership (IMPORTANT!) after hiding video window
    if(m_pVW != NULL)
    {
        hr = m_pVW->put_Visible(OAFALSE);
        hr = m_pVW->put_Owner(NULL);
    }

	if (m_pGB != NULL)
	{
		// Unregister....
		RemoveGraphFromRot(m_dwGraphRegister);

		// remove the filter added before
		//m_pGB->RemoveFilter(m_pSourceReader);
		// media event interface
		SAFE_RELEASE(m_pME)
		// media seeking interface
		SAFE_RELEASE(m_pMS)
		// media control interface
		SAFE_RELEASE(m_pMC)
		// basic audio interface
		SAFE_RELEASE(m_pBA)
		// video window interface
		SAFE_RELEASE(m_pVW)
		// graph builder interface
		SAFE_RELEASE(m_pGB)
	}

	SAFE_RELEASE(m_pSource);
	SAFE_RELEASE(m_pDemultiplexer);
	SAFE_RELEASE(m_pVideoDecoder);
	SAFE_RELEASE(m_pAudioDecoder);
	SAFE_RELEASE(m_pSourceInterface);
	SAFE_RELEASE(m_pVMR9);
	SAFE_RELEASE(m_pTranInterface);

	m_bRun = false;
}

bool CFilterGraph::BuildGraph(void)
{
	HRESULT hr;
	QueryMyInterface();

	if (m_bInit)
	{
		CMediaType   mt;      
		mt.majortype = MEDIATYPE_Stream;
		mt.subtype   = MEDIASUBTYPE_MPEG2_TRANSPORT;

		hr = AddFilterByCLSID(m_pGB, CLSID_MPEG2_PSSource, L"MPEG2 PS Source", &m_pSource);
		if (FAILED(hr)) 
		{
			AfxMessageBox("add MPEG2 PS Source failed!");
			m_bInit = false;
		}

		IUnknown* pInfSource;
		hr = m_pSource->QueryInterface(IID_IPsSource, (void **)&pInfSource);
		if (FAILED(hr))
		{
			AfxMessageBox("IID_IPsSource failed");
			m_bInit = false;
		}
		//g_pSourceInterface = static_cast<IPsSource*>(pInfSource);
		m_pSourceInterface = (IPsSource*)(pInfSource);

					
		hr = AddFilterByCLSID(m_pGB, CLSID_Demultiplexer, 
			L"My MPEG2 Demultiplexer", &m_pDemultiplexer);
		if (FAILED(hr)) 
		{
			AfxMessageBox("add Demultiplexer failed!");
			m_bInit = false;
		} 
		IUnknown* pInfTran;
		hr = m_pDemultiplexer->QueryInterface(IID_IPSDemultip, (void **)&pInfTran);
		if (FAILED(hr))
		{
			AfxMessageBox("IID_IPSDemultip failed");
			m_bInit = false;
		}
		//g_pTranInterface = static_cast<IPSDemultip*>(pInfTran);
		m_pTranInterface = (IPSDemultip*)(pInfTran);

		hr = AddFilterByCLSID(m_pGB, CLSID_VideoDecoder, 
			L"Elecard MPEG Video Decoder", &m_pVideoDecoder);
		if (FAILED(hr)) 
		{
			AfxMessageBox("add Video Decoder failed!");
			m_bInit = false;
		}
		hr = AddFilterByCLSID(m_pGB, CLSID_AudioDecoder, 
			L"ffdshow Audio Decoder", &m_pAudioDecoder);
		if (FAILED(hr)) 
		{
			AfxMessageBox("add audio Decoder failed!");
			m_bInit = false;
		}

		hr = AddFilterByCLSID(m_pGB, CLSID_VMR9, L"VMR9", &m_pVMR9);
		if (FAILED(hr)) 
		{
			AfxMessageBox("add VMR9 failed!");
			m_bInit = false;
		}
	}

	return m_bInit;
}

void CFilterGraph::SetWindowPosition(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_nLeft    = nLeft;
	m_nTop     = nTop;
	m_nWidth   = nWidth;
	m_nHeight  = nHeight;
}

bool CFilterGraph::StartGraph(void)
{
	if (m_bInit)
	{
		char buf[100];
		HRESULT hr;
		m_bRun = true;
			
		hr = ConnectAllFilters();
		if(FAILED(hr))
		{
			AMGetErrorText(hr, buf, 100);
			AfxMessageBox(buf);
		}
		SetDisplayWindow();
		SetNotifyWindow(m_hEventWnd);
		// run the filter graph

		hr = m_pMC->Run();
		if(FAILED(hr))
		{
			AMGetErrorText(hr, buf, 100);
			AfxMessageBox(buf);
		}
		return true;
	}
	
	return false;
}

void CFilterGraph::SetVideoWndOwner(HWND inWnd)
{
	m_hOwner = inWnd;
}

bool CFilterGraph::IsInit(void)
{
	return m_bInit;
}

bool CFilterGraph::IsRunning(void)
{
	return m_bRun;
}

void CFilterGraph::AdjustLength(LONGLONG inAdd)
{
//	if (m_pSourceStream)
//	{
//		m_pSourceStream->AddAvailableLength(inAdd);
//	}

}

bool CFilterGraph::ResetGraph(void)
{
	if (m_bRun)
	{
		// If the filter graph running, release it first
		ReleaseGraph();
	}
//	// Reset the datalist
//	if (m_pDataList != NULL)
//	{
//		m_pDataList->ResetList();
//		ResetEvent(m_pDataList->m_hBufEnough);
//	}

	// Build a new filter graph
	BuildGraph();
	return true;
}

///////////////////////////////////////////////////////////////////////////
HRESULT CFilterGraph::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[128];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}

void CFilterGraph::RemoveGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}
//////////////////////////////////////////////////////////////////////////////

bool CFilterGraph::SetDisplayWindow()
{
	if (m_pVW)
	{
		// Hide the video window first
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner((OAHWND)m_hOwner);
		
		RECT windowRect;
		::GetClientRect(m_hOwner, &windowRect);
		m_pVW->put_Left(0);
		m_pVW->put_Top(0);
		m_pVW->put_Width(windowRect.right - windowRect.left);
		m_pVW->put_Height(windowRect.bottom - windowRect.top);
		m_pVW->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		
		m_pVW->put_MessageDrain((OAHWND) m_hOwner);
		// Restore the video window
		if (m_hOwner != NULL)
		{
			//	mVideoWindow->put_Visible(lVisible);
			m_pVW->put_Visible(OATRUE);
		}
		else
		{
			m_pVW->put_Visible(OAFALSE);
		}
		return true;
	}
	return false;
}

bool CFilterGraph::SetNotifyWindow(HWND inWindow)
{
	if (m_pME)
	{
		m_hEventWnd = inWindow;
		m_pME->SetNotifyWindow((OAHWND)inWindow, WM_GRAPHNOTIFY, 0);
		return true;
	}
	return false;
}

IMediaEventEx* CFilterGraph::GetEventHandle()
{
	return m_pME;
}

bool CFilterGraph::SetCurrentPosition(double inPosition)
{
	//if (mSeeking)
	//	{
	//		__int64 one = 10000000;
	//		__int64 position = (__int64)(one * inPosition);
	//		HRESULT hr = mSeeking->SetPositions(&position, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, 
	//			0, AM_SEEKING_NoPositioning);
	//		return SUCCEEDED(hr);
	//	}
		return false;
	
}

bool CFilterGraph::ResizeVideoWindow(long inLeft, long inTop, long inWidth, long inHeight)
{

	if (m_pVW)
	{
		long lVisible = OATRUE;
		m_pVW->get_Visible(&lVisible);
		// Hide the video window first
		m_pVW->put_Visible(OAFALSE);
		
		m_pVW->put_Left(inLeft);
		m_pVW->put_Top(inTop);
		m_pVW->put_Width(inWidth);
		m_pVW->put_Height(inHeight);
		
		// Restore the video window
		m_pVW->put_Visible(lVisible);
		return true;
	}
	return false;
}

HRESULT CFilterGraph::AddFilterByCLSID(IGraphBuilder *pGraph, const GUID &clsid, LPCWSTR wszName, IBaseFilter **ppF)
{
	if (!pGraph || ! ppF) return E_POINTER;
    *ppF = 0;
    IBaseFilter *pF = 0;
    HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, reinterpret_cast<void**>(&pF));
    if (SUCCEEDED(hr))
    {
        hr = pGraph->AddFilter(pF, wszName);
        if (SUCCEEDED(hr))
            *ppF = pF;
        else
            pF->Release();
    }
    return hr;
}

HRESULT CFilterGraph::QueryMyInterface()
{
		m_bInit = false;
	//  Create filter graph
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph,
                                  NULL,
                                  CLSCTX_INPROC,
                                  IID_IGraphBuilder,
                                  (void**)&m_pGB);
	if (SUCCEEDED(hr))
	{
		m_bInit = true;
		// Register...
		AddGraphToRot((IUnknown *)m_pGB, &m_dwGraphRegister);
	}

	if (m_bInit)
	{
		// Media control interface
		hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
		if (FAILED(hr))
		{
			AfxMessageBox("IID_IMediaControl failed");
			m_bInit = false;
		}
	}

	if (m_bInit)
	{
		// Media event interface
		hr = m_pGB->QueryInterface(IID_IMediaEvent, (void **)&m_pME);
		if (FAILED(hr)) 
		{
			AfxMessageBox("IID_IMediaEvent failed");
			m_bInit = false;
		}
	}
	
	if (m_bInit)
	{
		// Media video window interface
		hr = m_pGB->QueryInterface(IID_IVideoWindow, (void **)&m_pVW);
		if (FAILED(hr)) 
		{
			AfxMessageBox("IID_IVideoWindow failed");
			m_bInit = false;
		}
	}

	if (m_bInit)
	{
		// Media basic audio interface
		hr = m_pGB->QueryInterface(IID_IBasicAudio, (void **)&m_pBA);
		if (FAILED(hr)) 
		{
			AfxMessageBox("IID_IBasicAudio failed");
			m_bInit = false;
		}
	}

	if (m_bInit)
	{
		// Media seeking interface
		hr = m_pGB->QueryInterface(IID_IMediaSeeking, (void **)&m_pMS);
		if (FAILED(hr)) 
		{
			AfxMessageBox("IID_IMediaSeeking failed");
			m_bInit = false;
		}
	}
	return hr;
}

HRESULT CFilterGraph::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
	if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

    // Find an output pin on the first filter.
    IPin *pOut = 0;
    HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);

    if (FAILED(hr)) 
    {
		char buf[100];
		AMGetErrorText(hr, buf, 100);
		AfxMessageBox(buf);
        return hr;
    }
    hr = ConnectFilters(pGraph, pOut, pDest);
    pOut->Release();
    return hr;
}

HRESULT CFilterGraph::GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	*ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
	pEnum->Reset();
	hr=pEnum->Next(1, &pPin, NULL);
    while (hr == S_OK)
    {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;
            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
		hr=pEnum->Next(1, &pPin, NULL);
    }
	char buf[100];

	AMGetErrorText(hr, buf, 100);
    pEnum->Release();
    // Did not find a matching pin.
    return E_FAIL;
}

HRESULT CFilterGraph::ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest)
{
if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }
#ifdef debug
        PIN_DIRECTION PinDir;
        pOut->QueryDirection(&PinDir);
        _ASSERTE(PinDir == PINDIR_OUTPUT);
#endif

    // Find an input pin on the downstream filter.
    IPin *pIn = 0;
    HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if (FAILED(hr))
    {
        return hr;
    }
    // Try to connect them.
    hr = pGraph->Connect(pOut, pIn);
	if(FAILED(hr))
	{
		char buf[100];
		AMGetErrorText(hr, buf, 100);
	}

    pIn->Release();
    return hr;
}

HRESULT CFilterGraph::ConnectTranToDecoder(IGraphBuilder *pGraph, IBaseFilter *pFilter, IBaseFilter *pVideo, CString strPinName)
{
    IEnumPins *pEnum = 0;
    IPin *pPin=0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
	pEnum->Reset();
	hr=pEnum->Next(1, &pPin, NULL);
    while (hr == S_OK)
    {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
		PIN_INFO PINInfo;
		pPin->QueryPinInfo(&PINInfo);
		CString str(PINInfo.achName);
		
        if (ThisPinDir==PINDIR_OUTPUT&&str==strPinName)
        {
            IPin *pTmp = 0;
            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
              
				IPin *pInPIN;
				hr = GetUnconnectedPin(pVideo, PINDIR_INPUT, &pInPIN);
				if(FAILED(hr))
				{
					AfxMessageBox("Get VideoDecoder InPin failed!");
					return E_FAIL;
				}
				hr = pGraph->Connect(pPin, pInPIN);
				if(FAILED(hr))
				{
					char buf[100];
					AMGetErrorText(hr, buf, 100);
					AfxMessageBox(buf);
				}

                return S_OK;
            }
        }
        pPin->Release();
		hr=pEnum->Next(1, &pPin, NULL);
    }

    pEnum->Release();
    // Did not find a matching pin.
    return E_FAIL;
}

HRESULT CFilterGraph::ConnectAllFilters()
{
	HRESULT hr;
	char buf[100];

	hr = ConnectFilters(m_pGB, m_pSource, m_pDemultiplexer);
	if (FAILED(hr)) 
	{
		AMGetErrorText(hr, buf, 100);
		//AfxMessageBox(buf);
		m_bInit = false;
	}

	IPin *pOut = 0;
	if(!m_bOnlyAudio)
	{
		hr = ConnectTranToDecoder(m_pGB, m_pDemultiplexer, m_pVideoDecoder, "VideoOut");
		if (FAILED(hr)) 
		{
			AMGetErrorText(hr, buf, 100);
			//AfxMessageBox(buf);
			m_bInit = false;
		}
		
		GetUnconnectedPin(m_pVideoDecoder, PINDIR_OUTPUT, &pOut);
		hr  = m_pGB->Render(pOut);
		if(FAILED(hr))
		{
			m_bInit = false;
			//AfxMessageBox("Video Render failed!");
		}
		pOut->Release();
	}

	hr = ConnectTranToDecoder(m_pGB, m_pDemultiplexer, m_pAudioDecoder, "AudioOut");
	if (FAILED(hr)) 
	{
		AMGetErrorText(hr, buf, 100);
		//AfxMessageBox(buf);
		m_bInit = false;
	}
	
	pOut = NULL;
	GetUnconnectedPin(m_pAudioDecoder, PINDIR_OUTPUT, &pOut);
	hr  = m_pGB->Render(pOut);
	if(FAILED(hr))
	{
		m_bInit = false;
		AfxMessageBox("Audio Render failed!");
	}
	pOut->Release();

	return hr;
}

bool CFilterGraph::SetFullScreen(BOOL inEnabled)
{
	if (m_pVW)
	{
		HRESULT hr = m_pVW->put_FullScreenMode(inEnabled ? OATRUE : OAFALSE);
		return SUCCEEDED(hr);
	}
	return false;
}


bool CFilterGraph::GetFullScreen()
{
	if (m_pVW)
	{
		long  fullScreenMode = OAFALSE;
		m_pVW->get_FullScreenMode(&fullScreenMode);
		return (fullScreenMode == OATRUE);
	}
	return false;
}

STDMETHODIMP CFilterGraph::WriteData(BYTE *pData, int nLen)
{
	HRESULT hr = NOERROR;
	if(m_pSourceInterface)
	{
		hr = ((IPsSource *)m_pSourceInterface)->GetData(pData, nLen);
	}
	else
	{
		AfxMessageBox("setBuffer failed!");
	}
	return hr;
}

HRESULT CFilterGraph::Stop()
{
	HRESULT hr;

	//g_pSourceInterface->setStop(TRUE);
	hr = m_pMC->Stop();
	//g_pSourceInterface->setStop(FALSE);
	if(FAILED(hr))
	{
		AfxMessageBox("Failed to stop Graph");
		return hr;
	}
	return S_OK;
}

BOOL CFilterGraph::GetMediaType(BYTE *pbBuffer, DWORD dwDataLen, WORD VPID, WORD APID)
{
	PBYTE	pbTmp = NULL;
	DWORD	es_len = 0;
	DWORD	i, j;
	CMPEG2_PES_SECTION pes;
	WORD	PID;
	BYTE	Payload_unit_start_indicator, adaptation_fiele_control;
	BYTE	adaptation_length;
	int		nPESHeadLen;
	
	m_bVideoMedia = FALSE;
	m_bAudioMedia = FALSE;
	if(VPID==0)
	{
		m_bVideoMedia = TRUE;
		m_bOnlyAudio = TRUE;
	}
	j = 0;
	while(j<dwDataLen)
	{
		pbTmp = pbBuffer + j ;
		if(pbTmp[0]!=0x47 ) 
		{
			j++;
			continue;
		}

		PID = (pbTmp[1])&0x1f;
		PID = (PID<<8)|pbTmp[2];
		
		adaptation_fiele_control = (pbTmp[3]>>4)&0x03;
		if(adaptation_fiele_control==0x02)
		{
			j = j + MPEG2FRAMELEN;
			continue ;
		}
		adaptation_length = 0;
		if(adaptation_fiele_control==0x03)
		{
			adaptation_length = pbTmp[4] + 1;
			if(adaptation_length>183)
			{
				j = j + MPEG2FRAMELEN;
				continue ;
			}
		}

		if(PID==VPID&&m_bVideoMedia==FALSE)
		{
			Payload_unit_start_indicator = 	pbTmp[1]&0x40;
//			if(Payload_unit_start_indicator!=0x40)
//			{
//				j = j + MPEG2FRAMELEN;
//				continue ;
//			}

			pes.ParseFromMem( pbTmp, MPEG2FRAMELEN);
			if(pes.trans_error_indicator || pes.trans_scrambling_control)
			{
				j = j + MPEG2FRAMELEN;
				continue ;
			}
			
			//	填充
			if ( pes.pid == 0x1FFF )
			{
				j = j + MPEG2FRAMELEN;
				continue;
			}
			//	找到起始帧
			//	有一种情况，各个节目的pcr由音频提供，
			//	视频的组头不出现在payload_unit_start_indicator＝＝1的帧，
			//	而是出现在adaptation_field_constrol＝＝0x03的帧。为了得到视频参数
			if (pes.payload_unit_start_indicator||(pes.adaptation_field_constrol == 0x03))
			{
				for(i=4; i<188; i++)
				{
					if (pbTmp[i]	==	0x00	&&
						pbTmp[i+1]	==	0x00	&&
						pbTmp[i+2]	==	0x01	&&
						pbTmp[i+3]	==	0xB3	)
					{
						//	找到视频序列头	
						CVideoFrame VideoFrame;	
						es_len = 188 - i;
						if( !VideoFrame.Parse( pbTmp + i, es_len ) )
						{
							j = j + MPEG2FRAMELEN;
							continue;		
						}
						
						m_Videomt.SetType(&MEDIATYPE_Video);
						m_Videomt.SetSubtype(&MEDIASUBTYPE_MPEG2_VIDEO);
						m_Videomt.SetFormatType(&FORMAT_MPEG2Video);
						
						MPEG2VIDEOINFO* pvi = 
							(MPEG2VIDEOINFO*)m_Videomt.AllocFormatBuffer( sizeof(MPEG2VIDEOINFO)+VideoFrame.Length() );
						
						ZeroMemory(pvi,m_Videomt.FormatLength());
						//	RECT rcSource = {0, 0, 160, 120}; 
						RECT rcSource			= {0,0,0,0};
						RECT rcTarget			= rcSource;
						pvi->hdr.rcTarget		= rcTarget;
						pvi->hdr.rcSource		= rcSource;
						pvi->hdr.dwBitRate		= VideoFrame.Bitrate();
						pvi->hdr.AvgTimePerFrame= VideoFrame.FrameTime();
						// MPEG-2 profile + MPEG-2 level
						pvi->dwProfile			= AM_MPEG2Profile_Main;
						pvi->dwLevel			= AM_MPEG2Level_Main;
						// BITMAPINFOHEADER information.
						pvi->hdr.bmiHeader.biSize	= sizeof(BITMAPINFOHEADER);
						pvi->hdr.bmiHeader.biWidth	= VideoFrame.Width();
						pvi->hdr.bmiHeader.biHeight	= VideoFrame.Height();
						// 拷贝头结构
						pvi->cbSequenceHeader = VideoFrame.Length();
						memcpy(	pvi->dwSequenceHeader,VideoFrame.Header(),VideoFrame.Length());
						// 定义外观比例							
						switch( VideoFrame.AspectRatio() )
						{
						case 2:
							pvi->hdr.dwPictAspectRatioX = 4;
							pvi->hdr.dwPictAspectRatioY = 3;
							break;
						case 3:
							pvi->hdr.dwPictAspectRatioX = 16;
							pvi->hdr.dwPictAspectRatioY = 9;
							break;
						default:
							break;
						}
						m_bVideoMedia = TRUE;
						TRACE("\nVideo mt ok\n");
						if(m_bVideoMedia&&m_bAudioMedia)
							return TRUE;
						else
							break;
					}
				}
				j = j + MPEG2FRAMELEN;
			}
			else
				j = j + MPEG2FRAMELEN;
		}
		else if(PID==APID&&m_bAudioMedia==FALSE)
		{
			CAudioFrame AudioFrame;

			nPESHeadLen = 0;
			Payload_unit_start_indicator = 	pbTmp[1]&0x40;
			if(Payload_unit_start_indicator==0x40)
			{
				nPESHeadLen = 9 + pbTmp[4+adaptation_length+8];
			}

			if(AudioFrame.Locate(pbTmp+4+adaptation_length+nPESHeadLen, 184-adaptation_length-nPESHeadLen))
			{
				m_Audiomt.SetType(&MEDIATYPE_Audio);
				m_Audiomt.SetSubtype(&MEDIASUBTYPE_MPEG1AudioPayload);
				m_Audiomt.SetFormatType(&FORMAT_WaveFormatEx);
				m_Audiomt.SetFormat((BYTE*)AudioFrame.Format(),sizeof(MPEG1WAVEFORMAT)+AudioFrame.Length());
	
				m_bAudioMedia = TRUE;
				TRACE("\nAudio mt ok\n");
				if(m_bVideoMedia&&m_bAudioMedia)
					return TRUE;
			}
			else
			{
				j = j + MPEG2FRAMELEN;
				continue;
			}
		}
		else
		{
			j = j + MPEG2FRAMELEN;
			continue;
		}
	}
	return FALSE;
}

void CFilterGraph::SetMediaType()
{
	((IPSDemultip *)m_pTranInterface)->SetVideoType(&m_Videomt, &m_Audiomt);
}

void CFilterGraph::SetPID(WORD VPID, WORD APID)
{
	((IPSDemultip *)m_pTranInterface)->SetPID(VPID, APID);
	((IPsSource *)m_pSourceInterface)->SetPID(VPID, APID);
}

void CFilterGraph::SetFrameLen(int nFrameLen)
{
	((IPsSource *)m_pSourceInterface)->setFrameLength(nFrameLen);
	((IPSDemultip *)m_pTranInterface)->SetFrameLen(nFrameLen);
}


void CFilterGraph::SetDataEnd()
{
	((IPsSource *)m_pSourceInterface)->SetDataEnd();
}

