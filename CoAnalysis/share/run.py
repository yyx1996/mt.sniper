timewindow = [-0.1, 0.1]

def CoAnalyJob():
    import Sniper
    global ith
    ith += 1
    task = Sniper.Task(str(ith) +"-Job")

    import BufMemMgr
    bufMgr = task.createSvc("BufMemMgr")
    bufMgr.property("TimeWindow").append(timewindow)

    import CoAnalysis
    task.createAlg("DummyAnalyAlg")
    task.createSvc("ThrdInputSvc/InputSvc")
    

    global first_time
    if first_time:
        task.show()
        first_time = False

    return task

def GInput():
    import Sniper
    task = Sniper.Task("GInput")
    task.createAlg("InputFragAlg")
    return task
def Goutput():
    import Sniper
    task = Sniper.Task("GOutput")

    return task

if __name__ == "__main__":

    import Sniper
    Sniper.setLogLevel(3)
    #Sniper.setShowTime()
    #Sniper.setLogFile("log.txt", False)

    import SniperMuster
    muster = SniperMuster.Muster()

    gs = SniperMuster.createGlobalStream("FragmenStream/GFragStream")
    gs.configInput(GInput)
    gs.configOutput(GOutput)
    gs.configBuffer(50,20)

    