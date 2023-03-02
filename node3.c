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
} dt3;

/* students to write the following two routines, and maybe some others */

void rtinit3() 
{
  printf("rtinit3 invoked at current time: %f\n", clocktime);

  // Initialize Distance Table for router3
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      dt3.costmat[i][j] = 999;
    }
  }
  // Update cost-paths based on given Network Topology (for router3)
  dt3.costmat[0][0] = 7;    // neighbor router0
  dt3.costmat[0][3] = 7;
  dt3.costmat[2][2] = 2;    // neighbor router2
  dt3.costmat[2][3] = 2;
  dt3.costmat[3][3] = 0;    // router3 (itself)

  // Display Distance Table for router3
  printdt3(&dt3);

  // Construct pkts for each of router3's neighbors (0 and 2)
  struct rtpkt pkt0, pkt2;
  pkt0.sourceid = 3;
  pkt0.destid = 0;
  pkt2.sourceid = 3;
  pkt2.destid = 2;

  for(int i = 0; i < 4; i++) {
    pkt0.mincost[i] = dt3.costmat[i][3];
    pkt2.mincost[i] = dt3.costmat[i][3];
  }
  // Send pkts to neighbors (via Link Layer)
  tolayer2(pkt0);
  tolayer2(pkt2);
}
/* */

void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
  printf("\nrtupdate3 invoked at current time: %f\n", clocktime);
  if(rcvdpkt->destid != 3) {
    return;       // incorrect distance table update
  }
  // Internal scope vars
  int j = rcvdpkt->sourceid;
  int updateFlag = 0;

  // Traverse costmat for ONLY column j(sourceid) to which distance table updates may be needed
  for(int i = 0; i < 4; i++) {
    if(dt3.costmat[i][j] > dt3.costmat[j][3] + rcvdpkt->mincost[i]) {
      // update costmat with new least-cost path found
      dt3.costmat[i][j] = dt3.costmat[j][3] + rcvdpkt->mincost[i];

      if(i != 3) {
        updateFlag = 1;     // update bit turned on
      }
    }
  }
  if(updateFlag == 1) {
    printf("\nDistance table at router3 updating...\n");
    printdt3(&dt3);

    struct rtpkt pkt0, pkt2;
    pkt0.sourceid = 3;
    pkt0.destid = 0;
    pkt2.sourceid = 3;
    pkt2.destid = 2;
    // Update mincost arrays for each (neighbor's) pkt for router3
    pkt0.mincost[3] = 0;
    pkt2.mincost[3] = 0;
    // Find minimum least-cost paths for router3's neighbors (0 and 2), from the costmat
    // ...values which router3 holds from itself to each of the other routers
    for(int i = 0; i < 4; i++) {
      if(i == 3) {
        continue;
      }
      pkt0.mincost[i] = min(dt3.costmat[i][0], dt3.costmat[i][1], dt3.costmat[i][2]);
      pkt2.mincost[i] = min(dt3.costmat[i][0], dt3.costmat[i][1], dt3.costmat[i][2]);
    }
    printf("\nSending routing pkts to routers(0,2) with updated MINCOST[]:\n");
    // Display updated Distance Vector (for router3)
    for(int i = 0; i < 4; i++) {
      printf("%d\t", pkt0.mincost[i]);
    }
    printf("\n");
    // Send DistVec pkts to router3's neighbors
    tolayer2(pkt0);
    tolayer2(pkt2);
  }
}
/* */

printdt3(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}







