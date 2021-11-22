#ifndef EventIndexWriter_hh
#define EventIndexWriter_hh

#include "EventIndex/EventIndexBase.hh"
#include <string>


template<typename S>
struct EventIndexWriter {
    typedef S streamer_type;
    typedef typename streamer_type::data_type data_type;
    typedef typename streamer_type::wrapper_type wrapper_type;

    bool open(const std::string& filename) {
        return m_streamer.open(filename);
    }

    bool close() {
        return m_streamer.close();
    }

    bool put(const wrapper_type& data)  {
        return m_streamer.put(data);
    }

private:
    streamer_type m_streamer;
};


#endif
