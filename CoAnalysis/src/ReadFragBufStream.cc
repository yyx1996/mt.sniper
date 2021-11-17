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
    return true;
}

bool ReadFragBufStream::next(int /*step*/, bool /*read*/){
    return false;
}

TObject* ReadFragBufStream::get(){
    return nullptr; 
}

std::shared_ptr<Fragment<JM::EvtNavigator>>& ReadFragBufStream::getNextFrag()
{
    auto _pElm = m_fbuf->next();
    if ( _pElm != nullptr ) {
        m_frag = _pElm->dptr;
        m_fbuf->setDone(_pElm);
    }
    else {
        m_frag = nullptr;
    }
    return m_frag;
}
