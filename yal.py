"""Python parser for YAL description language
"""
import os

import ply.lex as lex


keywords = {
    'MODULE'        :   'Module'        ,
    'ENDMODULE'     :   'EndModule'     ,

    'IOLIST'        :   'IOList'        ,
    'ENDIOLIST'     :   'EndIOList'     ,

    'NETWORK'       :   'Network'       ,
    'ENDNETWORK'    :   'EndNetwork'    ,
    
    'PLACEMENT'     :   'Placement'     ,
    'ENDPLACEMENT'  :   'EndPlacement'  ,
    
    'VOLTAGE'       :   'voltage'
}

tokens = [
    'comment',
    'semicolon',
    'number',
    'name',
    'moduletype',
    'terminaltype',
    'side',
    'layer',
    'reflection',
    'rotation'
] + list(keywords.values())



def YalLexer():

    # C style comments
    t_comment = r'(/\*(.|\n)*?\*/)|(//.*)'

    # Semicolon : end of logical line
    t_semicolon = r'\;'

    # Numbers in YAL (a token is a number or name is not context-free)
    t_number = r'[0-9]*([0-9]\.?|\.[0-9])[0-9]*([Ee][-+]?[0-9]+)*'
    
    # Module type : we merge all enumerations to token
    t_moduletype = r'(STANDARD|PAD|GENERAL|PARENT|FEEDTHROUGH)'
    
    # Terminal type
    t_terminaltype = r'I|O|B|PI|PO|PB|F|PWR|GND'
    
    # Side
    t_side = r'BOTTOM|RIGHT|TOP|LEFT'
    
    # Layer
    t_layer = r'PDIFF|NDIFF|POLY|METAL1|METAL2'
    
    # Reflection
    t_reflection = r'RFLNONE|RFLY'

    # Rotation
    t_rotation = r'ROT0|ROT90|ROT180|ROT270'

    def t_name(t):
        r'[a-zA-Z_#$%&<>\.][a-zA-Z_#$%&<>\.0-9]*'
        # r'[^:\s\t]+'
        # t.type = reserved.get(t.value, 'name')
        if (t.value in keywords):
            t.type = keywords[t.value]
        else:
            t.type = 'name'
        return t

    # Define a rule so we can track line numbers
    def t_newline(t):
        r'\n+'
        t.lexer.lineno += len(t.value)

    # A string containing ignored characters (spaces and tabs)
    t_ignore  = ' \t'

    # Error handling rule
    def t_error(t):
        print("Illegal character '%s'" % t.value[0])
        t.lexer.skip(1)

    return lex.lex()


if __name__ == "__main__":

    lexer = YalLexer()

    fpath = os.path.join('mcnc', 'pd91', 'bench', 'floorplan', 'fan.yal')
    fpath = os.path.join('mcnc', 'pd91', 'bench', 'mixed', 'a3.yal')

    with open(fpath) as f:
        for line in f:

            lexer.input(line)
            lexer.lineno += 1

            while True:
                tok = lexer.token()
                if not tok:
                    break
                if tok.type == 'comment':
                    pass
                if tok.type in keywords:
                    pass
                if tok.type == 'number':
                    print(tok)

                tok = lexer.token()

            if lexer.lineno > 1000:
                break

