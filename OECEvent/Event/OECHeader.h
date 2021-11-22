#ifndef OECHEADER_H
#define OECHEADER_H

#include "Event/HeaderObject.h"
#include "EDMUtil/SmartRef.h"
#include "OECEvent.h"

namespace JM{
    class OECHeader:public HeaderObject{
        private:
            JM::SmartRef m_event; // ||
            uint32_t     m_l1id;

        public:
            OECHeader();
            ~OECHeader();

            uint32_t l1id() { return m_l1id; }
            void setL1id(uint32_t l1id) { m_l1id = l1id; }

            //virtual function
            EventObject* event() {
                return m_event.GetObject();
            }
            EventObject *event(const std::string &value);
            void setEventEntry(const std::string &eventName, Long64_t &value);
            
            void setEvent(OECEvent* value){m_event=value;}


        public:
            ClassDef(OECHeader,2)
    };
}

#endif
