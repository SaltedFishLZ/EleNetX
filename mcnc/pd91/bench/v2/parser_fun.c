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
#include <stdio.h>
#include "unixproto.h"
#include "oasisutils.h"
#include "parser.h"
#include "custom.h"
#include "parser_proto.h"

extern int iocount;

/******************************************************************************/
/* Functions used to initialize various entities.                             */
/******************************************************************************/

/* This function is called upon reading any attribute (that is not a point
 * attribute) of any VPNR entity.
 * SAMPLE:    a { (0-3,0) (0-3,100) } capacitance=0.3 pintype=pi ;
 *                                                   |          |
 *                                  will be caled here...and here
 */

/* Create an attribute record.  Attribute names and string-type attributes are
 * stored in two hash tables, AttNames and AttValues, allocated in the main
 * segment.
 */
ATTRIB_PTR InitAttrib(apType,apName,apValStr)
int apType;
char *apName;
char *apValStr;
{
   register ATTRIB_PTR ap;
   ap = (ATTRIB_PTR) MessPtr(sizeof(*ap));
   ap->next = NULL;
   UseHashTable(AttNames);
   RegisterName(apName,&ap->attName);
   ap->attType = apType;
   switch (apType) {
   case zzINTEGER:
      ap->attValue.intValue = atoi(apValStr);
      break;
   case zzID:
   case zzQUOTESTR:
      UseHashTable(AttValues);
      RegisterName(apValStr,&ap->attValue.stringValue);
      break;
   case zzNUMBER:
      ap->attValue.floatValue = atof(apValStr);
      break;
   default:
      ErrMessage("Defaulted in InitAttrib(%d,...\n",apType);
      exit(1);
   }
   return ap;
}

/* This function is called upon reading a point attribute of any VPNR entity.
 * SAMPLE:    pin B:(10,20) width=100 desired=(1234,5678)
 *                                                       |
 *                                      will be caled here
 */

/* Create an attribute record.  Attribute names and string-type attributes are
 * stored in two hash tables, AttNames and AttValues, allocated in the main
 * segment.
 */
ATTRIB_PTR InitPointAttrib(apName,x,y)
char *apName;
int x,y;
{
   register ATTRIB_PTR ap;
   ap = (ATTRIB_PTR) MessPtr(sizeof(*ap));
   ap->next = NULL;
   UseHashTable(AttNames);
   RegisterName(apName,&ap->attName);
   ap->attType = zzPOINT;
   ap->attValue.pointValue.h = x;
   ap->attValue.pointValue.v = y;
   return ap;
}

/* This function is called after reading a name of a VPNR cell.
 * SAMPLE:    cell begin a2s logfun=AND area=1856 transistors=4
 *                          |
 *                called here
 */

/* Initialize a cell record, which is really a generic module record.  Names of
 * all modules are stored in a hash table ModuleNames, which is allocated in the
 * main segment of the program.  This has table is also used to retrieve the
 * pointers to the module definitions by the module names.  The hash tables for
 * the nets, submodules and terminals of the module are not alocated yet.  They
 * will be allocated upon adding any of these entities into the module data
 * structure.
 */
CELL_PTR InitCell(cpName)
char* cpName;
{
   register CELL_PTR cp;
   register cell_ptr cptr;
   register int nc = NumCells;

   NumCells += 1;
   if (nc == 0) {
      AllCells = (module_ptr*) MessPtr(sizeof(*AllCells));
   } else 
      AllCells = (module_ptr*) realloc((char*)AllCells
            ,NumCells*sizeof(*AllCells));
   AllCells[nc] = cp = (CELL_PTR) MessPtr(sizeof(*cp));

   UseHashTable(ModuleNames);
   SetNameData(cpName,&cp->moduleName,(voidptr)cp);
   cp->moduleAttribs = NULL;
   cp->numSubModules = 0;
   cp->numTerminals = 0;
   cp->numNets = 0;

   cp->what = zzCELL;
   cp->entity.mCell = cptr = (cell_ptr) MessPtr(sizeof(*cptr));

   /* Initialize cell-specific data. */
   cptr->profiles[0] = cptr->profiles[1] = cptr->profiles[2]
    = cptr->profiles[3] = NULL;
   cptr->cellEqs = NULL;

   return cp;
}

/* This function is called imediately after reading a 'siglist' keyword. */

