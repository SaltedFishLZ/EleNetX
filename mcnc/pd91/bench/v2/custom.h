/*###########################################################################

                                                         ###
                                                         ###
     #########        #########         ######## #    ######       ######## #
   ###      ###      ###      ##       ##       ##       ###      ##       ##
  ###        ###     ###   ######      ######### #       ###      ######### #
  ###        ###     #######  ###         #######        ###         #######
  ###        ###    ###       ###      #        ##       ###      #        ##
   ###      ###     ###       ###      ##        #       ###      ##        #
     #########       ######### ####    # ########     #######     # ########

#############################################################################

     Copyright 1990 by the Microelectronics Center of North Carolina       
              Designed and written by Krzysztof Kozminski.

     This copy, of MCNC proprietary software, is made available exclusively
     for the use as a part of the 'v2' translator.

###########################################################################*/
#ifndef _custom_h_
#define _custom_h_

/*   $Source: /mcnc/mcnc/kk/benchmarks/stuff/vpnr2yal/RCS/types.h,v $
 *   $Date: 89/08/01 14:25:27 $
 *   $Author: kk $
 *   $Revision: 1.4 $
 */

#ifndef NULL
#include <stdio.h>
#endif NULL

/* Useful defines */

#define LEFT    0
#define TOP     1
#define RIGHT   2
#define BOTTOM  3

/* Forwarded  definitions of all types used in the hierarchical description. The
 * description is composed of module definitions and module instances.  A module
 * definition may contain instances of other modules. The following are possible
 * modules: domain, row, channel, cell, transistor. A module definition contains
 * a set of terminals that interface its signals to the outside,  and also a set
 * of nets that connect the terminals and the I/O ports of the instances of the
 * modules called within this definition. Some nets connect only the instances'
 * I/O ports.  Any entity in the hieraarchy may posess any number of attributes.
 */

/* Module definitions */
typedef struct Module      module,  *module_ptr;   /* Generic module.      */
typedef struct Transistor  trans,   *trans_ptr;    /* Transistor           */
typedef struct Cell     cell, *cell_ptr;           /* Cell definition.     */
typedef struct Row      row,  *row_ptr;            /* Row defnintion.      */
typedef struct Channel     chan, *chan_ptr;        /* Channel defnition.   */
typedef struct Domain      domain,  *dom_ptr;      /* Domain defnition.    */

/* Components included in all definitions. */
typedef struct Term     term, *term_ptr;  /* Module I/O        */
typedef struct Net      net,  *net_ptr;   /* Module net        */
typedef struct Inst     inst, *inst_ptr;  /* Module instance.  */

/* Each I/O terminal may contain several equipotential pins.   */
typedef struct Pin      pin,  *pin_ptr;

/* Components of some particular definitions */
typedef struct Member      eqm,  *eqm_ptr;   /* EClass member.       */
typedef struct EClass      eqc,  *eqc_ptr;   /* Equivalence class.   */
typedef struct Profile     prof, *prof_ptr;  /* Profile.             */

/* A component of an instance. */
typedef struct IOPort      io,   *io_ptr;    /* Instance I/O         */

/* Generic structures */
typedef struct Attr     att,  *att_ptr;            /* Attribute.        */
typedef struct NakedPoint  npoint,  *npoint_ptr;   /* A (X,Y) point.    */
typedef struct Segment     segm, *segmt_ptr;       /* Two points.       */

/*############################################################################*/

/* Point. */

struct NakedPoint { int h,v; };

/* Attribute. Attributes may be associated with any object in the VPNR
 * language.
 */

struct Attr {
   char *attName;
   att_ptr next;  /* Next attribute on a list of attributes.         */
   int attType;   /* Attribute type - tells how to parse attValue.   */
   union {
      int intValue;
      double floatValue;
      npoint  pointValue;
      char *stringValue;
   } attValue;
};

/* Segment: two points + perhaps a list of attributes */

struct Segment {
   npoint first, second;
   att_ptr segAttribs;
};

/* Profile of a module: dynamic array of points.   */

struct Profile {
   int         ptCount;    /* Numbers of points in the profile.   */
   npoint_ptr  profile;    /* Array of points.                    */
};

/*############################################################################*/
/* Each generic module points to its definition in the 'entity' union.
 */

