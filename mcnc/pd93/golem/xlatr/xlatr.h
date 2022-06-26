
struct cell *InitLibrary PROTOTYPE((long));
struct cell *AddCell2Library PROTOTYPE((struct cell *,struct cell *));
struct cell *InitCell PROTOTYPE((long, long, long, long, long, long, long, long, long, long));
struct cell *AddPin2Cell PROTOTYPE((struct cell *, long, long, long, long, long, long, long, long));
struct instance *NewInstance PROTOTYPE((long, long, long, long, long, long, long));

struct instance **InitInstances PROTOTYPE((long, long, long, long, long));
struct instance **AddInstance PROTOTYPE((struct instance **,struct instance *));
struct instance **AddTerminal PROTOTYPE((struct instance **,struct terminal *));
struct terminal *NewTerminal PROTOTYPE((long,long,long));

typedef void (*cellFun) PROTOTYPE((FILE*, cellptr, int, int, int));
typedef void (*netFun)  PROTOTYPE((FILE *, long*, int, int, int));

extern void PrintVPNRcell PROTOTYPE((FILE*, cellptr, int, int, int));
extern void WriteVPNRnet PROTOTYPE((FILE *, long*, int, int, int));

extern void PrintYALcell PROTOTYPE((FILE*, cellptr, int, int, int));
extern void WriteYALnet PROTOTYPE((FILE *, long*, int, int, int));

void WriteDB PROTOTYPE((char*,int,int,int,cellFun));
void WriteVPNR PROTOTYPE((char*,int,int,netFun));

void MergeNets PROTOTYPE((void));

/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set sw=4:
 */
