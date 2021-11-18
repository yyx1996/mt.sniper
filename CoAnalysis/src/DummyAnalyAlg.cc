#include "SniperKernel/AlgBase.h"
#include "SniperKernel/AlgFactory.h"
#include "EvtNavigator/EvtNavigator.h"
#include "EvtNavigator/NavBuffer.h"

using namespace std;
using namespace JM;

class DummyAnalyAlg: public AlgBase{
public:
    DummyAnalyAlg(const string& name);
    virtual ~DummyAnalyAlg();

    virtual bool initialize();
    virtual bool execute();
    virtual bool finalize();

private:
    NavBuffer* m_nbuf;

};

DECLARE_ALGORITHM(DummyAnalyAlg);

DummyAnalyAlg::DummyAnalyAlg(const string& name):
    AlgBase(name){
    
}

DummyAnalyAlg::~DummyAnalyAlg(){
    
}

bool DummyAnalyAlg::initialize(){
    //get the instance of NavBuffer
    SniperDataPtr<JM::NavBuffer>  navBuf(getParent(), "/Event");
    if ( navBuf.invalid() ) {
        LogError << "cannot get the NavBuffer @ /Event" << std::endl;
        return false;
    }
    m_nbuf = navBuf.data();
    return true;
}

bool DummyAnalyAlg::execute(){
    auto cEvt = m_nbuf->curEvt();
    //伪算法运算

    if ( this->logLevel() < 3 ) {
        SniperLog::Logger::lock();
        std::cout << "Get " << cEvt->getHeader("/Event/OEC")->EventID();
        for ( auto it = m_nbuf->begin(); it != m_nbuf->end(); ++it ) {
            std::cout << " " << *it;
        }
        std::cout << std::endl;
        SniperLog::Logger::unlock();
    }

    return true;
}

bool DummyAnalyAlg::finalize(){
    //delete m_nbuf;
    return true;
}




















