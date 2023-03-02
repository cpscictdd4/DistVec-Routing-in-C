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

int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table 
{
  int costmat[4][4];
} dt1;

/* 
  students to write the following two routines, and maybe some others
*/

void rtinit1() 
{
  printf("\nrtinit0 invoked at current time: %f\n", clocktime);

  // Initialize Router1's Distance Table
  for(int i=0; i<4; i++) {
    for(int j=0; j<4; j++) {
      dt1.costmat[i][j] = 999;
    }
  }
  // Update path-costs based on given Network Topology
  dt1.costmat[0][0] = 1;
  dt1.costmat[0][1] = 1;
  dt1.costmat[1][1] = 0;  // dist from router1 to router1 = 0
  dt1.costmat[2][1] = 1;
  dt1.costmat[2][2] = 1;
  // Print router1s init Distance Table
  printdt1(&dt1);
  // Construct DV pkts to send to each router1s neighbors
  struct rtpkt pkt0, pkt2;    // one pkt per router1 neighbor
  pkt0.sourceid = 1;
  pkt0.destid = 0;
  pkt2.sourceid = 1;
  pkt2.destid = 2;

  for(int i=0; i<4; i++) {
    pkt0.mincost[i] = dt1.costmat[i][1];
    pkt2.mincost[i] = dt1.costmat[i][1];
  }
  // Send DV pkts to router1 neighbors (via Link Layer)
  tolayer2(pkt0);
  tolayer2(pkt2);
}
/* */

rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  printf("\nrtupdate1 invoked at current time: %f\n", clocktime);
  // First, ensure the destid of rcvdpkt is 1 (otherwise, sent to wrong addr)
  if(rcvdpkt->destid != 1) {
    return;       // early return
  }
  int j = rcvdpkt->sourceid;  // sourceid denotes the column vec of dt1 that may need updating
  int updateFlag = 0;

  printf("\nPacket rcvd at router1 from router%d\n", j);

  // Traverse each column against j and check for possible updates
  for(int i=0; i<4; i++) {
    if(dt1.costmat[i][j] > dt1.costmat[j][1] + rcvdpkt.mincost[i]) {
      dt1.costmat[i][j] = dt1.costmat[j][1] + rcvdpkt.mincost[i];

      if(i != 1) {
        updateFlag = 1;
      }
    }
  }
  // Check for, then update router distvec's in the case of updateFlag being set to 1
  if(updateFlag == 1) {       // Detected a change in dt1
    printf("\nDistance Table at router1 updating...\n");
    // Use (given) printdt1 function below to display router1's Distance Table
    printdt1(&dt1);
    // configure sourceid/destid for sending updated distance vectors
    // ...these updates sent to ONLY router1's direct neighbors
    // Router1 Direct Neighbors: router0 and router2
    struct rtpkt pkt0, pkt2;
    pkt0.sourceid = 1;      // router1 is sending this update
    pkt0.destid = 0;
    pkt2.sourceid = 1;
    pkt2.destid = 2
    // update each pkt's distance vector (mincost array)
    pkt0.mincost[1] = 0;
    pkt2.mincost[1] = 0;
    // Reclaculate minimum of least-cost paths for router1's neighbors
    for(int i=0; i<4; i++) {
      if(i == 1) {        // when i=1, we are looking at router1's mincost. skip
        continue;
      }
      // For each neighbor (0 and 2), find the MIN from within dt1's costmat at each row[i]...
      // ...and per column - where the columns represent the costs from router1's distvec for routers0,2,3
      pkt0.mincost[i] = min(dt1.costmat[i][0], dt1.costmat[i][2], dt1.costmat[i][3]);
      pkt2.mincost[i] = min(dt1.costmat[i][0], dt1.costmat[i][2], dt1.costmat[i][3]);
    }
    // Note that the above expressions check/compare each of the 4 elements of each pkt's mincost array...
    // ...and for each i (of these 4 elements), finds the minimum of/from router1's Distance Vector for
    //    each matrix entry at row[i], and column[j] -> where j represents these costmat values for each
    //    of the other routers in network (namely, router0, router2, router3
    printf("\nSending pkts to Routers(0,2) with MINCOST[]...\n");
    for(int i=0; i<4; i++) {
      printf("%d\t", pkt0.mincost[i]);
    }
    printf("\n");
    // Sending DV pkts to neighbors
    tolayer2(pkt0);
    tolayer2(pkt2);
  }
} 
/* */

printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



linkhandler1(linkid, newcost)   
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
}


