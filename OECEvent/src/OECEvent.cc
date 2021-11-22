#include "Event/OECEvent.h"
#include <iostream>

ClassImp(JM::OECEvent);
namespace JM{
    OECEvent::OECEvent(){
        m_time=TTimeStamp(0,0);
        f_energy=-1;
        f_vertexX=0;
        f_vertexY=0;
        f_vertexZ=0;
        i_tag=0;
        f_muinx = 0;
        f_muiny = 0;
        f_muinz = 0;
        f_muoutx = 0;
        f_muouty = 0;
        f_muoutz = 0;
        f_muid = 0;
    }

    OECEvent::~OECEvent(){
    }

    std::string OECEvent::getIDA(){
        return "CD";
    }

    bool OECEvent::isMuon(){
        int i_temptag=unsigned(i_tag<<11)>>31;  
        if (i_temptag==1) return true;
        else return false;
    }

    bool OECEvent::isTTMuon(){
        int i_temptag=unsigned(i_tag<<1)>>31;
        if (i_temptag==1) return true;
        else return false;
    }

    bool OECEvent::isWPMuon(){
        int i_temptag=unsigned(i_tag<<2)>>31;
        if (i_temptag==1) return true;
        else return false;
    }   

    bool OECEvent::isMuonRelated(){
        int i_temptag=unsigned(i_tag<<9)>>31;
        if (i_temptag==1) return true;
        else return false;
    }

    bool OECEvent::addTag(std::string tag){
        int i_temptag=0;
        if (!tag.compare("muonCD")){
            i_temptag=0x10100000;     
        }
        else if (!tag.compare("muonRelated")){
            i_temptag=0x00400000;
        }
        else if (!tag.compare("pIBD")){
            i_temptag=0x10204000;
        }
        else if (!tag.compare("dIBD")){
            i_temptag=0x10200400;
        }
        else if (!tag.compare("singles_LE")){
            i_temptag=0x10200001;
        }
        else if (!tag.compare("singles_ME")){
            i_temptag=0x10200002;
        }
        else if (!tag.compare("singles_HE")){
            i_temptag=0x10200004;
        }
        else if (!tag.compare("singles_XE")){
            i_temptag=0x10200008;
        }
        else if (!tag.compare("Other")){
            i_temptag=0x10200800;
        }
        else if (!tag.compare("IBD")){
            i_temptag=0x10200400;
        }
        else if (!tag.compare("BiPo")){
            i_temptag=0x10200200;
        }
        else if (!tag.compare("ProtonDecay")){
            i_temptag=0x10200100;
        }
        else if (!tag.compare("SpallationNeutron")){
            i_temptag=0x10201000; //old: 0x10200010
        }
        else if (!tag.compare("michelElec")){
            i_temptag=0x10202000;
        }
        else if (!tag.compare("SpallationIsotope")){
            i_temptag=0x10203000;
        }
        else if (!tag.compare("muWP")){
            i_temptag=0x20100000;
        }
        else if (!tag.compare("muTT")){
            i_temptag=0x40100000;
        }
        else if (!tag.compare("muShower")){
            i_temptag=0x10103000;
        }
        else if (!tag.compare("muStop")){
            i_temptag=0x10104000;
        }
        else if (!tag.compare("muCDLE")){
            i_temptag=0x10102000;
        }
        else if (!tag.compare("muSglTrack")){
            i_temptag=0x10100100;
        } 
        else if (!tag.compare("muDblTrack")){
            i_temptag=0x10100200;
        }
        else if (!tag.compare("muMultiTrack")){
            i_temptag=0x10100300;
        }
        else if (!tag.compare("muShrTrack")){
            i_temptag=0x10103400;
        }
        else if (!tag.compare("muStpTrack")){
            i_temptag=0x10104800;
        }
        else if (!tag.compare("muNoTrack")){
            i_temptag=0x10100F00;
        }
        else {
            //LogError<<"unreconginised string!!!"<<endl;
            return false;
        }
        i_tag=i_tag|i_temptag;
        return true;
        //if (!strcmp(tag.c_str(),"singles_LE")) i_tag=1;
        //else if (!strcmp(tag.c_str(),"singles_ME")) i_tag=2;
        //else if (!strcmp(tag.c_str(),"singles_HE")) i_tag=3;
        //else if (!strcmp(tag.c_str(),"singles_XE")) i_tag=4;
        //return true;
    }
}
