#ifndef EventIndex_hh
#define EventIndex_hh

#include "EventIndex/EventIndexBase.hh"
#include "EventIndex/EventIndexReader.hh"
#include "EventIndex/EventIndexWriter.hh"
#include "EventIndex/EventIndexData.hh"

typedef EventIndexWrapper<EventIndexDataV0> AEIv0;
typedef EventIndexReader<EventIndexDataV0PlainInputStreamer> AEIv0PlainReader;
typedef EventIndexWriter<EventIndexDataV0PlainOutputStreamer> AEIv0PlainWriter;

typedef AEIv0 AEI;
typedef AEIv0PlainReader AEIPlainReader;
typedef AEIv0PlainWriter AEIPlainWriter;


#endif