/* Initialize cell signal list.  Nothing needs to be done here with my data
 * structures - it is all handled in 'InitCell()' and 'AddCellSignal()' - KK.
 */
CELL_PTR InitCellSigList(cp)
CELL_PTR cp;
{
   return cp;
}

/* This function is called imediately after reading a 'translist' keyword. */

/* Initialize transistor list in a cell.  Transistors are module instances.
 * Nothing needs to be done here with my data structures - it is all handled
 * in 'InitCell()' and 'AddXtor()' - KK.
 */
CELL_PTR InitXtorList(cp)
CELL_PTR cp;
{
   return cp;
}

/* This function is called after reading the first point of ANY profile.
 * SAMPLE:   profile top (0,100) (80,100) (80,80) (120,80) ;
 *                              |
 *                    called here
 */

/* Initialize a profile record and insert the first point.
 */

PROFILE_PTR InitProfile(x,y)
int x;
int y;
{
   register PROFILE_PTR pp;
   register npoint_ptr np;

   pp = (PROFILE_PTR) MessPtr(sizeof(*pp));
   pp->ptCount = 1;
   pp->profile = np = (npoint_ptr) MessPtr(sizeof(*np));
   np->h = x;
   np->v = y;

   return pp;
}

/* This function is called after reading any of the alternate locations of a
 * cell terminal.
 * SAMPLE:    a { (0-3,0) (0-3,100) } capacitance=0.3 pintype=pi ;
 *                       |         |
 *             called here  and here
 */

/* Create a pin record, plug in the data and determine the pin shape. The cell
 * pins are always fixed - there are no virtual ones.
 */

PIN_PTR InitCellPin(xmin,xmax,ymin,ymax)
int xmin;
int xmax;
int ymin;
int ymax;
{
   register PIN_PTR pp;
   pp = (PIN_PTR) MessPtr(sizeof(*pp));
   pp->xmin = xmin;
   pp->xmax = xmax;
   pp->ymin = ymin;
   pp->ymax = ymax;
   pp->pinAttribs = NULL;
   if (xmax == xmin)
      if (ymax == ymin)
         pp->shape = TERM_POINT;
      else
         pp->shape = TERM_VERT;
   else
      if (ymax == ymin)
         pp->shape = TERM_HORIZ;
      else
         pp->shape = TERM_SQUARE;
   return pp;
}

/* This function is called after reading the name of an equivalence class and
 * the name of the first member of this class.
 * SAMPLE:    eqa ( a b c ) ;
 *                   |
 *         called here
 */

/* For now, we happily ignore the terminal/pin equivalences.
 */
EQUIV_PTR InitEquiv(eqClass,eqMember)
char *eqClass;
char *eqMember;
{
   return NULL;
}

/* This function is called after reading the name of any cell port.
 * SAMPLE:    a { (0-3,0) (0-3,100) } capacitance=0.3 pintype=pi ;
 *             |
 *   called here
 */

/* Initialize a cell terminal record. The port name is still the TmpNames hash
 * table at this time.  It will be added to the list of cell signals later,
 * in the AddCellPort();
 */
PORT_PTR InitCellPort(portName)
char* portName;
{
   register PORT_PTR pp;
   pp = (PORT_PTR) MessPtr(sizeof(*pp));
   pp->numPins = 0;
   pp->eqMem = NULL;
   pp->termAttribs = NULL;
   pp->whichNet = (net_ptr) portName;
   return pp;
}

/* This function is called after reading the name of the first signal connected
 * to a cell instance in a domain.
 * SAMPLE:      a2s INST12 ( sig12, sig32, sig15 )
 *                                 |
 *                       called here
 */

/* Initialize an instance record.  The instance name is still in TmpNames hash
 * table. It will be added to the table of instances in AddCellInstance().
 */
INST_PTR InitCellInst(master,ident,sigName)
char* master;
char* ident;
char* sigName;
{
   register INST_PTR ip;
   register CELL_PTR cp;
   register io_ptr *iop;
   register int i;

   ip = (INST_PTR) MessPtr(sizeof(*ip));
   UseHashTable(ModuleNames);
   ip->reference = cp = (CELL_PTR) GetNameData(master);
   if (cp == NULL) {
      Message("FATAL ERROR while reading cell instance '%s'",ident);
      ErrMessage("            Cell '%s' not found in the database"
         ,master);
      exit(1);
   }
   ip->instName = ident;
   iop = (io_ptr*) MessPtr( (cp->numTerminals)*sizeof(*iop));
   ip->interface = iop;
   for (i=cp->numTerminals; i>0; i--, iop++)
      *iop = NULL;
   ip->instAttribs = NULL;
   return AddInstanceNet(ip,sigName);
}

