The basic concept of 'load':

1) The parser template sits in parser.tmpl.

2) User's data for 'load' gets converted into 'ed' commands that find
   the appropriate comments in 'parser.tmpl' and put the code there.
   The resulting file is called 'load.zz.c'.

   The general idea is that from:

		## keyword1 keyword2
		<some stuff>

   the following is generated for 'ed':

		/\/\* keyword1 keyword2 \*\//a
		<some stuff>
		.
   
   The 'ed' editor puts then <some stuff> in 'parser.tmpl' immediately
   after a line containing text "/* keyword1 keyword2 */"

3) The translation is done within the lex-generated procedures.  Syntax
   is verified in yacc-generated procedures.  Any error causes a message and
   exit with status 1.  The 'load' run by the users is actually a shell file
   that runs the binary load from the library.

4) To implement future extensions of the language:

	* do whatever necessary to 'libparse.a' in '../parser'
	* write appropriate interpreter routines and add them to 'parser.tmpl'
	* put comments of the form "/* keyword1 keyword2 ... */" in the places
	  where user-supplied code will go.  IMPORTANT: use single spaces within
	  the comments.
	* Add commented key_lines to 'LOAD.tmpl'
	* Modify load.y to accept new key_lines.
	* do 'make' and make a copy of 'parser.tmpl'; call it 'tmp.tmpl'
	* verify that 'load <load.tmpl | /bin/ed tmp.tmpl' does not produce
	  any error messages.  Repeat for 'LOAD.tmpl' as the data.
	* do 'make install'

Krzysztof Kozminski
kk@mcnc.org

Created: Oct 11, 1988
Revised: Jan 04, 1989
