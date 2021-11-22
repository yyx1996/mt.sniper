#ifndef OECEVENT_H
#define OECEVENT_H
#include "Event/EventObject.h"
#include <string>

#include "TTimeStamp.h"

namespace JM{
    class OECEvent:public EventObject{
        public:
            OECEvent();
            ~OECEvent();

            //get function
            std::string getIDA();//get detector
            std::string getIDB();//get top level category
            std::string getIDC();//get sub-level category
            float getEnergy(){return f_energy;}
            //float getTime(){return f_time;}
            const TTimeStamp &getTime() const{return m_time;}
            float getVertexX(){return f_vertexX;}
            float getVertexY(){return f_vertexY;}
            float getVertexZ(){return f_vertexZ;}
            int getTag(){return i_tag;}

            //set function
            void setEnergy(float e){f_energy=e;}
            //void setTime(float t){f_time=t;}
            void setTime(const TTimeStamp& value){m_time=value;}
            void setVertexX(float x){f_vertexX=x;}
            void setVertexY(float y){f_vertexY=y;}
            void setVertexZ(float z){f_vertexZ=z;}
            //void setTag(int tag){i_tag=tag;}
            bool addTag(std::string);
            bool addTag(int tag){i_tag=i_tag | tag;return true;}
            bool isMuon();
            bool isTTMuon();
            bool isWPMuon();
            bool isMuonRelated();
            //bool addpTag(string);
            //bool addtTag(string);
            void setMuID(int muid){f_muid=muid;}
            int getMuID(){return f_muid;}
            void setMuInX(float muinx){f_muinx=muinx;}
            void setMuInY(float muiny){f_muiny=muiny;}
            void setMuInZ(float muinz){f_muinz=muinz;}
            float getMuInX(){return f_muinx;}
            float getMuInY(){return f_muiny;}
            float getMuInZ(){return f_muiny;}
            void setMuOutX(float muoutx){f_muoutx=muoutx;}
            void setMuOutY(float muouty){f_muouty=muouty;}
            void setMuOutZ(float muoutz){f_muoutz=muoutz;}
            float getMuOutX(){return f_muoutx;}
            float getMuOutY(){return f_muouty;}
            float getMuOutZ(){return f_muouty;}

        private:
            float f_energy;//reconstructed energy of single event
            TTimeStamp m_time;
            int n_clusters;
            float f_vertexX;//reconstructed vertex of single event
            float f_vertexY;
            float f_vertexZ;
            int i_tag;//the tag of event //eg: CD_muonUncor_IBD
            float f_muinx;
            float f_muiny;
            float f_muinz;
            float f_muoutx;
            float f_muouty;
            float f_muoutz;
            int f_muid;

        public:
            ClassDef(OECEvent,2)
    };
}
#endif
