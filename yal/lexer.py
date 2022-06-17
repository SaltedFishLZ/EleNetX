"""Python parser for YAL description language
"""
import os

import ply.lex as lex
import ply.yacc as yacc


keywords = {
    'MODULE'        :   'MODULE'        ,
    'ENDMODULE'     :   'ENDMODULE'     ,

    'IOLIST'        :   'IOLIST'        ,
    'ENDIOLIST'     :   'ENDIOLIST'     ,

    'NETWORK'       :   'NETWORK'       ,
    'ENDNETWORK'    :   'ENDNETWORK'    ,
    
    'PLACEMENT'     :   'PLACEMENT'     ,
    'ENDPLACEMENT'  :   'ENDPLACEMENT'  ,

    'DIMENSIONS'    :   'DIMENSIONS'    ,

    # Module types
    'TYPE'          :   'TYPE'          ,
    'STANDARD'      :   'STANDARD'      ,
    'PAD'           :   'PAD'           ,
    'GENERAL'       :   'GENERAL'       ,
    'PARENT'        :   'PARENT'        ,
    'FEEDTHORUGH'   :   'FEEDTHORUGH'   ,

    # Terminal types
    'I'             :   'I'             ,
    'O'             :   'O'             ,
    'B'             :   'B'             ,
    'PI'            :   'PI'            ,
    'PO'            :   'PO'            ,
    'PB'            :   'PB'            ,
    'F'             :   'F'             ,
    'PWR'           :   'PWR'           ,
    # I cannot understand why there can be signals with name 'GND'
    # why?
    # 'GND'           :   'GND'           ,

    # Side enum
    'BOTTOM'        :   'BOTTOM'        ,
    'RIGHT'         :   'RIGHT'         ,
    'TOP'           :   'TOP'           ,
    'LEFT'          :   'LEFT'          ,

    # Layer enum
    'PDIFF'         :   'PDIFF'         ,
    'NDIFF'         :   'NDIFF'         ,
    'POLY'          :   'POLY'          ,
    'METAL1'        :   'METAL1'        ,
    'METAL2'        :   'METAL2'        ,

    # Reflection enum
    'RFLNONE'       :   'RFLNONE'       ,
    'RFLY'          :   'RFLY'          ,

    # Rotation
    'ROT0'          :   'ROT0'          ,
    'ROT90'         :   'ROT90'         ,
    'ROT180'        :   'ROT180'        ,
    'ROT270'        :   'ROT270'        ,

    'VOLTAGE'       :   'VOLTAGE'       ,
    'CURRENT'       :   'CURRENT'       ,
}

tokens = [
    'Comment',
    'Semicolon',
    'Number',
    'Name',
] + list(keywords.values())



def YalLexer():

    # C style comments
    def t_Comment(t):
        r'(/\*(.|\n)*?\*/)|(//.*)'
        # return t
        pass

    # Semicolon : end of logical line
    def t_Semicolon(t):
        r'\;'
        return t

    # Numbers in YAL (a token is a number or name is not context-free)
    def t_Number(t):
        r'[-+]?[0-9]*([0-9]\.?|\.[0-9])[0-9]*([Ee][-+]?[0-9]+)*'
        return t

    def t_Name(t):
        # r'[a-zA-Z_#$%&<>\.][a-zA-Z_#$%&<>\.0-9]*'
        r'[^; \s\t]+'
        t.type = keywords.get(t.value, 'Name')
        # if (t.value in keywords):
        #     t.type = keywords[t.value]
        # else:
        #     t.type = 'Name'
        return t

    # Define a rule so we can track line numbers
    def t_newline(t):
        r'\n+'
        t.lexer.lineno += len(t.value)

    # A string containing ignored characters (spaces and tabs)
    t_ignore  = ' \t'

    # Error handling rule
    def t_error(t):
        print("[Error] Line %d : Illegal character '%s'" %
              (t.lexer.lineno, t.value[0]))
        t.lexer.skip(1)

    return lex.lex()



def test_lexer(fpath):
    lexer = YalLexer()

    with open(fpath) as f:
        for line in f:

            lexer.input(line)

            # print("=" * 64)
            # print("line no", lexer.lineno)

            num_tokens_in_line = 0
            while True:
                # Get new token
                tok = lexer.token()
                if not tok:
                    break
                num_tokens_in_line += 1

                # if tok.type == 'Comment':
                #     pass
                # if tok.type in keywords:
                #     pass
                # if tok.type == 'NETWORK':
                #     print(tok)
                # if tok.type == 'ENDNETWORK':
                #     print(tok)
                # print(tok)


            
            # print("num of tokens in line:", num_tokens_in_line)

            # if lexer.lineno > 1000:
            #     break




def YalParser(**kwargs):


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
        print('signal name')
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
        print("terminal type = ", p[1])
        return {'TerminalType' : p[1]}
    
    def p_Side(p):
        r'''
        Side    :   BOTTOM
                |   RIGHT
                |   TOP
                |   LEFT
        '''
        print('side = ', p[1])
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

    

    fpath = os.path.join('mcnc', 'pd91', 'bench', 'floorplan', 'fan.yal')
    fpath = os.path.join('mcnc', 'pd91', 'bench', 'mixed', 'a3.yal')

    test_lexer(fpath)


    lexer = YalLexer()
    parser = YalParser(debug=True)


    with open(fpath) as f:
        text = f.read()

        parser.parse(text)