#!/usr/bin/env python

def HelloJob():
    # configuration is just the same as a single thread job
    import Sniper
    global ith
    ith += 1
    task = Sniper.Task(str(ith) +"-Job")

    Sniper.loadDll("libCoAnalysis.so")
    # bufMgr = task.createSvc("MtBufferMemMgr")
    # # bufMgr.property("TimeWindow").set([-0.1, 0.1])
    # bufMgr.property("TimeWindow").set([0.0, 0.0])

    # iSvc = task.createSvc("ThrdInputSvc/InputSvc")
    x = task.createAlg("DummyAnalyAlg")

    global first_time
    if first_time:
        task.show()
        first_time = False
    # instead of task.run(), we must return the task object here
    return task


def GInput():
    import Sniper
    task = Sniper.Task("GInput")
    task.setLogLevel(0)
    # import BufferMemMgr
    # bufMgr = task.createSvc("BufferMemMgr")
    # bufMgr.property("TimeWindow").set([-0.1, 0.1]);
 
    # import RootIOSvc
    # riSvc = task.createSvc("RootInputSvc/InputSvc")
    # riSvc.property("InputFile").set(["/home/yixiang/ACAT_project/mt.sniper/0.5M_OEC.root"])

    task.createAlg("PackFragAlg")

    return task

def GOutput():
    import Sniper
    task = Sniper.Task("GOutput")
    #wa = task.createAlg("WriteGBufAlg")
    #wa.property("DataFile").set("data.file")

    return task

if __name__ == "__main__":

    import ROOT
    ROOT.EnableThreadSafety()

    import Sniper
    Sniper.setLogLevel(0)
    #Sniper.setShowTime()
    #Sniper.setLogFile("log.txt", False)

    import SniperMuster
    muster = SniperMuster.Muster()

    Sniper.loadDll("libCoAnalysis.so")

    global ith
    ith = 0
    global first_time
    first_time = True

    # the EvtMax in Sniper.Task is deactivated by Muster
    muster.setEvtMax(10)

    # we will execute the HelloJob maximumly in 4 threads
    muster.config(HelloJob, 1)

    # I/O and global buffer
    gs = SniperMuster.createGlobalStream("FragmentStream/GFragStream")
    gs.configInput(GInput)
    gs.configOutput(GOutput)
    gs.configBuffer(500, 20)

    # TODO: show the configurations
    #muster.show()

    # spawn the threads and begin to run 
    muster.run()
