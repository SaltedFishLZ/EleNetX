This file contains:

1) pbenchtypes.h - the types and constants
2) pext.h - the include file giving the procedures and argument types
3) getmodule.p - the parser, with the GetModule routine
4) writemodule.p - the routine to output a module
5) pptest.p - a simple main routine to read and write modules
6) makefile - to make ptest
7) pparse.doc - a short file explaining how to use GetModule, WriteModule

All files are separated by lines of #####'s and the file's name.
################################################################################
pbenchtypes.h
################################################################################
{ pbenchtypes.h contains the data types, structures, and constants
   for the Pascal-language parser of the standard placement program input
   format specified by Preas and Roberts.


   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.

}



{Constants Section - all grouped together in accordance with Pascal's wishes }

const MAXNAMESIZE = 100;
      KEYWORDSIZE = 15;

      {Module Type Constants}

      STANDARD    = 1;
      PAD =         2;
      GENERAL =     3;
      PARENT =      4;
      FEEDTHROUGH = 5;
      ENDFILE =     6;

      { Unspecified Position and width on terminal constants }

      NOPOSITION = -1.0;
      NOWIDTH =    -1.0;


      {IO Pin type constants}

      I =  1;
      O =  2;
      B =  3;
      PI = 4;
      PO = 5;
      PB = 6;
      F =  7;

      {Pin side constants}
      BOTTOM = 1;
      RIGHT =  2;
      TOP =    3;
      LEFT =   4;
      NOSIDE = 5;

      {Layer type constants}
      PDIFF =  1;
      NDIFF =  2;
      POLY =   3;
      METAL1 = 4;
      METAL2 = 5;
      NOLAYER = 6;


      {Reflection Type Constants}
      RFLNONE = 1;
      RFLY =    2;

      {Rotation Type Constants} 

      ROT0 =    1;
      ROT90 =   2;
      ROT180 =  3;
      ROT270 =  4;




{ Character strings that define the key words and comments }

{ Comment Delimiters }

      STARTCOMMENT = '/*             ';
      ENDCOMMENT =   '*/             ';

{ End of entry delimeter }

      LineTerminator = ';';

{ Module Parameters }

      ModuleKeyword =       'MODULE         ';
      EndModuleKeyword =    'ENDMODULE      ';
      TypeKeyword =         'TYPE           ';
      WidthKeyword =        'WIDTH          ';
      HeightKeyword =       'HEIGHT         ';

{ IOList Delimiters }
      IOListKeyword =       'IOLIST         ';
      EndIOListKeyword =    'ENDIOLIST      ';

{ Network Delimiters   }
      NetworkKeyword =      'NETWORK        ';
      EndNetworkKeyword =   'ENDNETWORK     ';

{ Placement Delimiters }
      PlacementKeyword =    'PLACEMENT      ';
      EndPlacementKeyword = 'ENDPLACEMENT   ';

{ Module Types }
      StandardKeyword =     'STANDARD       ';
      PadKeyword =          'PAD            ';
      GeneralKeyword =      'GENERAL        ';
      ParentKeyword =       'PARENT         ';
      FeedthroughKeyword =  'FEEDTHROUGH    ';

{ IOList Terminal Types }
      InputTerminal =       'I              ';
      OutputTerminal =      'O              ';
      BiDirectionTerminal = 'B              ';
      PadInputTerminal =    'PI             ';
      PadOutputTerminal =   'PO             ';
      PadBiTerminal =       'PB             ';
      FeedThroughTerminal = 'F              ';

{ Side Types }
      BottomSide = 'BOTTOM         ';
      RightSide  = 'RIGHT          ';
      TopSide    = 'TOP            ';
      LeftSide   = 'LEFT           ';

{ Layer Types }
      PDiffLayer  =  'PDIFF          ';
      NDiffLayer  =  'NDIFF          ';
      PolyLayer   =  'POLY           ';
      Metal1Layer =  'METAL1         ';
      Metal2Layer =  'METAL2         ';

{ Reflections }
      NoReflection = 'RFLNONE        ';
      YReflection  = 'RFLY           ';

{ Rotations }
      Rot0String   = 'ROT0           ';
      Rot90String  = 'ROT90          ';
      Rot180String = 'ROT180         ';
      Rot270String = 'ROT270         ';




{Types Definitions}


type  NameType = packed array [1..MAXNAMESIZE] of char;

{ Keyword Type is a  smaller size due to the need in Pascal to pad blanks}

      KeywordType = packed array [1..KEYWORDSIZE] of char;


     ModuleType = STANDARD .. ENDFILE;

     Number = real;

     TerminalType = I .. F;

     SideType = BOTTOM .. NOSIDE;

     LayerType = PDIFF .. NOLAYER;


IOListPointer = ^IOList;

IOList = record
    SignalName: NameType; 
    Terminal: TerminalType;
    Side:SideType; 
    Position: Number;
    Width: Number; 
    Layer: LayerType;
    Link: IOListPointer;
end;

SignalListPointer = ^SignalList;

SignalList = record
    SignalName: NameType; 
    Link:SignalListPointer;
end;


NetworkListPointer = ^NetworkList;

NetworkList = record
    InstanceName: NameType; 
    ModuleName: NameType; 
    SignalListHead: SignalListPointer; 
    Link: NetworkListPointer;
end;

     ReflectionType = RFLNONE .. RFLY;
     RotationType = ROT0 .. ROT270 ;


PlacementListPointer = ^PlacementList;

PlacementList = record
    InstanceName: NameType; 
    XLocation: Number; 
    YLocation: Number; 
    Reflection: ReflectionType; 
    Rotation: RotationType; 
    Link: PlacementListPointer;
end;

################################################################################
pext.h
################################################################################
Procedure GetModule(var TInputFile: text; 
		    var TName: NameType;
		    var TModType: ModuleType;
		    var TWidth: Number;
		    var THeight: Number; 
		    var TIOListHead: IOListPointer; 
		    var TNetworkListHead: NetworkListPointer; 
		    var TPlacementListHead: PlacementListPointer;
		    var TLineNumber: Integer);
		    external;

Procedure WriteModule(var WOutputFile: text;
		      WName: NameType; 
		      WModType: ModuleType; 
		      WWidth: Number; 
		      WHeight: Number; 
		      WIOListHead: IOListPointer; 
		      WNetworkListHead: NetworkListPointer; 
		      WPlacementListHead: PlacementListPointer);
		      external;

################################################################################
getmodule.p
################################################################################
{ Getmodule - routines for parsing the standard placement input form
   specified by Preas and Roberts.

   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
}



#include "pbenchtypes.h"

{ Constants  }

const FATAL = 1;
      WARNING = 2;


      BLANK   = ' ';
      TAB     = '	';

      EOFConst = -1;

{ Global Variables }

var
	InputField: NameType;

	EOFDetected: Boolean;
	ExpectingEOF: Boolean; 
	LineTerminated: Boolean; 

	LLineNumber: Integer;

#include "pext.h"

{ Support Routines  }


{ Error is called when an exception occurs - FATAL ones cause
   an halt, while WARNINGS are just printed and execution continues } 

Procedure Error(EString: NameType;  ErrorType: Integer);

begin
    if ErrorType = WARNING then write('Warning: ')
    else  write('Fatal Error: ');

    writeln(EString,' on line ',LLineNumber,' of input file');

    if ErrorType = FATAL then halt;
end;



{ kstrequal returns true if the two strings are the same, false otherwise }

Function kstrequal(s1: KeywordType;s2: NameType): Boolean;

var j: Integer;
begin

    j := 1;
    kstrequal := true;

    while  (s1[j] = s2[j]) and (s1[j] <> BLANK) and (s2[j] <> BLANK)
	do j := j + 1;
    
    if s1[j] <> s2[j] then kstrequal := false;
end;


{ nstrcpy copies one name type into another }

Procedure nstrcpy(var s1: NameType; s2: NameType);
    var j: Integer;

begin
    j := 1;

    while j <= MAXNAMESIZE do begin

	s1[j] := s2[j];
	j := j + 1;
    end;
end;

{ nstrlen returns the length of the first string on nonblank characters
  in the NameType string }

Function nstrlen(s1: NameType): Integer;

var j: Integer;

begin

    j := 1;
    while not ((s1[j] = BLANK) or (s1[j] = Chr(0)))  do j := j + 1;

    nstrlen := j - 1;

end;


{ ScanField picks up the next field, whilst counting the number of
   lines that go by }

Function ScanField (var InputFile: text; var SText:NameType): Integer;

    var NextChar: char; 
        TextP: integer; 

    { Skip over blanks, tabs and newlines, counting newlines }

begin

    if eof(InputFile) then 
	ScanField := EOFConst
    else begin

	read(InputFile,NextChar);
	if not eof(InputFile) then
	    if eoln(InputFile) then LLineNumber := LLineNumber + 1; 

	While ((NextChar = BLANK) or (NextChar = TAB)) and (not eof(InputFile))
	do  begin 
	
	    read(InputFile,NextChar);
	    if not eof(InputFile) then
		if eoln(InputFile) then LLineNumber := LLineNumber + 1; 
	end; 
	
	if eof(InputFile) then 
	    ScanField := EOFConst
	else begin

	    TextP := 1;
	    SText[TextP] := NextChar;
	    TextP := TextP + 1;

	    { Collect Characters until a white space or end of file }

	    read(InputFile,NextChar);
	    if not eof(InputFile) then
		if eoln(InputFile) then LLineNumber := LLineNumber + 1;

	    while (NextChar <> BLANK) and (NextChar <>  TAB) and
		   not eof(InputFile) do  begin

		SText[TextP] := NextChar;
		TextP := TextP + 1;
		read(InputFile,NextChar);
		if not eof(InputFile) then
		    if eoln(InputFile) then LLineNumber := LLineNumber + 1;
	    end;

	    SText[TextP] := BLANK;

	    ScanField := 1;

	end;
    end;
end;


{ GetField reads a field from the input stream.  A field is any set of
   characters delimited by blanks, tabs or newlines.  }

Procedure GetField(var IFile: text; var GText: NameType);
    var ErrFlag: integer; 

begin
    ErrFlag := ScanField(IFile,GText);

    if (ErrFlag = EOFConst) and (not ExpectingEOF) then
	Error('Unexpected End of File',FATAL)

    else if (ErrFlag = EOFConst) and ExpectingEOF then
	EOFDetected := true;
end;


{ ReadField reads a field from the input stream, ignoring comments }

Procedure ReadField (var IFile: text; var RText: NameType);
    var FieldLength: integer; 
begin

    GetField(IFile,RText);

    while kstrequal(STARTCOMMENT,RText) do begin 

	{ get fields until end of comment }

	GetField(IFile,RText);

	while  not kstrequal(ENDCOMMENT,RText) do begin
	    if EOFDetected then Error('Non-terminated Comment',FATAL);
	    GetField(IFile,RText);
	end;
	GetField(IFile,RText);
    end;

    { Strip off the line terminator if it is attached to the field,
       and set end of logical line indicator }

    FieldLength := nstrlen(RText);

    if RText[FieldLength] = LineTerminator then  begin
	LineTerminated := true;
	RText[FieldLength] := BLANK;
    end;
end;


{ ConvertToNumber does the conversion from ascii to a floating point
   number }

Function  ConvertToNumber(CText: NameType): Number;

    var CharPosition: Integer;
	SubTotal: Number;
	ch: Char;
	NewDigit: Number;
	DivFactor: Number;

begin

    SubTotal := 0.0;
    CharPosition := 1;
    
    ch := CText[CharPosition];
    while (ch <> BLANK) and (ch <> '.') do begin

	if (Ord(ch) < Ord('0')) or (Ord(ch) > Ord('9')) then

	    Error('Illegal Number specified',FATAL)
	else begin
	    NewDigit := Ord(ch) - Ord('0');
	    SubTotal := (SubTotal * 10.0) + NewDigit;
	end;

	CharPosition := CharPosition + 1;
	ch := CText[CharPosition];
    end;

    DivFactor := 10.0;
    if ch = '.' then begin
	CharPosition := CharPosition + 1;
	ch := CText[CharPosition];
	while ch <> BLANK do begin

	    if (Ord(ch) < Ord('0')) or (Ord(ch) > Ord('9')) then
		Error('Illegal Number specified',FATAL)
	    else begin
		NewDigit := Ord(ch) - Ord('0');
		SubTotal := SubTotal + NewDigit / DivFactor;
		DivFactor := DivFactor * 10;
	    end;

	    CharPosition := CharPosition + 1;
	    ch := CText[CharPosition];
	end;
    end;

    ConvertToNumber := SubTotal;
end;

{ Get Line Terminator looks for the line terminator as the next
   field if it wasn't already encountered }

Procedure GetLineTerminator(var IFile: text);

    var TermField: NameType;

begin
    if LineTerminated  then 
	LineTerminated := false
    else begin
	ReadField(IFile,TermField);

	if LineTerminated then LineTerminated := false
	else Error('Missing Line Terminator (;)',FATAL);
    end;
end;



{ GetIOList reads in the IO List  }

Procedure GetIOList(var IFile: text; var IOListHead: IOListPointer);

    var IOField: NameType; 
        EndDetected: Boolean; 

        NewIOPin:   IOListPointer; 
        IOListTail: IOListPointer; 

begin
    IOListHead := nil;
    
    GetLineTerminator(IFile);

    { Main loop to read each pin  }

    EndDetected := false;
    while not EndDetected  do begin

	{ Read Signal Name or end of IO List }

	ReadField(IFile,IOField);

        if kstrequal(EndIOListKeyword,IOField) then begin
	    EndDetected := true;
	    GetLineTerminator(IFile);
	end

	else begin  { Create a New IO Pin }

	    new(NewIOPin);

	    NewIOPin^.Side     := NOSIDE;
	    NewIOPin^.Position := NOPOSITION;
	    NewIOPin^.Layer    := NOLAYER;
	    NewIOPin^.Width    := NOWIDTH;
	    NewIOPin^.Link     := nil;

	    nstrcpy(NewIOPin^.SignalName,IOField);

	    { Read Terminal Type }

	    ReadField(IFile,IOField);


	    if kstrequal(InputTerminal,IOField) then
		NewIOPin^.Terminal := I
	    else if kstrequal(OutputTerminal,IOField) then
		NewIOPin^.Terminal := O
	    else if kstrequal(BiDirectionTerminal,IOField) then
		NewIOPin^.Terminal := B
	    else if kstrequal(PadInputTerminal,IOField) then
		NewIOPin^.Terminal := PI
	    else if kstrequal(PadOutputTerminal,IOField) then
		NewIOPin^.Terminal := PO
	    else if kstrequal(PadBiTerminal,IOField) then
		NewIOPin^.Terminal := PB
	    else if kstrequal(FeedThroughTerminal,IOField) then
		NewIOPin^.Terminal := F

	    else Error('Unknown Terminal Type Specification',FATAL);

	    if not LineTerminated then begin

		ReadField(IFile,IOField);

		if (not LineTerminated) or (LineTerminated and 
			(nstrlen(IOField) <> 0)) then begin 

		    if kstrequal(BottomSide,IOField) then
			NewIOPin^.Side := BOTTOM
		    else if kstrequal(RightSide,IOField) then
			NewIOPin^.Side := RIGHT
		    else if kstrequal(TopSide,IOField)  then
			NewIOPin^.Side := TOP
		    else if kstrequal(LeftSide,IOField) then
			NewIOPin^.Side := LEFT
		    
		    else Error('Unknown Side Type Specification',FATAL);

		    if not LineTerminated then begin

			ReadField(IFile,IOField);

			if (not LineTerminated) or
			    (LineTerminated and (nstrlen(IOField) <> 0)) then 
			    begin 

			    NewIOPin^.Position := ConvertToNumber(IOField);

			    if not LineTerminated then begin

				ReadField(IFile,IOField);

				if (not LineTerminated) or
				    (LineTerminated and (nstrlen(IOField) <> 0))
				    then begin 

				    NewIOPin^.Width := ConvertToNumber(IOField);
				    if not LineTerminated then begin

					ReadField(IFile,IOField);

					if (not LineTerminated) or
					    (LineTerminated and 
					    (nstrlen(IOField) <> 0)) then begin 

					  if kstrequal(PDiffLayer,IOField) then
						NewIOPin^.Layer := PDIFF
					  else if kstrequal(NDiffLayer,IOField) 
						then
						NewIOPin^.Layer := NDIFF
					  else if kstrequal(PolyLayer,IOField) 
					        then
						NewIOPin^.Layer := POLY
					  else if kstrequal(Metal1Layer,IOField)
					        then
						NewIOPin^.Layer := METAL1
					  else if kstrequal(Metal2Layer,IOField)
						then
						NewIOPin^.Layer := METAL2
					  
					  else Error('Unknown Layer Specification', FATAL);

					  GetLineTerminator(IFile);
					end;
				    end;
				end;
			    end;
			end;
		    end;
		end;
	    end;

	    { Link the new IO Pin }

	    if IOListHead = nil then
		IOListHead := NewIOPin
	    else
		IOListTail^.Link := NewIOPin;
	    
	    IOListTail := NewIOPin;

	    LineTerminated := false;

	end;
    end;
end;


Procedure GetNetworkList(var IFile: text; var NetworkListHead: NetworkListPointer);
    var NetworkField: NameType;
        EndDetected: Boolean; 
	SignalListEnd: Boolean; 
	NewNetworkEntry, NetworkListTail: NetworkListPointer; 
	NewSignal, SignalListTail: SignalListPointer; 

    
begin 
    NetworkListHead := nil;
    GetLineTerminator(IFile);

    { Main loop to read each Network Entry  }

    EndDetected := false;
    while not EndDetected do begin

	{ Read Instance Name or end of Network List }

	ReadField(IFile,NetworkField);

        if kstrequal(EndNetworkKeyword,NetworkField) then begin
	    EndDetected := true;
	    GetLineTerminator(IFile);
	end

	else begin  { Create a New Network Entry }

	    new(NewNetworkEntry);

            NewNetworkEntry^.SignalListHead := nil;
            NewNetworkEntry^.Link           := nil;

	    nstrcpy(NewNetworkEntry^.InstanceName,NetworkField);

	    { Read Module Name }

	    ReadField(IFile,NewNetworkEntry^.ModuleName);

	    { Get the signal names and link onto this entry's list }

	    SignalListEnd := false;

	    while not SignalListEnd do begin

		if LineTerminated then
		    SignalListEnd := true
		
		else begin
		    ReadField(IFile,NetworkField);

		    if LineTerminated and (nstrlen(NetworkField) = 0) then
			SignalListEnd := true
		    
		    else begin

			{ Create a New Signal }

			new(NewSignal);
			NewSignal^.Link := nil;
			nstrcpy(NewSignal^.SignalName,NetworkField);

			{ Link onto this entry's list }

			if NewNetworkEntry^.SignalListHead = nil then
			    NewNetworkEntry^.SignalListHead := NewSignal
			else
			    SignalListTail^.Link := NewSignal;

			SignalListTail := NewSignal;
	            end;

		end;

	    end;

	    { Link the new NetworkEntry }

	    if NetworkListHead = nil then 
		NetworkListHead := NewNetworkEntry
	    else
		NetworkListTail^.Link := NewNetworkEntry;
	    
	    NetworkListTail := NewNetworkEntry;

	    LineTerminated := false;
	end;
    end;
end;

Procedure GetPlacementList(var IFile: text; var PlacementListHead: PlacementListPointer);

    var PlacementField: NameType;
	EndDetected: Boolean; 
	NewPlacementEntry, PlacementListTail: PlacementListPointer; 

begin
    PlacementListHead := nil;
    
    GetLineTerminator(IFile);

    { Main loop to read each Placement Entry  }

    EndDetected := false;
    while not EndDetected do begin

	{ Read Instance Name or end of Placement List }

	ReadField(IFile,PlacementField);

        if kstrequal(EndPlacementKeyword,PlacementField) then begin
	    EndDetected := true;
	    GetLineTerminator(IFile);
	end

	else begin  { Create a New Placement Entry }

	    new(NewPlacementEntry);

            NewPlacementEntry^.Reflection := RFLNONE;
            NewPlacementEntry^.Rotation  := ROT0;
            NewPlacementEntry^.Link      := nil;

	    nstrcpy(NewPlacementEntry^.InstanceName,PlacementField);

	    { Read X Location }

	    ReadField(IFile,PlacementField);

            NewPlacementEntry^.XLocation := ConvertToNumber(PlacementField);

	    { Read Y Location }

	    ReadField(IFile,PlacementField);

            NewPlacementEntry^.YLocation := ConvertToNumber(PlacementField);

	    { Check for End of Line or reflections, rotations }

	    while not LineTerminated do begin
		ReadField(IFile,PlacementField);

		if  not (LineTerminated and (nstrlen(PlacementField) = 0))
	   	    then begin

		    if kstrequal(NoReflection,PlacementField) then
			NewPlacementEntry^.Reflection := RFLNONE

		    else if kstrequal(YReflection,PlacementField) then
			NewPlacementEntry^.Reflection := RFLY

		    else if kstrequal(Rot0String,PlacementField) then
			NewPlacementEntry^.Rotation := ROT0

		    else if kstrequal(Rot90String,PlacementField) then
			NewPlacementEntry^.Rotation := ROT90

		    else if kstrequal(Rot180String,PlacementField) then
			NewPlacementEntry^.Rotation := ROT180

		    else if kstrequal(Rot270String,PlacementField) then
			NewPlacementEntry^.Rotation := ROT270

		    else Error('Unknown Rotation or Reflection type',FATAL);
		end;
	    end;

	    LineTerminated := false;

	    { Link the new placement entry }

	    if PlacementListHead = nil then
		PlacementListHead := NewPlacementEntry
	    else
		PlacementListTail^.Link := NewPlacementEntry;
	    
	    PlacementListTail := NewPlacementEntry;
	end;
    end;
end;



{ This is the main routine, called to get the next module.  Depending on the
   type of module, the Network List and the Placement List may or may not
   be empty.  }

Procedure GetModule;
begin

    LineTerminated := false;
    EOFDetected := false;
    LLineNumber := TLineNumber;

    { Set all the optional return arguments to Null at start }

    TWidth  := 0.0;
    THeight := 0.0;

    TIOListHead        := nil;
    TNetworkListHead   := nil;
    TPlacementListHead := nil;


    { Get Module Declaration, return with ENDFILE type if end of file }

    ExpectingEOF := true;
    ReadField(TInputFile,InputField);

    if EOFDetected then 
	TModType := ENDFILE

    else if not kstrequal(ModuleKeyword,InputField) then
	Error('Missing Module Declaration', FATAL)

    else begin
    
    ExpectingEOF := false;
    ReadField(TInputFile,TName);

    GetLineTerminator(TInputFile);

    { Get Type }

    ReadField(TInputFile,InputField);

    if not kstrequal(TypeKeyword,InputField) then
	Error('Missing Type Declaration - Must be after MODULE', FATAL)
    else
    
    ReadField(TInputFile,InputField);

    if kstrequal(StandardKeyword,InputField)         then TModType := STANDARD
    else if kstrequal(PadKeyword,InputField)         then TModType := PAD
    else if kstrequal(GeneralKeyword,InputField)     then TModType := GENERAL
    else if kstrequal(ParentKeyword,InputField)      then TModType := PARENT
    else if kstrequal(FeedthroughKeyword,InputField) then TModType := FEEDTHROUGH

    else Error('Unknown Module Type Specification',FATAL);
    
    GetLineTerminator(TInputFile);

    { Loop to pick up the next field and process it - either
       width, height, IOList, NetworkList or PlacementList }

    ReadField(TInputFile,InputField);

    while  not (kstrequal(EndModuleKeyword,InputField)) do begin

	if kstrequal(WidthKeyword,InputField) then begin
	    
	    ReadField(TInputFile,InputField);

	    TWidth := ConvertToNumber(InputField);
	
	    GetLineTerminator(TInputFile);
	end

	else if kstrequal(HeightKeyword,InputField) then begin
	
	    ReadField(TInputFile,InputField);

	    THeight := ConvertToNumber(InputField);
	    
	    GetLineTerminator(TInputFile);
	end

	else if kstrequal(IOListKeyword,InputField) then

	    GetIOList(TInputFile,TIOListHead)

	else if kstrequal(NetworkKeyword,InputField) then

	    GetNetworkList(TInputFile,TNetworkListHead)

	else if kstrequal(PlacementKeyword,InputField) then

	    GetPlacementList(TInputFile,TPlacementListHead)

	else Error('Unknown Keyword',FATAL);

	ReadField(TInputFile,InputField);
    end;

    GetLineTerminator(TInputFile);

    end;
    { Update LineNumber }
    TLineNumber := LLineNumber;
end;

################################################################################
writemodule.p
################################################################################
{ Writemodule - routines for printing out a module in the format
   specified by Preas and Roberts.  
 
   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
 }


#include 'pbenchtypes.h'
#include 'pext.h'

Procedure PrintKeyword(var OFile: text; Keyword: KeywordType);

const BLANK = ' ';
var CharPosition: Integer;

Begin
    
    CharPosition := 1;

    while not ((Keyword[CharPosition] = BLANK) or 
	       (Keyword[CharPosition] = Chr(0))) do begin
    
	write(OFile,Keyword[CharPosition]);

	CharPosition := CharPosition + 1;
    end;
end;


Procedure PrintName(var OFile: text; Name: NameType);

const BLANK = ' ';
var CharPosition: Integer;

Begin
    
    CharPosition := 1;

    while not ((Name[CharPosition] = BLANK) or 
	       (Name[CharPosition] = Chr(0))) do begin
    
	write(OFile,Name[CharPosition]);

	CharPosition := CharPosition + 1;
    end;
end;


Procedure WriteModule;

var IOListPin: IOListPointer; 
    CurrentNetworkEntry: NetworkListPointer; 
    CurrentSignal: SignalListPointer; 
    CurrentPlacementEntry: PlacementListPointer; 

begin


    writeln(WOutputFile);
    PrintKeyword (WOutputFile, ModuleKeyWord);
    write(WOutputFile,' ');
    PrintName (WOutputFile, WName);
    writeln(WOutputFile,';');

    write(WOutputFile,'   ');
    PrintKeyword(WOutputFile,TypeKeyword);
    write(WOutputFile,' ');

    if WModType = STANDARD then
	PrintKeyword(WOutputFile,StandardKeyword)

    else if WModType = PAD then 
	PrintKeyword(WOutputFile,PadKeyword)

    else if WModType = GENERAL then
	PrintKeyword(WOutputFile,GeneralKeyword)

    else if WModType = PARENT then
	PrintKeyword(WOutputFile,ParentKeyword)

    else if WModType = FEEDTHROUGH then
	PrintKeyword(WOutputFile,FeedthroughKeyword)

    else 
	writeln(WOutputFile,'UNKNOWN!!!');

    writeln(WOutputFile,';');

    if (WWidth <> 0.0) then begin
	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,WidthKeyword);
	writeln(WOutputFile,' ',WWidth:1:1,';');
    end;

    if (WHeight <> 0.0) then begin
	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,HeightKeyword);
	writeln(WOutputFile,' ',WHeight:1:1,';');
    end;

    IOListPin := WIOListHead;

    if (WIOListHead <> nil) then begin
	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,IOListKeyword);
	writeln(WOutputFile,';');
    end;

    while (IOListPin <> nil) do begin
	
	write(WOutputFile,'      ');
	PrintName(WOutputFile,IOListPin^.SignalName);
	write(WOutputFile,' ');

	if (IOListPin^.Terminal = I ) then
	    PrintKeyword(WOutputFile,InputTerminal)

	else if (IOListPin^.Terminal = O) then
	    PrintKeyword(WOutputFile,OutputTerminal)

	else if (IOListPin^.Terminal = B) then
	    PrintKeyword(WOutputFile,BiDirectionTerminal)

	else if (IOListPin^.Terminal = PI) then
	    PrintKeyword(WOutputFile,PadInputTerminal)

	else if (IOListPin^.Terminal = PO) then
	    PrintKeyword(WOutputFile,PadOutputTerminal)

	else if (IOListPin^.Terminal = PB) then
	    PrintKeyword(WOutputFile,PadBiTerminal)

	else if (IOListPin^.Terminal = F) then
	    PrintKeyword(WOutputFile,FeedThroughTerminal)

	else
	    write(WOutputFile,'UNKNOWN!!! ');

	if (IOListPin^.Side <> NOSIDE) then begin

	    if  (IOListPin^.Side = BOTTOM) then begin
	      write(WOutputFile,' ');
	      PrintKeyword(WOutputFile,BottomSide);
	    end

	    else if  (IOListPin^.Side = RIGHT) then begin
	      write(WOutputFile,' ');
	      PrintKeyword(WOutputFile,RightSide);
	    end

	    else if  (IOListPin^.Side = TOP) then begin
	      write(WOutputFile,' ');
	      PrintKeyword(WOutputFile,TopSide);
	    end

	    else if  (IOListPin^.Side = LEFT) then begin
	      write(WOutputFile,' ');
	      PrintKeyword(WOutputFile,LeftSide);
	    end

	    else
	      write(WOutputFile,'UNKNOWN!!!');


	    if (IOListPin^.Position <> NOPOSITION) then begin
		write(WOutputFile,' ',IOListPin^.Position:1:1);

		if (IOListPin^.Width <> NOWIDTH) then begin
		    write(WOutputFile,' ',IOListPin^.Width:1:1);

		    if (IOListPin^.Layer <> NOLAYER) then begin

			write(WOutputFile,' ');

			if (IOListPin^.Layer = PDIFF) then 
			    PrintKeyword(WOutputFile, PDiffLayer)

			else if (IOListPin^.Layer = NDIFF) then 
			    PrintKeyword(WOutputFile, NDiffLayer)

			else if (IOListPin^.Layer = POLY) then 
			    PrintKeyword(WOutputFile,PolyLayer)

			else if (IOListPin^.Layer = METAL1) then 
			    PrintKeyword(WOutputFile,Metal1Layer)

			else if (IOListPin^.Layer = METAL2) then 
			    PrintKeyword(WOutputFile,Metal2Layer)

			else 
			    PrintKeyword(WOutputFile,'UNKNOWN LAYER!!');
		    end;
		end;
	    end;
	end;


	writeln(WOutputFile,';');
	IOListPin := IOListPin^.Link;
    end;

    if (WIOListHead <> nil)  then begin
	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,EndIOListKeyword);
	writeln(WOutputFile,';');
    end;
    
    if (WNetworkListHead <> nil) then begin

	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,NetworkKeyword);
	writeln(WOutputFile,';');

	CurrentNetworkEntry := WNetworkListHead;

	while (CurrentNetworkEntry <> nil) do begin

	    write(WOutputFile,'     ');
	    PrintName(WOutputFile,CurrentNetworkEntry^.InstanceName);
	    write(WOutputFile,' ');
	    PrintName(WOutputFile,CurrentNetworkEntry^.ModuleName);

	    CurrentSignal := CurrentNetworkEntry^.SignalListHead;

	    while (CurrentSignal <> nil) do begin
		
		write(WOutputFile,' ');
		PrintName(WOutputFile,CurrentSignal^.SignalName);

		CurrentSignal := CurrentSignal^.Link;
	    end;

	    writeln(WOutputFile,';');

	    CurrentNetworkEntry := CurrentNetworkEntry^.Link;
	end;

	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,EndNetworkKeyword);
	writeln(WOutputFile,';');
    end;

    if (WPlacementListHead <> nil) then begin

	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,PlacementKeyword);
	writeln(WOutputFile,';');

	CurrentPlacementEntry := WPlacementListHead;

	while (CurrentPlacementEntry <> nil) do begin

	    write(WOutputFile,'     ');
	    PrintName(WOutputFile,CurrentPlacementEntry^.InstanceName);
	    write(WOutputFile,' ',CurrentPlacementEntry^.XLocation:1:1,' ',
		CurrentPlacementEntry^.YLocation:1:1);
	    
	    if (CurrentPlacementEntry^.Reflection = RFLNONE) then 
		write(WOutputFile,' ')

	    else if (CurrentPlacementEntry^.Reflection = RFLY) then begin
		write(WOutputFile,' ');
		PrintKeyword(WOutputFile,YReflection);
	    end

	    else
		write(WOutputFile,' UNKNOWN REFLECTION');
	       
	    if (CurrentPlacementEntry^.Rotation = ROT0) then 
		write(WOutputFile,' ')
	    
	    else if (CurrentPlacementEntry^.Rotation = ROT90) then begin
		write(WOutputFile,' ');
		PrintKeyword(WOutputFile,Rot90String);
	    end

	    else if (CurrentPlacementEntry^.Rotation = ROT180) then begin
		write(WOutputFile,' ');
		PrintKeyword(WOutputFile,Rot180String);
	    end

	    else if (CurrentPlacementEntry^.Rotation = ROT270) then begin
		write(WOutputFile,' ');
		PrintKeyword(WOutputFile,Rot270String);
	    end

	    else
		write(WOutputFile,' UNKNOWN ROTATION');

	    writeln(WOutputFile,';');
		

	    CurrentPlacementEntry := CurrentPlacementEntry^.Link;
	end;

	writeln(WOutputFile,'   ',EndPlacementKeyword,';');
	write(WOutputFile,'   ');
	PrintKeyword(WOutputFile,EndPlacementKeyword);
	writeln(WOutputFile,';');
    end;

    
    write(WOutputFile,'   ');
    PrintKeyword(WOutputFile,EndModuleKeyword);
    writeln(WOutputFile,';');
