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
#define Y_InitAttrib(attType,attName,attValStr)				\
	(int)InitAttrib(attType,(char*)attName,(char*)attValStr);
#define Y_InitPointAttrib(attName,x,y)				\
	(int)InitPointAttrib((char*)attName,x,y)
#define Y_AddCellAttrib(cell,att)					\
	(int)AddCellAttrib((CELL_PTR)cell,(ATTRIB_PTR)att);
#define Y_AddCellEquiv(cell,equiv)					\
	(int)AddCellEquiv((CELL_PTR)cell,(EQUIV_PTR)equiv);
#define Y_AddCellPort(cell,port)					\
	(int)AddCellPort((CELL_PTR)cell,(PORT_PTR)port);
#define Y_AddCellProfile(cell,prof,whichProf,prName)			\
	(int)AddCellProfile((CELL_PTR)cell,(PROFILE_PTR)prof,whichProf,(char*)prName);
#define Y_AddCellSignal(cell,sigName)					\
	(int)AddCellSignal((CELL_PTR)cell,(char*)sigName);
#define Y_AddXtor(cell,xtor)						\
	(int)AddXtor((CELL_PTR)cell,(XTOR_PTR)xtor);
#define Y_InitCell(cellName)						\
	(int)InitCell((char*)cellName);
#define Y_InitCellSigList(cell)						\
	(int)InitCellSigList((CELL_PTR)cell);
#define Y_InitXtorList(cell)						\
	(int)InitXtorList((CELL_PTR)cell);
#define Y_WrapUpCell(cell,cellName)					\
	(int)WrapUpCell((CELL_PTR)cell,(char*)cellName);
#define Y_WrapUpCellSiglist(cell)					\
	(int)WrapUpCellSiglist((CELL_PTR)cell);
#define Y_WrapUpXtors(cell)						\
	(int)WrapUpXtors((CELL_PTR)cell);
#define Y_AddEquiv(equiv,eqMember)					\
	(int)AddEquiv((EQUIV_PTR)equiv,(char*)eqMember);
#define Y_InitEquiv(eqClass,eqMember)					\
	(int)InitEquiv((char*)eqClass,(char*)eqMember);
#define Y_WrapUpEquiv(equiv)						\
	(int)WrapUpEquiv((EQUIV_PTR)equiv);
#define Y_AddCellPinAttrib(pin,att)					\
	(int)AddCellPinAttrib((PIN_PTR)pin,(ATTRIB_PTR)att);
#define Y_InitCellPin(xmin,xmax,ymin,ymax)				\
	(int)InitCellPin(xmin,xmax,ymin,ymax);
#define Y_AddCellPortAttrib(port,att)					\
	(int)AddCellPortAttrib((PORT_PTR)port,(ATTRIB_PTR)att);
#define Y_AddCellPortPin(port,pin)					\
	(int)AddCellPortPin((PORT_PTR)port,(PIN_PTR)pin);
#define Y_InitCellPort(portName)					\
	(int)InitCellPort((char*)portName);
#define Y_WrapUpCellPort(port)						\
	(int)WrapUpCellPort((PORT_PTR)port);
#define Y_WrapUpCellPortPins(port)					\
	(int)WrapUpCellPortPins((PORT_PTR)port);
#define Y_AppendProfilePoint(prof,x,y)					\
	(int)AppendProfilePoint((PROFILE_PTR)prof,x,y);
#define Y_InitProfile(x,y)						\
	(int)InitProfile(x,y);
#define Y_AddXtorAttrib(xtor,att)					\
	(int)AddXtorAttrib((XTOR_PTR)xtor,(ATTRIB_PTR)att);
#define Y_InitCellXtor(xName,gate,source,drain)				\
	(int)InitCellXtor((char*)xName,(char*)gate,(char*)source,(char*)drain);
#define Y_AddCellInstance(domPtr,instPtr)				\
	(int)AddCellInstance((DOMAIN_PTR)domPtr,(INST_PTR)instPtr)
#define Y_AddDomainAttrib(domPtr,att)					\
	(int)AddDomainAttrib((DOMAIN_PTR)domPtr,(ATTRIB_PTR)att)
#define Y_AddDomainPort(domPtr,portPtr)					\
	(int)AddDomainPort((DOMAIN_PTR)domPtr,(DPORT_PTR)portPtr)
#define Y_AddDomainProfile(domPtr,profPtr,whichProf,prname)			\
	(int)AddDomainProfile((DOMAIN_PTR)domPtr,(PROFILE_PTR)profPtr,whichProf,(char*)prname)
#define Y_InitDomain(domName)						\
	(int)InitDomain((char*)domName)
#define Y_WrapUpDomain(domPtr,domName)					\
	(int)WrapUpDomain((DOMAIN_PTR)domPtr,(char*)domName)
#define Y_AddDomainPortAttrib(portPtr,att)				\
	(int)AddDomainPortAttrib((DPORT_PTR)portPtr,(ATTRIB_PTR)att)
#define Y_InitDomainPort(sigName,sigSide,from,to)			\
	(int)InitDomainPort((char*)sigName,(char*)sigSide,from,to)
#define Y_AddInstanceAttrib(instPtr,att)				\
	(int)AddInstanceAttrib((INST_PTR)instPtr,(ATTRIB_PTR)att)
#define Y_AddInstanceNet(instPtr,sigName)				\
	(int)AddInstanceNet((INST_PTR)instPtr,(char*)sigName)
#define Y_InitCellInst(parent,ident,sigName)				\
	(int)InitCellInst((char*)parent,(char*)ident,(char*)sigName)
#define Y_WrapUpInstance(instPtr)					\
	(int)WrapUpInstance((INST_PTR)instPtr)