/* This function is called after reading all names of the signals connected
 * to a transistor instance in a cell.
 * SAMPLE:      pa a n512 vdd width=17 length=2 type=p
 *                           |
 *                 called here
 */

/* Initialize a transistor instance.  The instance name is still in TmpNames
 * hash table. It will be added to the table of instances in AddXtor().  The
 * transistor definition is always identical.
 */
XTOR_PTR InitCellXtor(ident,gate,source,drain)
char *ident;
char *gate;
char *source;
char *drain;
{
   register INST_PTR ip;
   register io_ptr *iop;

   ip = (INST_PTR) MessPtr(sizeof(*ip));
   UseHashTable(ModuleNames);
   ip->reference = (CELL_PTR) GetNameData("");
   ip->instName = ident;
   ip->interface = iop = (io_ptr*) MessPtr(3*sizeof(*iop));
   iop[0] = iop[1] = iop[2] = NULL;
   ip->instAttribs = NULL;
   (void) AddInstanceNet(ip,gate);
   (void) AddInstanceNet(ip,source);
   (void) AddInstanceNet(ip,drain);
   iocount -= 3;
   return ip;
}

/* This function is called after reading each specification for a member of
 * a domain's iolist.
 * SAMPLE:   reset T:(0,100) pintype=reset
 *                          |
 *                called here
 */

/* Initialize a domain pin.  This function is similar to initializing a cell
 * port and pin simultaneously.  The port record may be discarded in the
 * function AddDomainPort(), if found to be a duplicate of another port.  Adding
 * a pin to the port is identical as adding a pin to a cell port.  The port name
 * is still the TmpNames hash table at this time.  It will be added to the list
 * of cell signals later, in the AddDomainPort();
 */
DPORT_PTR InitDomainPort(sigName,sigSide,from,to)
char* sigName;
char* sigSide;
int from;
int to;
{
   register PIN_PTR pip;
   register PORT_PTR pp;

   pp = (PORT_PTR) MessPtr(sizeof(*pp));
   pp->numPins = 0;
   pp->eqMem = NULL;
   pp->termAttribs = NULL;
   pp->whichNet = (net_ptr) sigName;

   pip = (PIN_PTR) MessPtr(sizeof(*pip));
   pip->pinAttribs = NULL;
   /* Only one of these pairs will be ever looked at, so we set both */
   pip->ymin = pip->xmin = from;
   pip->ymax = pip->xmax = to;
   if (strEQ(sigSide,"T"))
      pip->shape = TERM_ON_TOP;
   else if (strEQ(sigSide,"R"))
      pip->shape = TERM_ON_RIGHT;
   else if (strEQ(sigSide,"L"))
      pip->shape = TERM_ON_LEFT;
   else if (strEQ(sigSide,"B"))
      pip->shape = TERM_ON_BOT;
   else {
      Message("FATAL ERROR while reading pin '%s:%s (%d,%d)'"
         ,sigName,sigSide,from,to);
      ErrMessage("            Illegal chip side code '%s'",sigSide);
      exit(0);
   }
   if (from != to)
      pip->shape |= TERM_INTERVAL;
   return AddCellPortPin(pp,pip);
}

/* This function is called after reading a name of a VPNR domain.
 * SAMPLE:    domain begin highway scale=0.1
 *                                |
 *                      called here
 */

/* Initialize a domain record.  This is very similar to InitCell - look up the
 * comments about the hash tables there.
 */
DOMAIN_PTR InitDomain(domName)
char* domName;
{
   register DOMAIN_PTR dp;
   register dom_ptr dptr;
   register int nd = NumDomains;

   NumDomains += 1;
   if (nd == 0)
      AllDomains = (module_ptr*) MessPtr(sizeof(*AllDomains));
   else 
      AllDomains = (module_ptr*) realloc((char*)AllDomains
            ,NumDomains*sizeof(*AllDomains));
   AllDomains[nd] = dp = (DOMAIN_PTR) MessPtr(sizeof(*dp));

   UseHashTable(ModuleNames);
   SetNameData(domName,&dp->moduleName,(voidptr)dp);
   dp->moduleAttribs = NULL;
   dp->numSubModules = 0;
   dp->numTerminals = 0;
   dp->numNets = 0;

   dp->what = zzDOMAIN;
   dp->entity.mDom = dptr = (dom_ptr) MessPtr(sizeof(*dptr));

   dptr->profiles[0] = dptr->profiles[1] = dptr->profiles[2]
    = dptr->profiles[3] = NULL;

   return dp;
}