end;
################################################################################
pptest.p
################################################################################
{ ptest - test the Preas/Roberts placement language parser 

   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.
}



Program TestParser(input,output);

#include "pbenchtypes.h"

    type FileNameType = packed array [1..100] of char;

    var InputFile : text; 
	OutputFile: text;

    Name: NameType; 
    ModType: ModuleType; 
    Width: Number; 
    Height: Number; 
    IOListHead: IOListPointer; 
    NetworkListHead: NetworkListPointer; 
    PlacementListHead: PlacementListPointer; 
    Done: Boolean; 
    ModuleNumber: Integer; 
    LineNumber: Integer;

    InputFileName : FileNameType;
    OutputFileName: FileNameType;

#include "pext.h"

Begin

    if argc < 2  then
	writeln('Usage: ptest inputfile outputfile');
    
    LineNumber := 1;

    { Open input and output files }

    argv(1,InputFileName);
    argv(2,OutputFileName);

    reset(InputFile,InputFileName);

    rewrite(OutputFile,OutputFileName);

    Done := false;
    ModuleNumber := 1;
    while not Done do begin

	GetModule(InputFile, Name, ModType, Width, Height, IOListHead, 
		  NetworkListHead, PlacementListHead, LineNumber);
	
	if ModType = ENDFILE then Done := true

	else begin
	    writeln('Got Module ',ModuleNumber);

	    WriteModule(OutputFile, Name, ModType, Width, Height, IOListHead, 
		      NetworkListHead, PlacementListHead);
	end;

    ModuleNumber := ModuleNumber + 1;
    end;
    
