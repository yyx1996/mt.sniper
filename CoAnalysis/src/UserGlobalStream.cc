#include "SniperMuster/GlobalStream.h"
#include "SniperMuster/GlobalStreamFactory.h"
#include "SniperMuster/Fragment.h"
#include "EvtNavigator/EvtNavigator.h"

//BookGlobalStream MusterBook_GlobalStreamJMEvtNavigator_("GlobalStream<JM::EvtNavigator>", &MusterCreateGS<GlobalStream<JM::EvtNavigator>>);

typedef GlobalStream<Fragment<JM::EvtNavigator>> FragmentStream;

// template<>
// std::map<std::string, FragmentStream*> FragmentStream::s_GBufMap;
//template<>
//typename std::map<std::string, GlobalStream<Fragment<JM::EvtNavigator>>*> GlobalStream<Fragment<JM::EvtNavigator>>:: s_GBufMap;
//
DECLARE_GLOBAL_STREAM(FragmentStream);//注册存放Fragment的GlobalStream


