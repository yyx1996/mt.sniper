#ifndef MT_FULL_STATE_NAV_BUF_H
#define MT_FULL_STATE_NAV_BUF_H

#include "EvtNavigator/NavBuffer.h"
#include "ReadFragBufStream.h"
#include <memory>

class IInputStream;

class MtFullStateNavBuf : public JM::NavBuffer
{
    public :

        typedef std::shared_ptr<JM::EvtNavigator> EventSmartPtr;

        MtFullStateNavBuf(double lowBound, double highBound);

        virtual ~MtFullStateNavBuf();

        void init(IInputStream* iStream);

        bool next();

        bool adopt(JM::EvtNavigator* nav);
        bool adopt(EventSmartPtr& nav);

        bool reset(int entry);

    private :

        void trimDated();
        void fillFresh();
        void fillNext();
        double timeShift(JM::EvtNavigator* nav);

        int                m_rEntry;
        const double       m_floor;
        const double       m_ceiling;

        EventSmartPtr      m_beyond;
        ReadFragBufStream* m_iStream;
};

#endif
