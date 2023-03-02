## "Distributed Asynchronous Distance Vector Routing" (using Bellman-Ford Shortest-Path Algorithm)

### NOTE: Assignment taken from online course material for "Computer Networking: A Top-Down Approach, 8th Edition" by Jim Kurose

The routines you will write. For the basic part of the assignment, you are to write the following routines which will ``execute'' asynchronously within the emulated environment that we have written for this assignment.

For node 0, you will write the routines:

    rtinit0() This routine will be called once at the beginning of the emulation. rtinit0() has no arguments. It should initialize the distance table (see below) in node 0 to reflect the direct costs of 1, 3, and 7 to nodes 1, 2, and 3, respectively. In the figure above, all links are bi-directional and the costs in both directions are identical. After initializing the distance table, and any other data structures needed by your node 0 routines, it should then send its directly-connected neighbors (in this case, 1, 2 and 3) the cost of it minimum cost paths, i.e., its distance vector, to all other network nodes. This minimum cost information is sent to neighboring nodes in a routing packet by calling the routine tolayer2(), as described below. The format of the routing packet is also described below.
    rtupdate0(struct rtpkt *rcvdpkt). This routine will be called when node 0 receives a routing packet that was sent to it by one if its directly connected neighbors. The parameter *rcvdpkt is a pointer to the packet that was received.

rtupdate0() is the "heart" of the distance vector algorithm. The values it receives in a routing packet from some other node j contain j's current shortest path costs (j's distance vector) to all other network nodes. rtupdate0() uses these received values to update its own "distance table." The distance table inside each node is the principal data structure used by the distance vector algorithm. You will find it convenient to declare the distance table as a 4-by-4 array of int's, where entry [i,j] in the distance table in node 0 is node 0's currently computed cost to node i via direct neighbor j; node 0 (re)computes its distable table entries for column j after receiving j's distance vector. If 0 is not directly connected to j, you can ignore this entry. We will use the convention that the integer value 999 is ``infinity.''

As specified by the distance vector algorithm we studied in class and in the text, if node 0's own minimum cost to another node changes as a result of the update, node 0 informs its directly connected neighbors of this change in minimum cost by sending them a routing packet. Recall that in the distance vector algorithm, only directly connected nodes will exchange routing packets. Thus nodes 1 and 2 will communicate with each other, but nodes 1 and 3 will node communicate with each other.

Similar routines are defined for nodes 1, 2 and 3. Thus, you will write 8 procedures in all: rtinit0(), rtinit1(), rtinit2(), rtinit3(),rtupdate0(), rtupdate1(), rtupdate2(), rtupdate3() 

basic 4-router implementation using Bellman-Ford algorithm; network-control layer routing;
