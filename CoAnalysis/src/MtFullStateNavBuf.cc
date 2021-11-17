#include "MtFullStateNavBuf.h"
#include "RootIOSvc/IInputStream.h"

using JM::EvtNavigator;

MtFullStateNavBuf::MtFullStateNavBuf(double floor, double ceiling)
    : m_rEntry(-1),
      m_floor(floor),
      m_ceiling(ceiling),
      m_beyond(nullptr),
      m_iStream(0)
{
}

MtFullStateNavBuf::~MtFullStateNavBuf()
{
}

void MtFullStateNavBuf::init(IInputStream* iStream)
{
    bool first = m_iStream == 0;

    m_iStream = dynamic_cast<ReadFragBufStream*>(iStream);

    bool stat = true;
    if ( first || m_rEntry <= 0 ) {
        stat = m_iStream->first(m_rEntry<=0);
    }
    if ( m_rEntry > 0 ) {
        stat = m_iStream->setEntry(m_rEntry);
        m_rEntry = -1;
    }

    if ( stat ) {
        m_beyond = m_iStream->getRef();
        if ( m_beyond != nullptr ) {
            fillNext();
        }
    }
}

bool MtFullStateNavBuf::next()
{
    if ( ++m_iCur >= m_dBuf.size() ) {
        if ( m_beyond == nullptr ) {
            return false;
        }
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
    m_beyond = m_iStream->next() ? m_iStream->getRef() : nullptr;
}

double MtFullStateNavBuf::timeShift(EvtNavigator* nav)
{
    const TTimeStamp& t0 = m_dBuf[m_iCur]->TimeStamp();

    TTimeStamp t = nav->TimeStamp();
    t.Add(TTimeStamp(-t0.GetSec(), -t0.GetNanoSec()));
    return t.AsDouble();
}
