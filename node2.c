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
  int costmat[4][4];
} dt2;


/* 
  students to write the following two routines, and maybe some others
*/

void rtinit2() 
{
  printf("rtinit2 invoked at current time: %f\n", clocktime);

  // Intialize Distance Table for router2
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      dt2.costmat[i][j] = 999;
    }
  }
  // Update cost-paths based on given Network Topology (for router2, i.e. dt2)
  dt2.costmat[0][0] = 3;
  dt2.costmat[1][0] = 3;
  dt2.costmat[1][1] = 1;
  dt2.costmat[2][0] = 1;
  dt2.costmat[2][2] = 0;
  dt2.costmat[3][0] = 2;
  dt2.costmat[3][3] = 2;
  // Print router2s init Distance Table
  printdt2(&dt2);
  // Construct DV pkts to send to each of router2s neighbors
  struct rtpkt pkt0, pkt1, pkt3;
  pkt0.sourceid = 2;
  pkt0.destid = 0;
  pkt1.sourceid = 2;
  pkt1.destid = 1;
  pkt3.sourceid = 2;
  pkt3.destid = 3;

  for(int i = 0; i < 4; i++) {
    pkt0.mincost[i] = dt2.costmat[i][2];
    pkt1.mincost[i] = dt2.costmat[i][2];
    pkt3.mincost[i] = dt2.costmat[i][2];
  }
  // Send DV pkts to each of router2s neighbors (via Link Layer)
  tolayer2(pkt0);
  tolayer2(pkt1);
  tolayer2(pkt3);
}
/* */

void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
  printf("rtupdate2 invoked at current time: %f\n", clocktime);

  // Scope vars
  int j = rcvdpkt->sourceid;      // sourceid denotes the (distance) column vector which may need updating
  int updateFlag = 0;             // "Flag bit" -- if flipped to 1 (on), then changes need to be made (to DistVecs)

  // Traverse costmat, comparing least-cost paths ONLY for column[j]...
  // ...where this column reprs the path costs as they pertain to router2s distance table
  for(int i = 0; i < 4; i++) {
    // Check costmat for each row(*router*) i, against costmat in column[j] -- i.e. router2s DistVec
    if(dt2.costmat[i][j] > dt2.costmat[j][2] + rcvdpkt->mincost[i]) {
      // new Least-Cost Path found!
      dt2.costmat[i][j] = dt2.costmat[j][2] + rcvdpkt->mincost[i];

      // Before flipping on updateFlag, ensure that the row/router[i] that needs changing is NOT i=2
      // ...i=2 would imply that the changes are being made in router2s own DistVec
      if(i != 2) {
        // if i is NOT 2, turn on updateFlag -- denoting changes must be made
        updateFlag = 1;
      }
    }
  }
  // Check condition where DistVec updates are required
  if(updateFlag == 1) {
    // Print statement for debugging (denoting when and to which router changes are occurring)
    printf("\nDistance Table at router2 updating...\n");
    // Print distance table for router2 PRIOR to updates
    printdt2(&dt2);
    // Setup sourceid/destid's for pkts to each router where changes are needed (pkt0,pkt1,pkt3)
    struct rtpkt pkt0, pkt1, pkt3;
    pkt0.sourceid = 2;
    pkt0.destid = 0;
    pkt1.sourceid = 2;
    pkt1.destid = 1;
    pkt3.sourceid = 2;
    pkt3.destid = 3;
    // For each router, update mincost (array) for least-cost path to/from router2
    pkt0.mincost[2] = 0;
    pkt1.mincost[2] = 0;
    pkt3.mincost[2] = 0;

    for(int i = 0; i < 4; i++) {
      if(i == 2) {
        continue;
      }
      // Find minimum of each row (by way of columns) to determine new least-cost path to be sent to...
      // ...and update each pkts DistVec
      pkt0.mincost[i] = min(dt2.costmat[i][0], dt2.costmat[i][1], dt2.costmat[i][3]);
      pkt1.mincost[i] = min(dt2.costmat[i][0], dt2.costmat[i][1], dt2.costmat[i][3]);
      pkt3.mincost[i] = min(dt2.costmat[i][0], dt2.costmat[i][1], dt2.costmat[i][3]);
    }
  }
  // Print notification for changes being made in router2's neighbors DistVec(s) -- Debugging purposes
  printf("\nSending routing pkts to neighbor routers(0,1,3) with updated MINCOST[]: \n");
  // Display the updated mincost vector...
  for(int i = 0; i < 4; i++) {
    printf("%d\t", pkt0.mincost[i]);
  }
  printf("\n");
  // Send each pkt's updated DistVec to each neighboring router (via Link Layer)
  tolayer2(pk0);
  tolayer2(pkt1);
  tolayer2(pkt3);
}
/* */

printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}