/******************************************************************************/
/* Functions used to add various entities to cell definitions.                */
/******************************************************************************/

/* This function is called after reading each cell attribute and is
 * passed a pointer to the attribute record created by the general
 * attribute routine InitAttrib.
 * SAMPLE:    cell begin a2s logfun=AND area=1856 transistors=4
 *                                     |         |             |
 *                           called here,     here,     and here
 */

/* No need to do anything special with the attribute, so just tack it on.
 */
CELL_PTR AddCellAttrib(cp,ap)
CELL_PTR cp;
ATTRIB_PTR ap;
{
   ap->next = cp->moduleAttribs;
   cp->moduleAttribs = ap;
   return cp;
}

/* This function is called after reading an entire description of an equivalence
 * class.
 * SAMPLE:    eqa ( a b c ) ;
 *                           |
 *                 called here
 */

/* For now, we happily ignore the terminal/pin equivalences.
 */
CELL_PTR AddCellEquiv(cp,ep)
CELL_PTR cp;
EQUIV_PTR ep;
{
   return cp;
}

/* Auxiliary functions used in AddCellPort() and AddCellSignal().  */

static net_ptr newNet(netName,cp)
char *netName;
CELL_PTR cp;
{
   register net_ptr np = (net_ptr) MessPtr(sizeof(*np));
   np->netParent = cp;
   UseHashTable(cp->netHashID);
   SetNameData(netName,&np->netName,(voidptr)np);
   np->numTerms = 0;
   np->netAttribs = NULL;
   np->exit = NULL;
   np->netFlags = 0;
   return np;
}

static void appendNet2Module(np,cp)
net_ptr np;
module_ptr cp;
{
   register int nn = cp->numNets;
   cp->numNets += 1;
   if (nn == 0) {
      cp->nets = (net_ptr*) MessPtr(sizeof(*cp->nets));
   } else 
      cp->nets = (net_ptr*) realloc((char*)cp->nets
            ,cp->numNets*sizeof(*cp->nets));
   cp->nets[nn] = np;
}

/* This function is called after reading an entire description of a cell port.
 * SAMPLE:    a { (0-3,0) (0-3,100) } capacitance=0.3 pintype=pi ;
 *                                                                |
 *                                                      called here
 */

CELL_PTR AddCellPort(cp,port)
CELL_PTR cp;
PORT_PTR port;
{
   register int nt = cp->numTerminals;
   register char *portName = (char*) port->whichNet;
   register net_ptr np;

  /* Increase the dynamic array of pointers
   */
   cp->numTerminals += 1;
   if (nt == 0) {
      cp->terminals = (PORT_PTR*) MessPtr(sizeof(*cp->terminals));
      cp->termHashID = UseHashTable(0);
      cp->netHashID = UseHashTable(0);
   } else 
      cp->terminals = (PORT_PTR*) realloc((char*)cp->terminals
            ,cp->numTerminals*sizeof(*cp->terminals));
   cp->terminals[nt] = port;
   UseHashTable(cp->termHashID);
   SetNameData(portName,NULL,(voidptr)port);

  /* Move the port name from the TmpNames hash table to the table containing
   * the list of cell signals.
   */
   UseHashTable(cp->netHashID);
   np = (net_ptr) GetNameData(portName);
   if (np == NULL)
      np = newNet(portName,cp);
   else {
      ErrMessage("FATAL ERROR - Illegal duplicate terminal '%s' in cell '%s'"
         ,portName,cp->moduleName);
      exit(1);
   }
   np->exit = port;
   port->whichNet = np;
   port->whichModule = cp;
   appendNet2Module(np,cp);
   return cp;
}

/* This function is called after reading an entire cell profile.
 * SAMPLE:   profile top (0,100) (80,100) (80,80) (120,80) ;
 *                                                          |
 *                                                called here
 */
