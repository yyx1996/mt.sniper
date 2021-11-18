#include "MtBeginEvtHdl.h"
#include "MtFullStateNavBuf.h"
#include "RootIOSvc/IInputSvc.h"
#include "SniperKernel/Incident.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperException.h"
#include "SniperKernel/Task.h"

MtBeginEvtHdl::MtBeginEvtHdl(Task* par, int stoptask_option)
    : IIncidentHandler(par), m_1stCall(true),
      m_par(par), m_stoptask_option(stoptask_option)
{
    SniperPtr<IInputSvc> iSvc(*m_par, "InputSvc");
    if ( iSvc.invalid() ) {
        LogFatal << "cann't find InputSvc for "
                 << m_par->scope() << m_par->objName() << std::endl;
        throw SniperException("InputSvc is invalid");
    }
    m_iSvc = iSvc.data();

    SniperDataPtr<MtFullStateNavBuf> pBuf(m_par, "/Event");
    if ( pBuf.invalid() ) {
        LogError << "cann't get the EvtNavigator Buffer" << std::endl;
        throw SniperException("NavBuffer is invalid");
    }
    m_buf = pBuf.data();
}

bool MtBeginEvtHdl::handle(Incident& /*incident*/)
{
    if ( m_1stCall ) {
        m_buf->init(m_iSvc->getInputStream("EvtNavigator"));
        m_1stCall = false;
    }

    if ( m_buf->next() ) {
        return true;
    }
    
    return m_par->stop();
}
