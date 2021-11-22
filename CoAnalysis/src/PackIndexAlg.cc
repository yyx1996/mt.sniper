#include "SniperKernel/AlgBase.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperMuster/Fragment.h"
#include "SniperMuster/GlobalStream.h"
#include "EvtNavigator/EvtNavigator.h"
#include "RootIOSvc/NavInputStream.h"
#include "Event/OECHeader.h"
#include <fstream>
#include <memory>

class PackIndexAlg : public AlgBase
{
public:
    typedef Fragment<JM::EvtNavigator> Frag;
    typedef std::shared_ptr<Frag> FragPtr;

    PackIndexAlg(const std::string &name);
    virtual ~PackIndexAlg();

    virtual bool initialize();
    virtual bool execute();
    virtual bool finalize();

private:
    long nextIndexEntry();
    FragPtr index2Frag(long entry);
    bool inTimeWindow(JM::EvtNavigator *p, const TTimeStamp &t0);

    long m_lastEntry;  //记录前一个fragment读到了哪个Entry,避免evt被重复读入
    FragPtr m_lastFrag;  //存放前一次放入globalbuffer的fragment
    
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
    : AlgBase(name),
      m_lastEntry(-1)
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
    m_lastFrag = index2Frag(indexEntry);
    //dump the fragment for debug
    if (logLevel() <= 2)
    {
        using SniperLog::LogStream;
        int i = 0;
        SniperLog::Logger::lock();
        *LogStream << "\t[" << std::endl;
        for (auto& nav : m_lastFrag->evtDeque)
        {
            *LogStream << "\t\t" << dynamic_cast<JM::OECEvent*>(nav->getHeader("/Event/OEC")->event("JM::OECEvent"))->getTime();
            if (i++ == m_lastFrag->lbegin) {
                *LogStream << "\t" << "(index)";
            }
            *LogStream << std::endl;
        }
        *LogStream << "\t]" << std::endl;
        SniperLog::Logger::unlock();
    }

    //put the Fragment to GlobalBuffer
    m_gbuf->push_back(m_lastFrag);

    return true;
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

long PackIndexAlg::nextIndexEntry()
{
    long entry;
    m_indexStream >> entry;
    if ( ! m_indexStream.good() ) {
        m_par->stop();
    }
    return entry;
}

PackIndexAlg::FragPtr PackIndexAlg::index2Frag(long entry)
{
    PackIndexAlg::FragPtr dest(new Frag);

    std::shared_ptr<JM::EvtNavigator> cur;
    if (entry > m_lastEntry)
    {
        m_dataStream->setEntry(entry);
        cur.reset(m_dataStream->get());
    }
    else
    {
        auto _it = m_lastFrag->evtDeque.rbegin() + (m_lastEntry - entry); //要取的evt在lastFrag的位置
        cur = *_it;
    }
    dest->evtDeque.push_back(cur);

    auto evt = dynamic_cast<JM::OECEvent*>(cur->getHeader("/Event/OEC")->event("JM::OECEvent"));
    auto& t0 = evt->getTime();

    long ientry = entry;
    while (--ientry >= 0)
    {
        if (ientry > m_lastEntry)
        { //要读的事例在curEntry之后，说明不曾被放入过fragment
            m_dataStream->prev();
            auto p = m_dataStream->get();
            if (!inTimeWindow(p, t0))
            {
                delete p;
                break;
            }
            dest->evtDeque.push_front(std::shared_ptr<JM::EvtNavigator>(p));
        }
        else
        { //读入的事例曾被放入过fragment
            auto _it = m_lastFrag->evtDeque.rbegin() + (m_lastEntry - ientry); //要取的evt在lastFrag的位置
            if (_it >= m_lastFrag->evtDeque.rend() || !inTimeWindow(_it->get(), t0))
            {
                break; //说明向前读的事例已经超出了上一个frag的第0个，或已不在时间窗口内
            }
            dest->evtDeque.push_front(*_it);
        }
    }
    dest->lend = dest->evtDeque.size();
    dest->lbegin = dest->lend - 1;

    ientry = entry;
    while (++ientry < m_total)
    {
        if (ientry > m_lastEntry)
        { //要读的事例在lastEntry之后,说明不曾被放入过frag
            m_dataStream->next();
            auto p = m_dataStream->get();
            if (!inTimeWindow(p, t0))
            {
                delete p;
                m_lastEntry = ientry - 1; //本次pack结束后更新m_lastEntry
                break;
            }
            dest->evtDeque.push_back(std::shared_ptr<JM::EvtNavigator>(p));
        }
        else
        { //要读的事例在lastEntry之前，需要从lastFrag取
            auto _it = m_lastFrag->evtDeque.rbegin() + (m_lastEntry - ientry);
            if (!inTimeWindow(_it->get(), t0))
            {
                break;
            }
            dest->evtDeque.push_back(*_it);
        }
    }

    return dest;
}

bool PackIndexAlg::inTimeWindow(JM::EvtNavigator *p, const TTimeStamp &t0)
{
    auto evt = dynamic_cast<JM::OECEvent *>(p->getHeader("/Event/OEC")->event("JM::OECEvent"));
    auto t = evt->getTime();
    t.Add(TTimeStamp(-t0.GetSec(), -t0.GetNanoSec()));
    return (m_window.first <= t.AsDouble() && t.AsDouble() < m_window.second);
}