CELL_PTR AddCellProfile(cp,prp,whichProf,prname)
CELL_PTR cp;
PROFILE_PTR prp;
int whichProf;
char *prname;
{
   PROFILE_PTR *prpp;

   switch(whichProf) {
   case zzTOP:
      whichProf = TOP;
      break;
   case zzBOT:
      whichProf = BOTTOM;
      break;
   case zzLEFT:
      whichProf = LEFT;
      break;
   case zzRIGHT:
      whichProf = RIGHT;
      break;
   default:
      ErrMessage("PROGRAMMING ERROR - unknown profile read (%d)"
         ,whichProf);
      exit(1);
   }
   prpp = cp->entity.mCell->profiles + whichProf;
   if (*prpp != NULL) {
      WARN(WARN_REDPROF);
      if (doWarnings)
         ErrMessage("WARNING: redefined %s profile for '%s'"
            ,prname,cp->moduleName);
   }
   *prpp = prp;
   return cp;
}

/* This function is called after reading any signal internal to the cell.
 * SAMPLE:   siglist  n188  n268      n564 ;
 *                  |     |     |         |
 *        called here, here, here, and here
 */
CELL_PTR AddCellSignal(cp,sigName)
CELL_PTR cp;
char *sigName;
{
   register net_ptr np;
   UseHashTable(cp->netHashID);
   np = (net_ptr) GetNameData(sigName);
   if (np == NULL) {       /* Create net record.   */
      np = newNet(sigName,cp);
      appendNet2Module(np,cp);
   } else {
      WARN(WARN_REDSIGN);
      if (doRWarnings) {
         Message("WARNING: Redundant signal name '%s' in cell '%s'"
            ,sigName,cp->moduleName);
         ErrMessage("         It was listed already as a %s"
            ,np->exit ? "terminal" : "signame");
      }
   }
   return cp;
}

/* Auxiliary function called in AddXtor() */

static void joinSubmoduleNets(mp,ip)
module_ptr mp;
inst_ptr ip;
{
   io_ptr *iop = ip->interface;
   int i;
   UseHashTable(mp->netHashID);
   for (i=0; i<ip->reference->numTerminals; iop++, i++)
      if (*iop != NULL) {
         register int nst;
         net_ptr np = (net_ptr) GetNameData((char*)((*iop)->net));
         if (np == NULL)
            np = newNet((char*)((*iop)->net),mp);
         (*iop)->net = np;
         nst = np->numTerms;
         np->numTerms += 1;
         if (nst == 0)
            np->subTerminals = (io_ptr*) MessPtr(sizeof(*np->subTerminals));
         else
            np->subTerminals = (io_ptr*) realloc((char*)np->subTerminals
               ,np->numTerms*sizeof(*np->subTerminals));
         np->subTerminals[nst] = *iop;
         appendNet2Module(np,mp);
      }
}

/* This function is called after reading a complete transistor description.
 * SAMPLE:      pa a n512 vdd width=17 length=2 type=p
 *                                                    |
 *                                          called here
 */

/* We add a transistor instance to the list of cell submodules and process all
 * nets connected to the transistor.
 */
CELL_PTR AddXtor(cp,xtor)
CELL_PTR cp;
XTOR_PTR xtor;
{
   register char *instName = xtor->instName;
   register int nsm = cp->numSubModules;

   cp->numSubModules += 1;
   /* Initialize the hash table of subModules, if necessary */
   if (nsm == 0) {
      cp->subModules = (INST_PTR*) MessPtr(sizeof(*cp->subModules));
      cp->subHashID = UseHashTable(0);
   } else 
      cp->subModules = (INST_PTR*) realloc((char*)cp->subModules
            ,cp->numSubModules*sizeof(*cp->subModules));
   /* Check for duplicate instance name */
   UseHashTable(cp->subHashID);
   if (GetNameData(instName) != NULL) {
      ErrMessage("FATAL ERROR - Duplicate transistor name '%s' in cell '%s'"
         ,instName,cp->moduleName);
      exit(1);
   }
   /* Add the instance */
   SetNameData(instName,&xtor->instName,(voidptr)xtor);
   cp->subModules[nsm] = xtor;

   /* Process the nets */
   joinSubmoduleNets(cp,xtor);

   return cp;
}

/* This function is called after reading the end of the cell definition.
 * SAMPLE:    cell end a2s
 *                        |
 *              called here
 */
CELL_PTR WrapUpCell(cp,cpName)
CELL_PTR cp;
char* cpName;
{
   return cp;
}