struct Module {
   char     *moduleName;
   att_ptr moduleAttribs;  /* Any universal attributes of the module.      */
   inst_ptr *subModules;   /* Pointers to instances IN the module ...      */
   int      numSubModules; /* ... and their number ...                     */
   int    subHashID;       /* ... and the id of the relevant hash table.   */
   term_ptr *terminals;    /* Pointers to terminals of the module ...      */
   int      numTerminals;  /* ... and their number ...                     */
   int    termHashID;      /* ... and the id of the relevant hash table.   */
   net_ptr  *nets;         /* Pointers to nets internal to the module...   */
   int      numNets;       /* ... and their number ...                     */
   int    netHashID;       /* ... and the id of the relevant hash table.   */
   int what;               /* Tells what this module really is.            */
   union {                 /* Additional info dependent on the above.      */
      cell_ptr mCell;
      dom_ptr mDom;
      row_ptr mRow;
      chan_ptr mChannel;
      trans_ptr mTransistor;
   } entity ;
};

/* Module terminal. A terminal is a definition of an I/O port, through which a
 * module communnicates with the exterior.  IOPort structure, defined later,
 * is an instance of a terminal.  Net associated with a terminal is internal
 * to the module contained in the terminal.
 */
struct Term {
   module_ptr whichModule; /* Module containing this terminal.             */
   net_ptr whichNet;       /* Net containing this terminal.                */
   int numPins;            /* Number of equivalent pins of this I/O port.  */
   pin_ptr *pins;          /* Array of pointers to these pins.             */
   eqm_ptr eqMem;          /* Backlink from terminal to member.            */
   att_ptr termAttribs;    /* Terminal attributes.                         */
};
/* A terminal pin.  A module may have several electrically connected pins that
 * all represent a single terminal.
 */

struct Pin {
   int xmin,xmax,ymin,ymax;   /* Pin coordinates.                       */
   int shape;                 /* Terminal shapes are listed below.      */
   att_ptr pinAttribs;        /* Pin attributes.                        */
};
/* Codes of the pin shapes. */

#define TERM_POINT   0x0001      /* A point                       */
#define TERM_HORIZ   0x0002      /* A horizontal segment.         */
#define TERM_VERT 0x0003         /* A vertical segment.           */
#define TERM_SQUARE  0x0004      /* A square.                     */
#define TERM_SHAPE_MASK 0x0007

#define TERM_VIRTUAL 0x0008      /* A point, the coordinates of which are     */
                                 /* allowed segments on the module boundary,  */
                                 /* where the Terminal is allowed to occur.   */
#define TERM_INTERVAL   0x0080   /* Modifies a virtual terminal to have its   */
                                 /* coords expressed as percentages of the    */
                                 /* module dimensions.                        */
#define TERM_ON_TOP  0x0010
#define TERM_ON_LEFT 0x0020
#define TERM_ON_RIGHT   0x0030
#define TERM_ON_BOT  0x0040
#define TERM_LOC_MASK   0x0070


/* Netlist.  Netlist defined here is internal to a 'parent' module.  It connects
 * to IOPorts of sub-modules of the parent (the 'subterminals' list), and to
 * the terminals of the 'parent' module placed on the 'exits'list.
 */
struct Net {
   module_ptr netParent;   /* Module containing this net.            */
   char *netName;          /* Name of the netlist within the module. */
   term_ptr exit;          /* Net connects to the outside via this.  */
   io_ptr *subTerminals;   /* Ports of the subcells of the 'parent'. */
   int numTerms;           /* Number of the above.                   */
   int netFlags;
   att_ptr netAttribs;     /* Net attributes.                        */
};

#define NET_PRINTED 0x1    /* Flag to mark nets output in EDIF */

/* An instance of a module.
 */
struct Inst {
   module_ptr reference;   /* The instantiated module.               */
   char *instName;         /* Instance name.                         */
   module_ptr instParent;  /* Module containing the instance.        */
   npoint xlation;         /* Translation (negative == unknown).     */
   int xform;              /* Transformation.                        */
   io_ptr *interface;      /* I/O ports of an instance.              */
   att_ptr instAttribs;    /* Instance attributes.                   */
};

/* I/O port. The instances of a module terminals are its I/O ports.
 */
struct IOPort {
   inst_ptr interfaceBak;  /* Backlink to the instance with the port */
   net_ptr  net;           /* Netlist that exits via this port.      */
   term_ptr portDef;       /* Link from an instance I/O port to the  */
                           /* entity definition I/O port.            */
   att_ptr ioAttribs;      /* Attributes of the port.                */
};

/*############################################################################*/

