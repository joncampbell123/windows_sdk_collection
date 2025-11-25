TAPI ACD Samples
=======================================================================
ACDSMPL and ACDCLNT implement a simple ACD call center.  The purpose of
these samples is to highlight some of the new call center features of 
TAPI 2.0.  This document will give an overview of some of the design 
details of these two applications.

ACDSMPL
=======
ACDSMPL is the ACD server application.  It keeps track of groups and 
agents, When calls come in on a group’s line, ACDSMPL looks for an 
agent to transfer the call to.  It also keeps track of agent state information.

Groups
------
In the sample, a group corresponds to a single address on a line 
device.  When a call arrives on that address, the ACD server will look 
for an agent that is a member of that group to take the call.  A group 
has a list of agents that are allowed to log into it.

Agents
------
An agent corresponds to a person that has access to a single line. They 
are allowed to use any address on that line. This implementation is 
limited, however, because it only has one destination address for each 
agent.  That is, when a call is to be transferred to the agent, there 
is a single number that it will be transferred to, not a number for 
each address.  Most implementations should allow for a destination 
address for each address.

UI
--
The UI allows adding of agents and groups.  ACDSMPL automatically 
writes out the agent and group information to ACDSMPL.INI when it shuts 
down.  The File | Open command will read in the information from that 
file.  See below for a description of the ini file.

The default UI view is the Group view.  The Group view shows each 
group, and underneath them, every agent that is allowed to log into the 
group.  If the agent is logged in, their user name will be in bold.  
The Agent view shows each agent.  This view can be used to change 
information about the Agent.

Security
--------
There is no security implemented in this sample.  Users are verified by 
the user name that TAPI puts in each LINEPROXYREQUEST structure and the 
line device.  ACDSMPL compares these with the information entered in 
through the ACDSMPL UI.

The new TAPI features that ACDSMPL illustrates are:

	New lineOpen functionality
	Handling of the LINE_PROXYREQUEST message
	New lineInitializeEx functionality.
	Unicode

ACDCLNT
=======
ACDCLNT is the client application that works the ACDSMPL.  Using the 
model from ACDSMPL, ACDCLNT assumes that the user can only have access 
to one line.  Upon startup, it calls lineGetAgentCaps for each device. 
When it finds a line device that lineAgentCaps succeeds on, it uses that 
line device.

ACDCLNT allows the user to set state, group and activity information.  
Also, when a call appears on its line, ACDCLNT will allow the user to 
answer that call.

The new TAPI features that ACDCLNT illustrates are:

	The new Agent functions:  lineGetAgentList, lineGetAgentStatus, 
	lineGetAgentCaps, lineGetAgentActivityList, lineSetAgentState, 
	lineSetAgentActivity, and lineSetAgentGroup.
	Unicode
	lineInitializeEx - using completion ports for callback mechanism


ACDSMPL.INI format
==================

[GENERAL] section
NumAgents=x
NumGroups=y
x and y are the number of agents and groups in this file

[GROUPS] section
GROUPx=name,perm device id,address id

x starts at 0 and increments for each group
name is the name of the group (cannot contain commas)
permanent device id is the permanent device id of the line device 
associated with the group.  
address id is the address id of the line that the group uses

[AGENTS] section
AGENTx=name,dest address,perm device id

x starts at 0 and increments for each group
name is the user name of the agent
dest address is the address that acdsmpl would transfer calls to that 
agent
permanent device id is the permanent device id if the user's line

[GROUPx] section
each group has a section that indicates which agents are members of that group.  If the agent is in the group, the entries look like
AGENTy=1 where AGENTy corresponds to the agent key in the [AGENTS] section


EXAMPLE:

[Groups]
GROUP0=First Group,786435,0
GROUP1=Second Group,786435,1
GROUP2=Third Group,786436,0

[GROUP0]
AGENT0=1
AGENT1=1

[GROUP1]
AGENT0=1

[Agents]
AGENT0=scooter,3343,786432
AGENT1=frankie,3342,786433

[General]
NumGroups=3
NumAgents=2