/* This function is called after reading the end of the list of signals
 * internal to the cell.
 * SAMPLE:   siglist n188 n268  n564 ;
 *                                    |
 *                          called here
 */
CELL_PTR WrapUpCellSiglist(cp)
CELL_PTR cp;
{
   return cp;
}

/* This function is called after reading the end of the list of transistors.
 */
CELL_PTR WrapUpXtors(cp)
CELL_PTR cp;
{
   return cp;
}

/* This function is called after reading the second and subsequent mebers of an
 * equivalence class.
 * SAMPLE:    eqa ( a b c ) ;
 *                     | |
 *             called here
 */

/* For now, we happily ignore the terminal/pin equivalences.
 */
EQUIV_PTR AddEquiv(ep,eqMember)
EQUIV_PTR ep;
char *eqMember;
{
   return ep;
}

/* This function is called after reading the ending paren of the equivalence
 * class description.
 * SAMPLE:    eqa ( a b c ) ;
 *                         |
 *               called here
 */

/* For now, we happily ignore the terminal/pin equivalences.
 */
EQUIV_PTR WrapUpEquiv(ep)
EQUIV_PTR ep;
{
   return ep;
}

/* This function is called after reading any attribute of a physical pin in
 * a definition of a cell terminal.
 * SAMPLE:    a { (0-3,0) layer=METAL2 (0-3,100) layer=METAL2 }
 *                                    |                      |
 *                          called here               and here
 */

/* For now, we just tack the attribute on the list.
 */
PIN_PTR AddCellPinAttrib(pp,ap)
PIN_PTR pp;
ATTRIB_PTR ap;
{
   ap->next = pp->pinAttribs;
   pp->pinAttribs = ap;
   return pp;
}

/* This function is called after reading any attribute that refers to the
 * entire cell terminal.
 * SAMPLE:    a { (0-3,0) (0-3,100) } capacitance=0.3 pintype=pi ;
 *                                                   |          |
 *                                  will be caled here...and here
 */

/* For now, we just tack the attribute on the list.
 */
PORT_PTR AddCellPortAttrib(port,ap)
PORT_PTR port;
ATTRIB_PTR ap;
{
   ap->next = port->termAttribs;
   port->termAttribs = ap;
   return port;
}

/* This function is called after reading any physical pin of a cell port
 * (after all pin attributes have been read in).
 * SAMPLE:    a { (0-3,0) layer=METAL2  (0-3,100) } capacitance=0.3 pintype=pi ;
 *                                    |          |
 *                   will be caled here...and here
 */
PORT_PTR AddCellPortPin(port,pp)
PORT_PTR port;
PIN_PTR pp;
{
   register int npins = port->numPins;

   port->numPins += 1;
   if (npins == 0) {
      port->pins = (PIN_PTR*) MessPtr(sizeof(*port->pins));
   } else 
      port->pins = (PIN_PTR*) realloc((char*)port->pins
            ,port->numPins*sizeof(*port->pins));
   port->pins[npins] = pp;
   return port;
}

/* This function is called after reading the entire description of a cell port
 * SAMPLE:  a { (0-3,0) layer=METAL2  (0-3,100) } capacitance=0.3 pintype=pi ;
 *                                                                            |
 *                                                           will be caled here
 */
PORT_PTR WrapUpCellPort(port)
PORT_PTR port;
{
   return port;
}

/* This function is called after reading all pins of a cell port
 * SAMPLE:  a { (0-3,0) layer=METAL2  (0-3,100) } capacitance=0.3 pintype=pi ;
 *                                               |
 *                              will be caled here
 */
PORT_PTR WrapUpCellPortPins(port)
PORT_PTR port;
{
   return port;
}

/* This function is called after reading the second and subsequent points
 * of a profile.
 * SAMPLE:   profile top (0,100) (80,100) (80,80) (120,180) ;
 *                              |        |       |         |
 *                    called here,    here,   here, and here
 */
PROFILE_PTR AppendProfilePoint(pp,x,y)
PROFILE_PTR pp;
int x;
int y;
{
   register npoint_ptr np;
   int ptc = pp->ptCount;

   pp->ptCount += 1;
   pp->profile = (npoint_ptr) realloc((char*)pp->profile
            ,pp->ptCount*sizeof(*pp->profile));
   np = pp->profile + ptc;
   np->h = x;
   np->v = y;

   return pp;
}

