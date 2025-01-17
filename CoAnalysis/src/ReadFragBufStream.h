#ifndef READ_FRAG_BUF_STREAM_H
#define READ_FRAG_BUF_STREAM_H

#include "RootIOSvc/IInputStream.h"
#include "SniperMuster/GlobalBuffer.h"
#include "SniperMuster/Fragment.h"
#include "EvtNavigator/EvtNavigator.h"
#include "EvtNavigator/NavBuffer.h"

class ReadFragBufStream: public IInputStream{
public:
    ReadFragBufStream();
    virtual bool initialize();
    virtual bool first(bool /*read*/) { return true; }
    virtual bool next(int step = 1, bool read = true);
    virtual TObject* get();

    std::shared_ptr<Fragment<JM::EvtNavigator>>& getNextFrag();

private:
    GlobalBuffer<Fragment<JM::EvtNavigator>>* m_fbuf;
    
    std::shared_ptr<Fragment<JM::EvtNavigator>> m_frag;
    int m_cEvt;
};

#endif
