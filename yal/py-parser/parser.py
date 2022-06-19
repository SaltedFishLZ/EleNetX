"""Python parser for YAL description language
"""
import os

import ply.lex as lex
import ply.yacc as yacc

from lexer import tokens, keywords, YalLexer



def YalParser(**kwargs):

    # -------------------------------- #
    #            Module                #
    # -------------------------------- #
    
    def p_ModuleList(p):
        r'''
        ModuleList  :   Module
                    |   ModuleList Module
        '''
        pass

    def p_Module(p):
        r'''
        Module  :   MODULE ModuleName Semicolon TYPE ModuleType Semicolon Dimensions IO ENDMODULE Semicolon
                |   MODULE ModuleName Semicolon TYPE ModuleType Semicolon IO Network ENDMODULE Semicolon
                |   MODULE ModuleName Semicolon TYPE ModuleType Semicolon Dimensions IO Network ENDMODULE Semicolon
        '''
        pass

    # -------------------------------- #
    #         Dimension Info           #
    # -------------------------------- #

    def p_Dimensions(p):
        r'''
        Dimensions  :   DIMENSIONS  DimensionList   Semicolon
        ''' 
        pass

    def p_DimensionList(p):
        r'''
        DimensionList   :   XiYiPair
                        |   DimensionList XiYiPair
        '''
        pass

    def p_XiYiPair(p):
        r'''XiYiPair : Number Number'''
        pass

    # -------------------------------- #
    #           IO Struct              #
    # -------------------------------- #

    def p_IO(p):
        r'''
        IO  :   IOLIST Semicolon IOList ENDIOLIST Semicolon
        '''
        print('IO block matched!')
        pass

    def p_IOList(p):
        r'''
        IOList  :   IOLine
                |   IOList IOLine
        '''
        pass

    def p_IOLine(p):
        r'''
        IOLine  :   SignalName TerminalType XPositionOrSide Semicolon
                |   SignalName TerminalType XPositionOrSide YPositionOrPosition Semicolon
                |   SignalName TerminalType XPositionOrSide YPositionOrPosition WidthAndLayer Semicolon
                |   SignalName TerminalType XPositionOrSide YPositionOrPosition WidthAndLayer Current Semicolon
                |   SignalName TerminalType XPositionOrSide YPositionOrPosition WidthAndLayer Voltage Semicolon
                |   SignalName TerminalType XPositionOrSide YPositionOrPosition WidthAndLayer Current Voltage Semicolon
        
        XPositionOrSide :   XPosition 
                        |   Side
        
        YPositionOrPosition :   YPosition
                            |   Position
        '''
        print("IO line matched!")
        pass

    def p_WidthAndLayer(p):
        r'''
        WidthAndLayer   :   Width Layer
        '''
        pass

    def p_Current(p):
        r'''
        Current :   CURRENT Number
        '''
        pass

    def p_Voltage(p):
        r'''
        Voltage :   VOLTAGE Number
        '''
        pass

    # -------------------------------- #
    #       Network Struct             #
    # -------------------------------- #

    def p_Network(p):
        r'''
        Network         :   NETWORK Semicolon NetworkLineList ENDNETWORK Semicolon
        '''
        print("network matched!")
        pass

    def p_NetworkLineList(p):
        r'''
        NetworkLineList :   NetworkLine
                        |   NetworkLineList NetworkLine
        '''
        print("network list matched!")
        pass

    def p_NetworkLine(p):
        r'''
        NetworkLine     :   Name Name SignalNameList Semicolon
        '''
        print("network line matched")
        pass

    def p_NetworkSignalNameList(p):
        r'''
        SignalNameList  :   SignalName
                        |   SignalName SignalNameList
        '''
        print('signal name list')
        pass

    # -------------------------------- #
    #       Terminal Rules             #
    # -------------------------------- #

    def p_ModuleName(p):
        r'''
        ModuleName : Name
        '''
        print('module name = ', p[1])
        pass

    def p_ModuleType(p):
        r'''
        ModuleType  :   STANDARD
                    |   PAD
                    |   GENERAL
                    |   PARENT
                    |   FEEDTHROUGH
        '''
        pass

    def p_Width(p):
        r'''Width : Number'''
        pass

    def p_Height(p):
        r'''Height : Number'''
        pass

    def p_SignalName(p):
        r'''
        SignalName : Name
        '''
        # print('signal name')
        pass

    def p_TerminalType(p):
        r'''
        TerminalType    :   I
                        |   O
                        |   B
                        |   PI
                        |   PO
                        |   PB
                        |   F
                        |   PWR
        '''
        # print("terminal type = ", p[1])
        return {'TerminalType' : p[1]}
    
    def p_Side(p):
        r'''
        Side    :   BOTTOM
                |   RIGHT
                |   TOP
                |   LEFT
        '''
        # print('side = ', p[1])
        return {'Side' : p[1]}
    
    def p_Layer(p):
        r'''
        Layer   :   PDIFF
                |   NDIFF
                |   POLY
                |   METAL1
                |   METAL2
        '''
        return {'Layer' : p[1]}
    
    def p_XPosition(p):
        r'''XPosition : Number'''
        pass

    def p_YPosition(p):
        r'''YPosition : Number'''
        pass

    def p_Position(p):
        r'''Position : Number'''
        pass

    def p_Location(p):
        '''XLocation : Number'''
        pass

    def p_YLocation(p):
        '''YLocation : Number'''
        pass

    def p_Xi(p):
        '''Xi : Number'''
        pass

    def p_Yi(p):
        '''Yi : Number'''
        pass

    def p_Reflection(p):
        r'''
        Reflection  :   RFLNONE
                    |   RFLY
        ''' 
        pass

    def p_Rotation(p):
        r'''
        Rotation    :   ROT0
                    |   ROT90
                    |   ROT180
                    |   ROT270
        '''
        pass

    return yacc.yacc(**kwargs)






if __name__ == "__main__":


    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'block', 'ami33.yal') # why line 729 is a number?
    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'block', 'ami49.yal')
    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'block', 'apte.yal')
    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'block', 'hp.yal')
    fpath = os.path.join('mcnc', 'pd91', 'bench', 'block', 'xerox.yal')


    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'floorplan', 'fan.yal')

    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'gatearr', 'GALib.yal')

    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'mixed', 'a3.yal')
    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'mixed', 'g2.yal')
    # fpath = os.path.join('mcnc', 'pd91', 'bench', 'mixed', 't1.yal')


    lexer = YalLexer()
    parser = YalParser(debug=True)


    with open(fpath) as f:
        text = f.read()
        parser.parse(text)