/* This function is called after reading any transistor attribute
 * SAMPLE:      pa a n512 vdd width=17 type=p  length=2
 *                                    |      |         |
 *                          called here,  here, and here
 */

/* For now, we just tack the attribute on the list.
 */
XTOR_PTR AddXtorAttrib(xtor,ap)
XTOR_PTR xtor;
ATTRIB_PTR ap;
{
   ap->next = xtor->instAttribs;
   xtor->instAttribs = ap;
   return xtor;
}

/* This function is called after reading any cell instance, but before
 * reading any attributes
 * SAMPLE:      a2s INST12 ( sig12, sig32, sig15 ) placement=keep_in_row
 *                                                |
 *                                      called here
 */
INST_PTR WrapUpInstance(instPtr)
INST_PTR instPtr;
{
   return instPtr;
}

/* This function is called after reading the end of the domain definition.
 * SAMPLE:    domain end highway
 *                              |
 *                    called here
 */
DOMAIN_PTR WrapUpDomain(domPtr,domName)
DOMAIN_PTR domPtr;
char* domName;
{
   return domPtr;
}


/* This function is called after reading the second and subsequent signals
 * connected to a cell instance.
 * SAMPLE:      a2s INST12 ( sig12, sig32, sig15987 )
 *                                        |        |
 *                              called here and here
 */
INST_PTR AddInstanceNet(instPtr,sigName)
INST_PTR instPtr;
char* sigName;
{
   int i, nports = instPtr->reference->numTerminals;
   io_ptr ip, *ipp = instPtr->interface;

   for (i=0; i<nports; i++, ipp++)
      if ((*ipp) == NULL)
         break;
   if (i >= nports) {
      ErrMessage("FATAL ERROR - Too many nets connected to instance '%s'"
         ,instPtr->instName);
      exit(1);
   }
   *ipp = ip = (io_ptr) MessPtr(sizeof(*ip));
   ip->interfaceBak = instPtr;
   ip->portDef = instPtr->reference->terminals[i];
   ip->ioAttribs = NULL;
   ip->net = (net_ptr) sigName;

   iocount += 1;
   return instPtr;
}

/* This function is called after reading any cell instance attribute
 * SAMPLE:      a2s INST12 ( sig12, sig32, sig15 ) placement=keep_in_row
 *                                                                      |
 *                                                            called here
 */
INST_PTR AddInstanceAttrib(instPtr,ap)
INST_PTR instPtr;
ATTRIB_PTR ap;
{
   ap->next = instPtr->instAttribs;
   instPtr->instAttribs = ap;
   return instPtr;
}

/* This function is called after reading an entire domain profile.
 * SAMPLE:   profile top (0,0) (0,0) ;
 *                                    |
 *                          called here
 */
DOMAIN_PTR AddDomainProfile(domPtr,profPtr,whichProf,prname)
DOMAIN_PTR domPtr;
PROFILE_PTR profPtr;
int whichProf;
char *prname;
{
   return AddCellProfile(domPtr,profPtr,whichProf,prname);
}

/* This function is called after reading a iolist attribute.
 * SAMPLE:   reset T:(0,100) pintype=reset
 *                                        |
 *                              called here
 */

/* If the attribute is 'layer; it gets moved to the list of the pin attributes.
 * Otherwise, it is tacked on the list of port attributes.
 */
DPORT_PTR AddDomainPortAttrib(portPtr,ap)
DPORT_PTR portPtr;
ATTRIB_PTR ap;
{
   if (strNE(ap->attName,"pintype")) {
      ap->next = portPtr->pins[0]->pinAttribs;
      portPtr->pins[0]->pinAttribs = ap;
   } else {
      ap->next = portPtr->termAttribs;
      portPtr->termAttribs = ap;
   }
   return portPtr;
}

/* This function is called after reading a iolist port, including attributes.
 * SAMPLE:   reset T:(0,100) pintype=reset
 *                                        |
 *                              called here
 */

/* This function is similar to AddCellPort, except that having duplicate
 * port names is OK.  In a case of duplicate, the pin info and attributes are
 * transferrec to the previously defined port.
 */
