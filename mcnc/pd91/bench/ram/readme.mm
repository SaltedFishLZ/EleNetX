.PH ""
.ce
Random Access Memory Benchmark
 
RAM is a staple of chip design.
This benchmark has two parts,
the first allows you to demonstrate a conventional,
parameterized RAM generator.
The second asks you to build a new type of RAM that it is unlikely that you
already have a generator for.


.H 1 "Conventional SRAM"

Build an SRAM that accepts 8 bit addresses, and produces 16 bit outputs.
That is, it should contain 16 * 256 bits total.
You can organize it as seems convenient,
e.g. 256 words with no multiplexing, 128 words multiplexed 2 ways, 
64 words multiplexed 4 ways, etc.
Assume power is available at the top,
ground and all IO, (address, data, control) is available at the bottom.
Also,
you may use whatever clocking strategy you normally use in your shop,
e.g. three phase cycle (precharge, decode, evaluate ) is OK.
The intent here is to demonstrate one of your existing,
production RAM generators.

Quote your read time, 
and write time in nanoseconds
and in terms of number of equivalent gate delays from the normalization cell.
Call this benchmark 4.1.

FALL BACK: if you can't handle 4K bits, build a smaller one, preferably still
16 bits per word, but maybe just 32 words.
Call this benchmark 4.2.

FOR OVERACHIEVERS: build one with 10 bits input (a 16K RAM).
Call this benchmark 4.3.

.H 1 "UnConventional SRAM"

Build a 16x16 bit SRAM that you can write by row or column and read by 
column or row. 
That is, 
it should have an input to select the row/column orientation.
This allows you to enter a raster-scan image in one orientation 
and read it out at 90 degrees.
An application for this is to take a set of characters and let
you display them with text running vertically.
This was described in Carry Kornfield's paper
``Fast Methods for Orthogonally Reorienting Bitmap Images,''
.I "1987 SID Technical Digest"
paper 21.2,
1987.
You will need two decoders:
each accepts 4 bit addresses and decodes into 16 outputs.
Each bit of memory is double ported:
it accepts two independent enable and data facilities.
To make this concrete,
modify the standard 6 transistor per cell SRAM model shown in Weste's book on page 350
to have 8 transistors per cell.
This makes for rather large cells,
but should simplify the analog design problem.
In the array of 256 memory cells,
there should be only 16 distinct data bits.
This is because each bit appears in all the elements in one column and in one row.
So you need 16 data drivers/receivers.

The intention of this exercise is to 
test the flexibility of automatic generators
when dealing with application specific circuits that are highly regular.
If you do this with some manual design,
please try to keep track of the number of designer hours/days
involved.
