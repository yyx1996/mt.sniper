#include "EndEvtHdl.h"
#include "FullStateNavBuf.h"
#include "RootIOSvc/RootOutputSvc.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/Incident.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperException.h"

MtEndEvtHdl::MtEndEvtHdl(Task* par)
    : IIncidentHandler(par), m_par(par)
{
    SniperPtr<RootOutputSvc> oSvc(*m_par, "OutputSvc");
    if ( oSvc.invalid() ) {
        LogFatal << "cann't find OutputSvc for "
                 << m_par->scope() << m_par->objName() << std::endl;
        throw SniperException("OutputSvc is invalid");
    }
    m_oSvc = oSvc.data();

    SniperDataPtr<MtFullStateNavBuf> pBuf(m_par, "/Event");
    if ( pBuf.invalid() ) {
        LogError << "cann't get the EvtNavigator Buffer" << std::endl;
        throw SniperException("NavBuffer is invalid");
    }
    m_buf = pBuf.data();
}

bool MtEndEvtHdl::handle(Incident& /*incident*/)
{
    LogDebug << "Cur output svc: " << m_oSvc << std::endl;
    LogDebug << "Cur Buffer: " << m_buf << std::endl;
    if (m_buf->size() == 0) {
        LogError << "There is nothing in Cur Buffer." << std::endl;
        return false;
    }
    LogDebug << "CurEvt in Buffer: " << m_buf->curEvt() << std::endl;
    if (not m_buf->curEvt()) {
        LogError << "There is no data in the buffer" << std::endl;
        return false;
    }
    LogDebug << "Cur Buffer: " << m_buf << std::endl;
    LogDebug << "CurEvt in Buffer: " << m_buf->curEvt() << std::endl;
    bool okay = m_oSvc->write(m_buf->curEvt());
    if (!okay) {
      // If writing is failed, end run immediately
      // FIXME Not an elegant way
      LogFatal << "Failed to write event data!!!" << std::endl;
      assert(okay);
    }
    return okay;
}
