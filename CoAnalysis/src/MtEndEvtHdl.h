#ifndef MT_END_EVENT_HANDLER_H
#define MT_END_EVENT_HANDLER_H

#include "SniperKernel/IIncidentHandler.h"

class Task;
class MtFullStateNavBuf;
class RootOutputSvc;

class MtEndEvtHdl : public IIncidentHandler
{
    public :

        MtEndEvtHdl(Task* par);

        bool handle(Incident& incident);

    private :

        Task*             m_par;
        MtFullStateNavBuf*  m_buf;
        RootOutputSvc*    m_oSvc;
};

#endif
