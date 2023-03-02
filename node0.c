#include <stdio.h>

struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;

struct distance_table 
{
  int costmatrix[4][4];
} dt0;

/* 
  students to write the following two routines, and maybe some others
*/

/* Initialize the lookups (into cost matrix) to properly correspond to the costs or distances to each node...
   ...and to create/allow connections from a given node to its Next-Hop Router*/
void rtinit0() {
  printf("\nrtinit0 invoked at current time: %f\n", clocktime);

  // initialize Router0's Distance Table
  for(int i=0; i<4; i++) {
    for(int j=0; j<4; j++) {
      dt0.costmatrix[i][j] = 999;       // where 999 nominally represents INFINITY
    }
  }
  // Update path-costs based on (initially-)given NETWORK TOPOLOGY
  dt0.costmatrix[0][0] = 0;
  dt0.costmatrix[1][0] = 1;
  dt0.costmatrix[1][1] = 1;
  dt0.costmatrix[2][0] = 3;
  dt0.costmatrix[2][2] = 3;
  dt0.costmatrix[3][0] = 7;
  dt0.costmatrix[3][3] = 7;
  // Print router0's initial Distance Table
  printdt0(&dt0);
  // construct DV pkts to send to each of router0's neighbors
  struct rtpkt pkt1, pkt2, pkt3;
  pkt1.sourceid = 0;
  pkt1.destid = 1;
  pkt2.sourceid = 0;
  pkt2.destid = 2;
  pkt3.sourceid = 0;
  pkt3.destid = 3;

  for(int i=0; i<4; i++) {
    pkt1.mincost[i] = dt0.costmatrix[i][0];
    pkt2.mincost[i] = dt0.costmatrix[i][0];
    pkt3.mincost[i] = dt0.costmatrix[i][0];
  }
  // Send DV pkts to router0 neighbors (via LINK LAYER)
  tolayer2(pkt1);
  tolayer2(pkt2);
  tolayer2(pkt3);
}
/* */

void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  printf("rtupdate0 invoked at current time: %f\n", clocktime);
  // Verify that received pkt has destid of dt0
  if(rcvdpkt->destid != 0) {
    return;
  }
  // rtupdate0 scope vars
  int j = rcvdpkt->sourceid;      // only concerned with column j traversal (for distvec updates)
  int updateFlag = 0;             // when set to 1, distance table updates are required

  // traverse column j to check for distance vector updates...
  for(int i=0; i<4; i++) {
    if(dt0.costmatrix[i][j] > dt0.costmatrix[j][0] + rcvdpkt->mincost[i]) {
      // dt0 vector needs to be updating...set to ^^^
      dt0.costmatrix[i][j] = dt0.costmatrix[j][0] + rcvdpkt->mincost[i];
      // NOTE: dt0...[j][0] represents router0's current least-cost path for each (other) router
      //        at the intersection of that router's least-cost([j]) and dt0's ([0])

      if(i != 0) {/* condition */
        // if i == 0, then this "update" would pertain to dt0's own DV (which it already has the state of)
        updateFlag = 1;       // flag=1, distance vector(s) for neighbors nees updating
      }
    }
  }
  // Assuming flag=1...
  if(updateFlag == 1) {
    printf("\nDistance Table at Router0 updating...\n");
    printdt0(&dt0);
    // Fill/verify sourceid/destid's for routers which will be updated
    // NOTE: Per the DVR protocol, these updates are only to be sent to router0's DIRECT neighbors
    // Router0 Direct Neighbors: router1, router2, and router3
    struct rtpkt pk1, pkt2, pkt3;
    pkt1.sourceid = 0;
    pkt1.destid = 1;
    pkt2.sourceid = 0;
    pkt2.destid = 2;
    pkt3.sourceid = 0;
    pkt3.destid = 3;
    // Update each pkt's mincost array/vector at [0]...
    // NOTE: Each router's distance table (dt0, dt1, etc.) contains a mincost array of size [4]
    //       ...this 4 element array holds the least-cost path for each router
    pkt1.mincost[0] = 0;
    pkt2.mincost[0] = 0;
    pkt3.mincost[0] = 0;

    for(int i=1; i < 4; i++) {
      // find/update the MINIMUM of each pkt's mincost[] array to further determine least-cost path
      // ...this MIN is found by checking each router's distance vector at ROW[i]...
      // ...AND against each possible column (in the matrix entries) (i.e. cols[1],[2],[3])
      //  NOTE: That the columns which are checked against (with corresponding row[i]) do NOT include i=0
      pkt1.mincost[i] = min(dt0.costmatrix[i][1], dt0.costmatrix[i][2], dt0.costmatrix[i][3]);
      pkt2.mincost[i] = min(dt0.costmatrix[i][1], dt0.costmatrix[i][2], dt0.costmatrix[i][3]);
      pkt3.mincost[i] = min(dt0.costmatrix[i][1], dt0.costmatrix[i][2], dt0.costmatrix[i][3]);
    }
    // print updated DistVec(s) -- changes or otherwise
    printf("\nSending pkts to Routers(1,2,3) with MINCOST[]...\n");
    for(int i=0; i<4; i++) {
      printf("%d\t", pkt1.mincost[i]);    // only need to send one copy of mincost[]; repeating for pkt2/pkt3 is redundant
    }
    printf("\n");
    // Send DV pkts to NEIGHBORS (of router0) via Link Layer
    tolayer2(pkt1);
    tolayer2(pkt2);
    tolayer3(pkt3);
  }
}
/* */

printdt0(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
}

linkhandler0(linkid, newcost)   
  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
}

