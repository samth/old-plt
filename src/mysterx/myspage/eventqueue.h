// EventQueue.h : Declaration of the CEventQueue

#ifndef __EVENTQUEUE_H_
#define __EVENTQUEUE_H_

#include "resource.h"       // main symbols

#define MAXQUEUELENGTH 25
#define STREAM_EVENT_SIZE 68  

void failureBox(const char *s);

/////////////////////////////////////////////////////////////////////////////
// CEventQueue
class ATL_NO_VTABLE CEventQueue : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEventQueue, &CLSID_EventQueue>,
	public IDispatchImpl<IEventQueue, &IID_IEventQueue, &LIBID_MYSPAGELib>
  {

private:
  HANDLE stubHandle;
  HANDLE readSem,mutex;
  HANDLE proxyReadSem,proxyMutex;
  int readerNdx,writerNdx;
  int queueLength;
  IEvent *theQueue[MAXQUEUELENGTH];
  
public:
  CEventQueue(void);

DECLARE_REGISTRY_RESOURCEID(IDR_EVENTQUEUE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CEventQueue)
	COM_INTERFACE_ENTRY(IEventQueue)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IEventQueue
public:
	  STDMETHOD(get_EventAvailable)(VARIANT_BOOL *pVal);
	  STDMETHOD(QueueEvent)(IEvent *pEvent);
	  STDMETHOD(GetEvent)(IEvent **ppEvent);
	  STDMETHOD(SetProxySemaphores)(void);
};

#endif //__EVENTQUEUE_H_
