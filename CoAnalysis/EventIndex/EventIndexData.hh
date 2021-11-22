#ifndef EventIndexData_hh
#define EventIndexData_hh

#include "EventIndex/EventIndexBase.hh"
#include <fstream>
/*
 * A streamer is implemented as an independent class.
 */

struct EventIndexDataV0 {
    uint64_t fileId;
    uint64_t entryId;

    float energy;
    float vertex_x;
    float vertex_y;
    float vertex_z;
    double time;
};

struct EventIndexDataV0PlainInputStreamer {
    typedef EventIndexDataV0 data_type;
    typedef EventIndexWrapper<data_type> wrapper_type;

    bool open(const std::string& filename);
    bool close();
    bool get(wrapper_type& data);

private:
    std::ifstream m_input;
};

struct EventIndexDataV0PlainOutputStreamer {
    typedef EventIndexDataV0 data_type;
    typedef EventIndexWrapper<data_type> wrapper_type;

    bool open(const std::string& filename);
    bool close();
    bool put(const wrapper_type& data);

private:
    std::ofstream m_output;
};

#endif