end.

################################################################################
makefile
################################################################################
PFLAGS=-g 

pptest: pptest.o getmodule.o writemodule.o 
	pc ${PFLAGS} -o pptest pptest.o getmodule.o writemodule.o 

pptest.o: pptest.p pbenchtypes.h pext.h
	pc ${PFLAGS} -c -w pptest.p

getmodule.o: getmodule.p pbenchtypes.h pext.h
	pc ${PFLAGS} -c getmodule.p

writemodule.o: writemodule.p pbenchtypes.h pext.h
	pc ${PFLAGS} -c writemodule.p
################################################################################
pparse.doc
################################################################################
		Preas-Roberts Placement Language Parser Documentation 

Copyright (C) 1987 by Jonathan Rose.  All rights reserved.

This document describes the Preas-Roberts placement language parser.
The parser is written in the Pascal language.

This is written in Berkeley Pascal, not standard Pascal, but aside
from I/O, this should be reasonably portable.

There is only one routine used to interface to the parser: GetModule.
This routine reads from a specified input file and provides all of the
information contained in the next "MODULE" in that file. 

The calling sequence is as follows:

    #include "pbenchtypes.h"
    #include "pext.h"
    Procedure GetModule(var InputFile: text; 
			var Name: NameType;
			var ModType: ModuleType;
			var Width: Number;
			var Height: Number; 
			var IOListHead: IOListPointer; 
			var NetworkListHead: NetworkListPointer; 
			var PlacementListHead: PlacementListPointer;
			var LineNumber: Integer);
			external;


