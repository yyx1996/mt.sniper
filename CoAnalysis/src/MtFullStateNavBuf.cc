#include "MtFullStateNavBuf.h"
#include "RootIOSvc/IInputStream.h"

using JM::EvtNavigator;

MtFullStateNavBuf::MtFullStateNavBuf(double floor, double ceiling)
    : m_rEntry(-1),
      m_floor(floor),
      m_ceiling(ceiling),
      m_beyond(nullptr),
      m_iStream(nullptr),
      m_fragCache(nullptr)
{
}

MtFullStateNavBuf::~MtFullStateNavBuf()
{
}

void MtFullStateNavBuf::init(IInputStream* iStream)
{
    m_iStream = dynamic_cast<ReadFragBufStream*>(iStream);
    initFromNextFrag();
}

bool MtFullStateNavBuf::next()
{
    if ( m_iCur >= 0 && m_dBuf[m_iCur] == m_fragCache->evtDeque[m_fragCache->lend-1] ) {
        if ( ! initFromNextFrag() ) {
            return false;
        }
    }

    if ( ++m_iCur >= m_dBuf.size() ) {
        fillNext();
    }

    trimDated();
    fillFresh();

    return true;
}

bool MtFullStateNavBuf::adopt(JM::EvtNavigator* nav)
{
    m_dBuf.push_back(ElementPtr(nav));
    m_iCur = m_dBuf.size() - 1;
    trimDated();

    return true;
}

bool MtFullStateNavBuf::adopt(EventSmartPtr& nav)
{
    m_dBuf.push_back(nav);
    m_iCur = m_dBuf.size() - 1;
    trimDated();

    return true;
}

bool MtFullStateNavBuf::reset(int entry)
{
    if ( m_iStream != 0 ) {
        clear();
        m_iCur = -1;
        init(m_iStream);
    }
    else {
        m_rEntry = entry;
    }
    return true;
}

bool MtFullStateNavBuf::initFromNextFrag()
{
    m_dBuf.clear();
    m_fragCache = m_iStream->getNextFrag();

    bool stat = false;
    if ( m_fragCache != nullptr ) {
        m_rEntry = m_fragCache->lbegin;
        auto _begin = m_fragCache->evtDeque.begin();
        auto _lbegin = _begin + m_rEntry;
        m_dBuf.insert(m_dBuf.end(), _begin, _lbegin);
        m_iCur = m_rEntry - 1;
        m_beyond = *_lbegin;
        stat = true;
    }
    return stat;
}

void MtFullStateNavBuf::trimDated()
{
    JM::EvtNavigator* fevt = m_dBuf.front().get();
    while ( fevt != m_dBuf[m_iCur].get() ) {
        if ( m_floor < 0.0 && timeShift(fevt) >= m_floor ) {
            break;
        }
        m_dBuf.pop_front();
        --m_iCur;
        fevt = m_dBuf.front().get();
    }
}

void MtFullStateNavBuf::fillFresh()
{
    if ( m_ceiling > 0.0 ) {
        while ( m_beyond != nullptr && timeShift(m_beyond.get()) < m_ceiling ) {
            fillNext();
        }
    }
}

void MtFullStateNavBuf::fillNext()
{
    //we have to ensure m_beyond is valid before call this function
    m_dBuf.push_back(m_beyond);
    if ( m_rEntry+1 < m_fragCache->evtDeque.size() ) {
        m_beyond = m_fragCache->evtDeque[++m_rEntry];
    }
    else {
        m_beyond = nullptr;
    }
}

double MtFullStateNavBuf::timeShift(EvtNavigator* nav)
{
    const TTimeStamp& t0 = m_dBuf[m_iCur]->TimeStamp();

    TTimeStamp t = nav->TimeStamp();
    t.Add(TTimeStamp(-t0.GetSec(), -t0.GetNanoSec()));
    return t.AsDouble();
}
