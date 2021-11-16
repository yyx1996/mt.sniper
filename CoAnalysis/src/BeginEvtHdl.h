#ifndef BEGIN_EVENT_HANDLER_H
#define BEGIN_EVENT_HANDLER_H

#include "SniperKernel/IIncidentHandler.h"

class Task;
class MtFullStateNavBuf;
class IInputSvc;

class MtBeginEvtHdl : public IIncidentHandler
{
    public :

        MtBeginEvtHdl(Task* par, int stoptask_option);

        bool handle(Incident& incident);

    private :

        bool              m_1stCall;
        Task*             m_par;
        MtFullStateNavBuf*  m_buf;
        IInputSvc*        m_iSvc;

        // 0: stop root (default)
        // 1: stop current task
        int m_stoptask_option;
};

#endif
