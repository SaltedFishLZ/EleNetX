"""Python parser for YAL description language
"""
import os

import ply.lex as lex
import ply.yacc as yacc


keywords = {
    'MODULE'        :   'Module'        ,
    'ENDMODULE'     :   'EndModule'     ,

    'IOLIST'        :   'IOList'        ,
    'ENDIOLIST'     :   'EndIOList'     ,

    'NETWORK'       :   'Network'       ,
    'ENDNETWORK'    :   'EndNetwork'    ,
    
    'PLACEMENT'     :   'Placement'     ,
    'ENDPLACEMENT'  :   'EndPlacement'  ,

    'DIMENSIONS'    :   'Dimensions'    ,

    # Module types
    'TYPE'          :   'Type'          ,
    'STANDARD'      :   'Standard'      ,
    'PAD'           :   'Pad'           ,
    'GENERAL'       :   'General'       ,
    'PARENT'        :   'Parent'        ,
    'FEEDTHROUGH'   :   'Feedthrough'   ,

    # Terminal types
    'I'             :   'I'             ,
    'O'             :   'O'             ,
    'B'             :   'B'             ,
    'PI'            :   'PI'            ,
    'PO'            :   'PO'            ,
    'PB'            :   'PB'            ,
    'F'             :   'F'             ,
    'PWR'           :   'PWR'           ,
    'GND'           :   'GND'           ,

    # Side enum
    'BOTTOM'        :   'Bottom'        ,
    'RIGHT'         :   'Right'         ,
    'TOP'           :   'Top'           ,
    'LEFT'          :   'Left'          ,

    # Layer enum
    'PDIFF'         :   'PDiff'         ,
    'NDIFF'         :   'NDiff'         ,
    'POLY'          :   'Poly'          ,
    'METAL1'        :   'Metal1'        ,
    'METAL2'        :   'Metal2'        ,

    # Reflection enum
    'RFLNONE'       :   'ReflectNone'   ,
    'RFLY'          :   'ReflectY'      ,

    # Rotation
    'ROT0'          :   'Rot0'          ,
    'ROT90'         :   'Rot90'         ,
    'ROT180'        :   'Rot180'        ,
    'ROT270'        :   'Rot270'        ,

    'VOLTAGE'       :   'Voltage'       ,
    'CURRENT'       :   'Current'       ,
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



"""
Rules for YACC


"""
def YalParser():
    pass



if __name__ == "__main__":

    lexer = YalLexer()

    fpath = os.path.join('mcnc', 'pd91', 'bench', 'floorplan', 'fan.yal')
    fpath = os.path.join('mcnc', 'pd91', 'bench', 'mixed', 'a3.yal')

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
                # if tok.type == 'Network':
                #     print(tok)
                # print(tok)


            
            # print("num of tokens in line:", num_tokens_in_line)

            # if lexer.lineno > 1000:
            #     break