->>>>>>NOTE: LineNumber must be initialized to 1 before any modules are read.


The data types used are all described in the file pbenchtypes.h.  
There are constants associated with some of the
types, and are also given in that file.


Inputs to GetModule:
--------------------

InputFile is text type file variable, which must already be "reset".

Outputs from GetModule:
-----------------------

Name is character string that is the name of the Module.

ModType is one of 6 constants, defined in benchtypes.h, that indicated
the type of module.  The constant names are:
        STANDARD    
        PAD        
        GENERAL   
        PARENT   
        FEEDTHROUGH
        ENDFILE
The constant "ENDFILE" indicates that the end of file was reached and that 
no module was read.



->>>>>>NOTE: In all of the strings in this program, there are no imbedded
	     blanks.  The blank character is used as the string terminator,
	     so all characters after (and including) and BLANK (' ') should
	     be ignore.<<<<<<<<<<<<<<<

The Width and Height are as given in the corresponding line.  If Width and
Height are not specified, they are set to 0.

IOListHead is a pointer to a pointer to the IOList type defined in
pbenchtypes.h.  It is the head pointer of a linked list of IO pin 
entries for the module.  Each entry contains:
1) SignalName - a string.
2) Terminal Type - one of the constants I,O,B,PI,PO,PB,F.
3) Side Type - one of the constants BOTTOM,LEFT,TOP,RIGHT. 
   or is set to the constant NOSIDE if none was specified.