/* Equivalence classes of terminals.  Equivalence classes are expressed as
 * trees to capture thinks like equvalence of two pairs of terminals, where
 * each pair can be swapped for th other.
 */

/* Member of an equivalence class.
 */

struct Member {
   int what;                  /* What is this node.            */
   union {
      eqc_ptr classMember;    /* Equivalence classe.           */
      term_ptr termMember;    /* Terminals.                    */
   } memb;
   att_ptr memberAttribs;     /* Attributes of this class.     */
};

/* Definition of a node of an equivalence tree  */

struct EClass {
   char *className;  /* Name of the equivalence class.               */
   cell_ptr cellBak; /* Backlink to the cell containing this class   */
   int numMemb;      /* Number of members.                           */
   eqc_ptr members;  /* Pointer to the array of members.             */
};

/*############################################################################*/

/* Transistor definition.  */

struct Transistor {
   int width, length;         /* Transistor dimensions (nanometers)  */
   int ttype;                 /* Transistor type.                    */
   att_ptr  trAttributes;     /* LIFO of attributes.                 */
};

#define P_TYPE 1
#define N_TYPE 2

/* Cell definition.  */

struct Cell {
   prof_ptr profiles[4];   /* Cell profile.                          */
   eqc_ptr cellEqs;        /* Equivalence classes of cell signals    */
};


/* Row definition.   */

struct Row {
   prof_ptr profiles[4];
};


/* Channel definition.  */

struct Channel {
   prof_ptr profiles[4];
};


/* Domain definition.   */

struct Domain {
   prof_ptr profiles[4];
};


/*############################################################################*/

/* A record with technology description.  */

struct Technology {
   char *techName;
   char *wireNames[3];
   float unit;
   int width[4];
   int sep[4];
   int contsize[4];
   int contcont[4];
   int contedge[4];
   int viasize[4];
   int viavia[4];
   int viacont[4];
   int viaedge[4];
   int xgrid;
   int ygrid;
   int yoffset;
   att_ptr techAttribs;
} techrecord, *tech_ptr;

#define ATTRIB_PTR   att_ptr
#define CELL_PTR  module_ptr
#define DOMAIN_PTR   module_ptr
#define EQUIV_PTR eqc_ptr
#define PIN_PTR      pin_ptr
#define PORT_PTR  term_ptr
#define DPORT_PTR term_ptr
#define PROFILE_PTR  prof_ptr
#define XTOR_PTR  inst_ptr
#define INST_PTR  inst_ptr

GLOBVAR(int,TmpNames,);
GLOBVAR(int,ModuleNames,);
GLOBVAR(int,AttNames,);
GLOBVAR(int,AttValues,);

GLOBVAR(int,NumDomains,=0);
GLOBVAR(int,NumCells,=0);
GLOBVAR(module_ptr,*AllDomains,=NULL);
GLOBVAR(module_ptr,*AllCells,=NULL);
GLOBVAR(Boolean,doWarnings,=false);
GLOBVAR(Boolean,doRWarnings,=false);
GLOBVAR(Boolean,commentWarnings,=false);
GLOBVAR(Boolean,do87,= false);
GLOBVAR(Boolean,do87widths,=true);
GLOBVAR(Boolean,do87parent,=false);
GLOBVAR(Boolean,doLibrary,= true);
GLOBVAR(Boolean,doCircuit,= true);
GLOBVAR(Boolean,revPins,= false);
GLOBVAR(Boolean,guessPins,= true);

EXT struct WARN_INFO {
   int count;
   char *info;
} warningCount[]
#ifdef MAIN_PROGRAM_MODULE
= {
  { 0, "unknown cell scale" }
 ,{ 0, "unknown cell type" }
 ,{ 0, "unknown pin type" }
 ,{ 0, "unknown pin layer" }
 ,{ 0, "redefined profile" }
 ,{ 0, "redundant signal name" }
 ,{ 0, "strange vpnr pin type" }
 ,{ 0, "relaxed pin interval" }
 ,{ -1, ""}
}
#endif MAIN_PROGRAM_MODULE
;

#define WARN_CSCALE  0
#define WARN_CTYPE   1
#define WARN_PTYPE   2
#define WARN_PLAYER  3
#define WARN_REDPROF 4
#define WARN_REDSIGN 5
#define WARN_PUTYPE  6
#define WARN_RELAX   7

#define WARN(i)   warningCount[i].count += 1

/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set ts=3 sw=3:
 */
#endif _custom_h_