DOMAIN_PTR AddDomainPort(domPtr,port)
DOMAIN_PTR domPtr;
DPORT_PTR port;
{
   register int nt = domPtr->numTerminals;
   register char *portName = (char*) port->whichNet;
   register net_ptr np;
   register term_ptr tp;

  /* Increase the dynamic array of pointers
   */
   if (nt == 0) {
      domPtr->terminals = (PORT_PTR*) MessPtr(sizeof(*domPtr->terminals));
      domPtr->termHashID = UseHashTable(0);
      domPtr->netHashID = UseHashTable(0);
   }
   UseHashTable(domPtr->termHashID);
   tp = (term_ptr) GetNameData(portName);

   /* If new signal name, add a new port to the domain.  Otherwise, add
    * the new pin to the old port.
    */
   
   if (tp == NULL) {
      domPtr->numTerminals += 1;
      domPtr->terminals = (PORT_PTR*) realloc((char*)domPtr->terminals
            ,domPtr->numTerminals*sizeof(*domPtr->terminals));
      domPtr->terminals[nt] = port;
      SetNameData(portName,NULL,(voidptr)port);

     /* Move the port name from the TmpNames hash table to the table containing
      * the list of cell signals.  Note that it is impossible for the signal to
      * be already on the list, since this would mean a duplicate terminal.
      */
      UseHashTable(domPtr->netHashID);
      np = newNet(portName,domPtr);
      np->exit = port;
      port->whichNet = np;
      port->whichModule = domPtr;
      appendNet2Module(np,domPtr);
   } else {
      register att_ptr newAtts = port->termAttribs;
      (void) AddCellPortPin(tp,port->pins[0]);
      /* Merge attributes. */
      while (newAtts != NULL) {
         register att_ptr oldAtts = tp->termAttribs;
         for (;oldAtts!=NULL; oldAtts=oldAtts->next)
            if (strEQ(oldAtts->attName,newAtts->attName))
               break;
         if (oldAtts==NULL) {
            oldAtts = newAtts;
            newAtts = newAtts->next;
            oldAtts->next = tp->termAttribs;
            tp->termAttribs = oldAtts;
         } else
            newAtts = newAtts->next;
      }
   }
   iocount += 1;
   return domPtr;
}

/* This function is called after reading each domain attribute and is
 * passed a pointer to the attribute record created by the general
 * attribute routine InitAttrib.
 * SAMPLE:    domain begin highway scale=0.1
 *                                          |
 *                                called here
 */

/* No need to do anything special with the attribute, so just tack it on.
 */
DOMAIN_PTR AddDomainAttrib(domPtr,ap)
DOMAIN_PTR domPtr;
ATTRIB_PTR ap;
{
   ap->next = domPtr->moduleAttribs;
   domPtr->moduleAttribs = ap;
   return domPtr;
}

/* This function is called after reading a complete cell instance
 * SAMPLE:      a2s INST12 ( sig12, sig32, sig15 ) placement=keep_in_row
 *                                                                      |
 *                                                            called here
 */

/* We add a cell instance to the list of domain submodules and process all
 * nets connected to the cell.  This routine is identical to ading a
 * transistor to a cell except for the difference in the diagnostics.
 */
DOMAIN_PTR AddCellInstance(domPtr,instPtr)
DOMAIN_PTR domPtr;
INST_PTR instPtr;
{
   register char *instName = instPtr->instName;
   register int nsm = domPtr->numSubModules;

   domPtr->numSubModules += 1;
   /* Initialize the hash table of subModules, if necessary */
   if (nsm == 0) {
      domPtr->subModules = (INST_PTR*) MessPtr(sizeof(*domPtr->subModules));
      domPtr->subHashID = UseHashTable(0);
   } else 
      domPtr->subModules = (INST_PTR*) realloc((char*)domPtr->subModules
            ,domPtr->numSubModules*sizeof(*domPtr->subModules));
   /* Check for duplicate instance name */
   UseHashTable(domPtr->subHashID);
   if (GetNameData(instName) != NULL) {
      ErrMessage("FATAL ERROR - Duplicate cell instance '%s' in domain '%s'"
         ,instName,domPtr->moduleName);
      exit(1);
   }
   /* Add the instance */
   SetNameData(instName,&instPtr->instName,(voidptr) instPtr);
   domPtr->subModules[nsm] = instPtr;

   /* Process the nets */
   joinSubmoduleNets(domPtr,instPtr);

   return domPtr;
}
/* This comment must be left in the end of this file so that we can have
 * sane tab spacing in vi: set ts=3 sw=3:
 */
