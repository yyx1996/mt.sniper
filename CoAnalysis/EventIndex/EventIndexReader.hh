#ifndef EventIndexReader_hh
#define EventIndexReader_hh

#include "EventIndex/EventIndexBase.hh"
#include <string>

template<typename S>
struct EventIndexReader {
    typedef S streamer_type;
    typedef typename streamer_type::data_type data_type;
    typedef typename streamer_type::wrapper_type wrapper_type;

    bool open(const std::string& filename) {
        return m_streamer.open(filename);
    }

    bool close() {
        return m_streamer.close();
    }

    bool get(wrapper_type& data) {
        return m_streamer.get(data);
    }

private:
    streamer_type m_streamer;
};

#endif