4) Position - a floating number if specified.  The constant NOPOSITION if not.
5) Width - a floating number if specified.  The constant NOWIDTH if not.
6) Layer - one of the constants PDIFF,NDIFF,POLY,METAL1,METAL2 if specified.
   or is set to the constant NOLAYER if not specified.
7) Link - a pointer to the next element in the IOList.  If this is the
   last then it is set to nil.
The pointer IOListHead is set to nil if an IOList not is given.

NetworkListHead is a pointer to a pointer to the NetworkList type defined in
pbenchtypes.h.  It is the head of a linked list of network entries, each
containing:
1) InstanceName - a string.
2) ModuleName - a string.
3) SignalName - the head of linked list of signal names.
4) Link - pointer to the next element. Set to nil if it is
   the last element.  
The pointer NetworkListHead is set to nil if no Network was
given in the module definition.

PlacementListHead is a pointer to a pointer to the PlacementList type defined
in pbenchtypes.h  It is the head of a linked list of placement entries, each
containing:
1) InstanceName - a string.
2) XLocation - a floating point number.
3) YLocation - a floating point number.
4) Reflection - one of the constants RFLNONE or RFLY. (default RFLNONE)
5) Rotation - one of the constants ROT0,ROT90,ROT180,ROT270. (default ROT0)
6) Link - pointer to the next element.  nil if is last.
The pointer PlacementListHead is set to nil if no Placement was
given in the module definition.


____________________________________________________________________________
Notes:

1) The parser only checks syntax.  If a syntax error is found, it is reported,
   along with the line number, and the program exits.  Thus, unfortunately,
   only one error is found at a time.  The errors are reported on the
   standard error output. 

2) The parser does not do semantic checking - i.e. it doesn't look for 
   duplicate names of modules or pins or any such thing.  It doesn't check
   to see if, a "CIRCUIT" type module in fact has an NETWORK specified.  All
   this is left to the user.

3) Comments (delimited by /* and */) must have white space around the
   delimiters.  i.e a comment /*Hi there*/ is not allowed, but /* Hi there */
   is.

_________________________________________________________________


The routine WriteModule outputs the Preas-Roberts form of a module,
starting from the form produced by GetModule.

Procedure WriteModule(var OutputFile: text;
		      Name: NameType; 
		      ModType: ModuleType; 
		      Width: Number; 
		      Height: Number; 
		      IOListHead: IOListPointer; 
		      NetworkListHead: NetworkListPointer; 
		      PlacementListHead: PlacementListPointer);

OutputFile must be opened (i.e. using "rewrite") before the routine is called.

The parameters are not passed var, except for the OutputFile.


