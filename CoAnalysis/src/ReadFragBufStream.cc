#include "ReadFragBufStream.h"
#include "SniperMuster/GlobalStream.h"

typedef GlobalBuffer<Fragment<JM::EvtNavigator>> FragBuf; 
typedef GlobalStream<Fragment<JM::EvtNavigator>> FragStream;

ReadFragBufStream::ReadFragBufStream():
    m_fbuf(nullptr),
    m_frag(nullptr),
    m_cEvt(-1){}

bool ReadFragBufStream::initialize(){
    m_fbuf = FragStream::GetBuffer("GFragStream");
    return this->next();
}

bool ReadFragBufStream::next(int /*step*/, bool /*read*/){
    if(m_frag != nullptr && ++m_cEvt < m_frag->evtDeque.size()) return true;
    
    //更新fragment
    auto _pElm = m_fbuf->next();
    if ( _pElm != nullptr ) {
        m_frag = _pElm->dptr;
        m_fbuf->setDone(_pElm);      //将拿到的Elm的状态设为完成，允许被Buffer清理掉
        m_cEvt = -1;
        return next();
    }

    return false;
}

TObject* ReadFragBufStream::get(){
    JM::EvtNavigator* nav = (m_frag->evtDeque[m_cEvt]).get();    
    return nav; 
}

std::shared_ptr<JM::EvtNavigator>& ReadFragBufStream::getRef()
{
    return m_frag->evtDeque[m_cEvt];
}
