#include "SniperMuster/GlobalStreamMgr.h"

GlobalStreamMgr& GlobalStreamMgr::instance() {
    static GlobalStreamMgr mgr;
    return mgr;
}

void GlobalStreamMgr::put(const std::string& name, GlobalStreamBase* stream) {
    m_GBufMap[name] = stream;
}

GlobalStreamBase* GlobalStreamMgr::get(const std::string& name) {
    return m_GBufMap[name];
}
