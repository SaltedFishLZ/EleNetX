.PH "''"
.ce
Arithmetic and Logic Circuits

This section tests some basic arithmetic and logic benchmarks.
They are highly regular,
but not intended to be perfectly regular.
You may lay them out by synthesizing 
a small pieces and glueing them together,
or put all the circuitry together and lay it out all at once.

Use the .vpnr files to get the circuitry,
including the detailed transistor level circuits of the leaf cells.
Note that these files are for a 3 micron process,
with the unitsize set to 1 nanometer (0.001).
Another thing to watch out for:
the library cells contain extra nets,
called feedthroughs,
which are not connected to any transistors.
These do not appear on the list of formal net parameters
when the cells are called.
The way you can tell they are feedthroughs is that they have
a string parameter called "type" which has the value "feedthrough".
(They also happen to begin with the letter 'u'.)
If you are synthesizing the circuit completely,
you can ignore these nets.

The files tell you which side each primary input
and output should be on,
but you are not restricted use this information.
See the a,b,c discussion below.
Note that these files were originally designed for work with standard cell,
and you can use your standard cell system on them if you wish,
assuming the same library or one of your own with the same functions.
Please lay them out assuming 
power available at the top and ground at the bottom.

Try to provide some estimate of the speed of your circuits.

1) Benchmark a is the TI74181 4 bit ALU slice, found in ti_alu.vpnr.
This file assumes the inputs are called a_0 ->a_3, b_0->b_3, and c for
the carry input. 
``m'' and s_0 ->s_3 are control inputs. 
The outputs are called f_0 ->f_4, equal (for the a==b output)
x, cout (for the cn+4 pin), x, and y.
If you read the TI databook around page 7-272 you should be able
to figure out how it works.
Note that the circuit was entered manually and may contain errors.
The xor gates were computed as

	x = !((a && b) || (!a && !b));

The invert/or gate that computes cout was computed with a NAND.
The nets of the form w_a_0->w_a_3 and w_b_0 -> w_b_3 are the outputs
of the AOI gates on the left side.

Please lay it out with the inputs a and b on the top, the control
lines on the left feeding through to the right,
and the outputs on the bottom.
Except that the carry input and output should line up.
In other words, 
lay it out so that you can assemble it into a wider (say a 16 bit)
ALU.

2) Benchmark b is a ``fractional multiplier'' and is found in ``fract.vpnr''.
See ``Intro to Computer Logic'' by H. Troy Nagle et al. Prentice Hall 1975, 
page 461.
Signal W is the carry-out from the counter.  
Other signals are as in the book.
It uses the standardcell database described in db.vpnr.
Make the inputs (which is most of the nets) on the top
and the outputs W and Z on the bottom.

3) Benchmark c is a 16 bit integer multiplier with 32 bit output.
It is found in ``mult16.vpnr.''
Lay it out with data inputs and outputs on any side you like.

FOR YOU STANDARD CELL FANS: 
Lay them out with the polycell library geometry described in db.vpnr,
or with your own. 
Call these benchmarks 2.0.a, 2.0.b, 2.0.c; 
 
Basic Cell Synthesis:
Without using 2nd level metal,
run the benchmarks with power and control lines running horizontally in metal,
data vertically in poly.
Call this 2.1.a, 2.1.b, 2.1.c; 

TO GO FURTHER: Use 2nd level metal in any orientation you want.
Call these 2.2.a, 2.2.b, and 2.2.c

TO SHOW OFF: Size the transistors, 
and/or reorder series devices, and lay it out again.
Question: is the layout smaller and/or faster with sized devices?
Call these 2.3.a, 2.3.b, and 2.3.c

FOR OVERACHIEVERS: Lay them out making use of 3rd level metal.
Call these 2.4.a, 2.4.b, and 2.4.c

FOR VISIONARIES: Lay them out in a Sea-of-Gates style. 
You choose the generic wafer.
Any number of metal layers you want.
Call these 2.5.a, 2.5.b, and 2.5.c

