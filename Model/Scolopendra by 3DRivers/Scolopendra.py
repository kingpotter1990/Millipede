import maya.cmds as cmds
dt = 0.03;
n_frame = 1000;
for i in range(0, n_frame)
    #parse one frame data
    #set to current time
    cmds.currentTime(dt*i)
    
    #select each joint and apply the translation rotation
    cmds.select("s10l2|pasted__pasted__joint24")
    cmds.rotate()