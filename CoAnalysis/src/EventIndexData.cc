#include "EventIndex/EventIndexData.hh"
#include <sstream>
#include <iostream>

// INPUT STREAMER
bool EventIndexDataV0PlainInputStreamer::open(const std::string& filename) {
    m_input.open(filename);
    return m_input.good();
}

bool EventIndexDataV0PlainInputStreamer::close() {
    m_input.close();
    return true;
}

bool EventIndexDataV0PlainInputStreamer::get(EventIndexDataV0PlainInputStreamer::wrapper_type& data) {
    bool st = false;
    wrapper_type tmp;
    std::string cache_line;

    while(std::getline(m_input, cache_line)) {
        std::stringstream ss(cache_line);

        ss >> tmp.fileId
           >> tmp.entryId
           >> tmp.energy
           >> tmp.vertex_x
           >> tmp.vertex_y
           >> tmp.vertex_z
           >> tmp.time;

        if (not ss.fail()) {
            st = true;
            break;
        }
    }

    if (st) {
        data = tmp;
    }

    return st;
}

// OUTPUT STREAMER
bool EventIndexDataV0PlainOutputStreamer::open(const std::string& filename) {
    m_output.open(filename);
    return m_output.good();
}

bool EventIndexDataV0PlainOutputStreamer::close() {
    m_output.close();
    return true;
}

bool EventIndexDataV0PlainOutputStreamer::put(const EventIndexDataV0PlainOutputStreamer::wrapper_type& data) {

    m_output << data.fileId   << " "
             << data.entryId  << " "
             << data.energy   << " "
             << data.vertex_x << " "
             << data.vertex_y << " "
             << data.vertex_z << " "
             << data.time     << ""
             << std::endl;;

    return m_output.good();
}
