#include "SniperKernel/AlgBase.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"
#include "SniperMuster/Fragment.h"
#include "SniperMuster/GlobalStream.h"
#include "EvtNavigator/EvtNavigator.h"
#include "EvtNavigator/NavBuffer.h"
#include <BufferMemMgr/IDataMemMgr.h>
#include <memory>

using namespace std;
using namespace JM;

typedef Fragment<EvtNavigator> Frag;

class PackFragAlg: public AlgBase{
public:
    PackFragAlg(const string& name);
    virtual ~PackFragAlg();

    virtual bool initialize();
    virtual bool execute();
    virtual bool finalize();

private:
    int m_max;
    GlobalBuffer<Frag>* m_gbuf;
    // NavBuffer*          m_nbuf;  
    IDataMemMgr* m_buf;

    int m_maxFragSize;//fragment中循环区事例数量
    int m_curFragSize;
    shared_ptr<Frag> m_frag;
    void addEvent();
    void fillFBuf();
};

DECLARE_ALGORITHM(PackFragAlg);

PackFragAlg::PackFragAlg(const string& name):
    AlgBase(name),m_curFragSize(0),
    m_frag(new Frag){
        declProp("MaxFragSize", m_maxFragSize = 100);
    }

PackFragAlg::~PackFragAlg(){
    std::cout<<"############################Destruct PackFragAlg!!##########"<<std::endl;
}

bool PackFragAlg::initialize(){
    //获取存放fragment 的globalbuffer
    m_gbuf = GlobalStream<Frag>::GetBuffer("GFragStream");
    std::cout << __FILE__ << " gbuf : " << m_gbuf << std::endl;
    
    //get the instance of NavBuffer
    // SniperDataPtr<JM::NavBuffer>  navBuf(getParent(), "/Event");
    // if ( navBuf.invalid() ) {
    //     LogError << "cannot get the NavBuffer @ /Event" << std::endl;
    //     return false;
    // }
    // m_nbuf = navBuf.data();

    // SniperPtr<IDataMemMgr> navBuf(*getParent(), "BufferMemMgr");
    // if ( navBuf.invalid() ) {
    //     LogError << "cannot get the NavBuffer @ /Event" << std::endl;
    //     return false;
    // }
    // m_buf = navBuf.data();

    return true;
}


bool PackFragAlg::execute(){
    addEvent();
    
    if(m_curFragSize >= m_maxFragSize){//判断是否应该把Fragment填入GlobalBuffer
        fillFBuf();
        m_frag.reset(new Frag);
        //addEvent();
    }

    return true;
}

bool PackFragAlg::finalize(){
    m_frag = nullptr;
    m_gbuf->push_back(m_frag);
    //sleep(10);
    std::cout<<"############################PackFragAlg finalize!!##########"<<std::endl;
    return true;
}

void PackFragAlg::addEvent(){
    if(m_curFragSize == 0){//如果当前被填的Fragment为空，需要先将冗余窗口填进来
        // m_frag->evtDeque.insert(m_frag->evtDeque.end(), m_nbuf->begin(), m_nbuf->current());
        m_frag->lbegin = m_frag->evtDeque.size();//标记事例循环区的开始(标志的是循环的第一个事例)
    }

    std::shared_ptr<EvtNavigator> evtnav(new EvtNavigator());
    m_frag->evtDeque.push_back(evtnav);
    m_curFragSize++;
}

void PackFragAlg::fillFBuf(){
    m_frag->lend = m_frag->evtDeque.size();//标记事例循环区的结束（标记的是最后一个循环事例的后一个事例）
    // m_frag->evtDeque.insert(m_frag->evtDeque.end(), m_nbuf->current(), m_nbuf->end());

    m_gbuf->push_back(m_frag);
    m_frag=nullptr;
    m_curFragSize = 0;
}
