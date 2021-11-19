#include "SniperKernel/AlgBase.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperMuster/Fragment.h"
#include "SniperMuster/GlobalStream.h"
#include "EvtNavigator/EvtNavigator.h"
#include "RootIOSvc/NavInputStream.h"
#include "Event/OECHeader.h"
#include <fstream>

class PackIndexAlg : public AlgBase
{
public:
    typedef Fragment<JM::EvtNavigator> Frag;

    PackIndexAlg(const std::string &name);
    virtual ~PackIndexAlg();

    virtual bool initialize();
    virtual bool execute();
    virtual bool finalize();

private:
    long nextIndexEntry();
    Frag* index2Frag(long entry);

    GlobalBuffer<Frag> *m_gbuf;

    std::string m_index;
    std::vector<std::string> m_data;
    std::pair<double, double> m_window;

    std::ifstream    m_indexStream;
    NavInputStream*  m_dataStream;
    long m_total;
};

DECLARE_ALGORITHM(PackIndexAlg);

PackIndexAlg::PackIndexAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("IndexFile", m_index);
    declProp("DataFiles", m_data);
    declProp("TimeWindow", m_window);
}

PackIndexAlg::~PackIndexAlg() {}

bool PackIndexAlg::initialize()
{
    //获取存放fragment 的globalbuffer
    m_gbuf = GlobalStream<Frag>::GetBuffer("GFragStream");

    //open the index stream
    m_indexStream.open(m_index);

    //open the data stream
    m_dataStream = new NavInputStream(m_data);
    m_dataStream->initialize();
    bool stat = m_dataStream->first(false);  //otherwise the stream can not work properly
    m_total = m_dataStream->getEntries();

    LogDebug << "total events number: " << m_total << std::endl;

    return stat;
}

bool PackIndexAlg::execute()
{
    //read index
    long indexEntry = nextIndexEntry();

    //TODO: index selection ...
    //if (!isGood(indexEntry))
    //{
    //    return true;
    //}

    //read events around the index event and generate a Fragment
    auto frag = index2Frag(indexEntry);

    //put the Fragment to GlobalBuffer
    m_gbuf->push_back(std::shared_ptr<Frag>(frag));

    return true;
}

long PackIndexAlg::nextIndexEntry()
{
    long entry;
    m_indexStream >> entry;
    if ( ! m_indexStream.good() ) {
        m_par->stop();
    }
    LogDebug << "reading event " << entry << std::endl;
    return entry;
}

PackIndexAlg::Frag* PackIndexAlg::index2Frag(long entry)
{
    auto dest = new Frag;

    m_dataStream->setEntry(entry);
    JM::EvtNavigator* cur = m_dataStream->get();

    auto evt = dynamic_cast<JM::OECEvent*>(cur->getHeader("/Event/OEC")->event("JM::OECEvent"));
    auto& t0 = evt->getTime();

    LogDebug << t0 << std::endl;

    dest->evtDeque.push_back(std::shared_ptr<JM::EvtNavigator>(cur));
    dest->lbegin = 0;
    dest->lend = 1;

    long ientry = entry;
    while (--ientry >= 0)
    {
        m_dataStream->prev();
        auto p = m_dataStream->get();
        auto evt = dynamic_cast<JM::OECEvent *>(p->getHeader("/Event/OEC")->event("JM::OECEvent"));
        auto t = evt->getTime();
        t.Add(TTimeStamp(-t0.GetSec(), -t0.GetNanoSec()));
        if (t.AsDouble() < m_window.first)
        {
            delete p;
            break;
        }
        dest->evtDeque.push_front(std::shared_ptr<JM::EvtNavigator>(p));
        ++(dest->lbegin);
        ++(dest->lend);
    }

    ientry = entry;
    while (++ientry < m_total)
    {
        m_dataStream->next();
        auto p = m_dataStream->get();
        auto evt = dynamic_cast<JM::OECEvent *>(p->getHeader("/Event/OEC")->event("JM::OECEvent"));
        auto t = evt->getTime();
        t.Add(TTimeStamp(-t0.GetSec(), -t0.GetNanoSec()));
        if (t.AsDouble() >= m_window.second)
        {
            delete p;
            break;
        }
        dest->evtDeque.push_back(std::shared_ptr<JM::EvtNavigator>(p));
    }

    return dest;
}

bool PackIndexAlg::finalize()
{
    m_indexStream.close();
    delete m_dataStream;

    if (m_gbuf->status()) {
        m_gbuf->push_back(std::shared_ptr<Frag>(nullptr));
    }

    return true;
}