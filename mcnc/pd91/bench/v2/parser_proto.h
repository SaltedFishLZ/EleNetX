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
#ifdef ANSI_PROTO
ATTRIB_PTR	InitAttrib(int,char*,char*);
ATTRIB_PTR	InitPointAttrib(char*,int,int);
CELL_PTR	AddCellAttrib(CELL_PTR,ATTRIB_PTR);
CELL_PTR	AddCellEquiv(CELL_PTR,EQUIV_PTR);
CELL_PTR	AddCellPort(CELL_PTR,PORT_PTR);
CELL_PTR	AddCellProfile(CELL_PTR,PROFILE_PTR,int,char*);
CELL_PTR	AddXtor(CELL_PTR,XTOR_PTR);
CELL_PTR	InitCell(char*);
CELL_PTR	InitCellSigList(CELL_PTR);
CELL_PTR	InitXtorList(CELL_PTR);
CELL_PTR	WrapUpCell(CELL_PTR,char*);
CELL_PTR	WrapUpCellSiglist(CELL_PTR);
CELL_PTR	WrapUpXtors(CELL_PTR);
CELL_PTR	AddCellSignal(CELL_PTR,char*);
EQUIV_PTR	AddEquiv(EQUIV_PTR,char*);
EQUIV_PTR	InitEquiv(char*,char*);
EQUIV_PTR	WrapUpEquiv(EQUIV_PTR);
PIN_PTR		AddCellPinAttrib(PIN_PTR,ATTRIB_PTR);
PIN_PTR		InitCellPin(int,int,int,int);
PORT_PTR	AddCellPortAttrib(PORT_PTR,ATTRIB_PTR);
PORT_PTR	AddCellPortPin(PORT_PTR,PIN_PTR);
PORT_PTR	InitCellPort(char*);
PORT_PTR	WrapUpCellPort(PORT_PTR);
PORT_PTR	WrapUpCellPortPins(PORT_PTR);
PROFILE_PTR	AppendProfilePoint(PROFILE_PTR,int,int);
PROFILE_PTR	InitProfile(int,int);
XTOR_PTR	AddXtorAttrib(XTOR_PTR,ATTRIB_PTR);
XTOR_PTR	InitCellXtor(char*,char*,char*,char*);
DOMAIN_PTR	WrapUpDomain(DOMAIN_PTR,char*);
DOMAIN_PTR	AddCellInstance(DOMAIN_PTR,INST_PTR);
DOMAIN_PTR	AddDomainPort(DOMAIN_PTR,PORT_PTR);
DOMAIN_PTR	InitDomain(char*);
DOMAIN_PTR	AddDomainAttrib(DOMAIN_PTR,ATTRIB_PTR);
DPORT_PTR	InitDomainPort(char*,char*,int,int);
DPORT_PTR	AddDomainPortAttrib(DPORT_PTR,ATTRIB_PTR);
DOMAIN_PTR	AddDomainProfile(DOMAIN_PTR,PROFILE_PTR,int,char*);
INST_PTR	WrapUpInstance(INST_PTR);
INST_PTR	AddInstanceAttrib(INST_PTR,ATTRIB_PTR);
INST_PTR	InitCellInst(char*,char*,char*);
INST_PTR	AddInstanceNet(INST_PTR,char*);
void ErrMessage(char*,...);

int	atoi(char*);
#else ANSI_PROTO
ATTRIB_PTR	InitAttrib();
ATTRIB_PTR	InitPointAttrib();
CELL_PTR	AddCellAttrib();
CELL_PTR	AddCellEquiv();
CELL_PTR	AddCellPort();
CELL_PTR	AddCellProfile();
CELL_PTR	AddXtor();
CELL_PTR	InitCell();
CELL_PTR	InitCellSigList();
CELL_PTR	InitXtorList();
CELL_PTR	WrapUpCell();
CELL_PTR	WrapUpCellSiglist();
CELL_PTR	WrapUpXtors();
CELL_PTR	AddCellSignal();
EQUIV_PTR	AddEquiv();
EQUIV_PTR	InitEquiv();
EQUIV_PTR	WrapUpEquiv();
PIN_PTR		AddCellPinAttrib();
PIN_PTR		InitCellPin();
PORT_PTR	AddCellPortAttrib();
PORT_PTR	AddCellPortPin();
PORT_PTR	InitCellPort();
PORT_PTR	WrapUpCellPort();
PORT_PTR	WrapUpCellPortPins();
PROFILE_PTR	AppendProfilePoint();
PROFILE_PTR	InitProfile();
XTOR_PTR	AddXtorAttrib();
XTOR_PTR	InitCellXtor();
DOMAIN_PTR	WrapUpDomain();
DOMAIN_PTR	AddCellInstance();
DOMAIN_PTR	AddDomainPort();
DOMAIN_PTR	InitDomain();
DOMAIN_PTR	AddDomainAttrib();
DPORT_PTR	InitDomainPort();
DPORT_PTR	AddDomainPortAttrib();
DOMAIN_PTR	AddDomainProfile();
INST_PTR	WrapUpInstance();
INST_PTR	AddInstanceAttrib();
INST_PTR	InitCellInst();
INST_PTR	AddInstanceNet();
void ErrMessage();

int	atoi();
#endif ANSI_PROTO
