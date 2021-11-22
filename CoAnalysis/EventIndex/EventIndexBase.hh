#ifndef EventIndexBase_hh
#define EventIndexBase_hh

#include <cstdint>

/* 
 * Note about the design:
 *
 * Type T should implement the real data, including fileId, entryId and so on. 
 * In this structure, the T don't need to be derived from EventIndexBase.
 *
 * As type T is very simple, it is easy to use different streamers to save/load the data.
 *
 * The EventIndexBase is used by the framework, so it don't need to know 
 * the real implmentation.
 *
 */

template<typename T>
class EventIndexWrapper: public T {
public:
    typedef EventIndexWrapper<T> self_type;
    typedef T data_type;

    const uint64_t& getFileId() const {
        return data_type::fileId;
    }
    const uint64_t& getEntryId() const {
        return data_type::entryId;
    }

};

#endif

