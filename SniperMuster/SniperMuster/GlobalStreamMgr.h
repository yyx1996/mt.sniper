#ifndef GlobalStreamMgr_h
#define GlobalStreamMgr_h

#include "SniperPrivate/GlobalStreamBase.h"
#include <map>

class GlobalStreamMgr {
public:
    static GlobalStreamMgr& instance();

    void put(const std::string&, GlobalStreamBase*);
    GlobalStreamBase* get(const std::string&);

    void join();

private:
    std::map<std::string, GlobalStreamBase*> m_GBufMap;
};

#endif

