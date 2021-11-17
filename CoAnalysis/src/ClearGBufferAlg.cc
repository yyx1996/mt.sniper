#include "SniperKernel/AlgBase.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"
#include "SniperMuster/Fragment.h"
#include "SniperMuster/GlobalStream.h"
#include "EvtNavigator/EvtNavigator.h"
#include "EvtNavigator/NavBuffer.h"
#include "SniperKernel/Task.h"

#include <memory>

using namespace std;
using namespace JM;

typedef Fragment<EvtNavigator> Frag;

class ClearGBufferAlg: public AlgBase{
public:
    ClearGBufferAlg(const string& name);
    virtual ~ClearGBufferAlg();

    virtual bool initialize();
    virtual bool execute();
    virtual bool finalize();

private:
    GlobalBuffer<Frag>* m_gbuf;

};


DECLARE_ALGORITHM(ClearGBufferAlg);

ClearGBufferAlg::ClearGBufferAlg(const string& name):
    AlgBase(name){

    }

ClearGBufferAlg::~ClearGBufferAlg(){

}

bool ClearGBufferAlg::initialize(){
    m_gbuf = GlobalStream<Frag>::GetBuffer("GFragStream");
    return true;
}

bool ClearGBufferAlg::execute(){
    auto result = m_gbuf->pop_front();

    if(result == nullptr){
        dynamic_cast<Task*>(m_par)->stop();
    }

    return true;
}


bool ClearGBufferAlg::finalize(){
    return true;
}
