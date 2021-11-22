
# BELOW is a hack, because the BookEDM.h can't be found in the install area
message("----> $ENV{EDMUTILROOT}/include")
include_directories($ENV{EDMUTILROOT}/include)

EDM(OECEvent
        src/OECEventBook.cc
    NOXOD
    FILES
        OECEvent
        OECHeader
    )

