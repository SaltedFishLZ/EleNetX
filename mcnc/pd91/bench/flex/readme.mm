.PH ""
.ce
Flexibility Benchmark

.H 1 "Basic Accumulator"

This is the first benchmark in the Physical Design Workshop.
It is an 8 bit accumulator using a ripple carry adder 
and 8 edge-triggered flipflops.
Each flip-flop is a master/slave with a single clock input.
As data is applied,
the adder computes a sum,
the clock rises and the sum is loaded into the flip flops.
Each stage has a full adder,
except the first and last,
which have, respectively,
the carry input and carry output circuitry removed.
Each flip-flop takes a single clock line,
and has clear and reset inputs.
Please lay it out as follows:
place the inputs across the top,
bits 7 (most significant) on the left to 0 (least significant) on the right,
and the outputs across the bottom,
also bits 7 -> 0.
Assume power is available from a single point above the top,
and ground from a single point below the bottom.
Put the clock, set, and reset inputs on left side.
Use just one level of metal,
and shoot for minimum total area.

.H 1 "FOR YOU STANDARD CELL FANS"
Lay out 8 adders and flip-flops with your favorite polycell library --  
call this 1.0.

.H 1 "Cell Synthesis"
A CMOS net list,
using all the same size transistors,
is described in ``accum.vpnr.''
Lay the accumulator out cell at a time or all at once.
This is benchmark 1.1.

.H 1 "Fall-back: Simplified Accumulator"

If you have gotten this far,
you have demonstrated more flexibility than most.
This is because the circuit described uses non-dual CMOS logic in the adders,
as described in Weste's book page 314 on the middle stages.
It also uses pass transistors in the edge-triggered flip-flops,
which are described on page 219 of Weste's book.
If you had trouble laying this out,
(e.g. your system cannot handle complex gates,
or non-dual gates),
you will have to do the fall-back circuit.
This uses the adder on page 447 of ``An Introduction
to Computer Logic'' by Nagle, Carroll, and Irwin.
Made with only NAND gates.

This results in a greater number of transistors.
The flip-flop has been reimplemented using 3 NAND gates (12 transistors)
instead of each group of pass-gates (a total of 16 more transistors).
This circuit is in ``accum.simple.vpnr''.
This is benchmark 1.2,
and is optional (do it only if you could not do 1.1).

.H 1 "Flexing your Synthesizer's Muscles"

Now for the ``flexibility part.''
Many people look at module generators as suitable for shoving 
small circuits in small places between larger modules.
So, please run the benchmark as follows:

A) run for minimum cell height (no limit on width) (Call this benchmark 1.3.)

B) run for minimum width (no limit on height). This will reduce
the pitch between inputs. (Call this benchmark 1.4).


.H 2 "Sizing Transistors"

Now that you have the basic accumulator laid out,
do it again with different transistor sizes.
The sized version is in ``accum.sized.vpnr.''
Call this benchmark 1-5.

.H 2 "Second level Metal"

Now lay it out for minimum area using second-level metal throughout,
including for power and ground, with second-level metal running horizontally.
Call this benchmark 1.6.

Now use any number of metal layers you want in any orientation,
and lay it out for minimum area.
Call this benchmark 1.7.

.H 2 "NMOS"

An NMOS version is available in ``accum.nmos.vpnr.'' Lay
it out with any number of metal layers.
Call this 1.8.

.H 2 "Bipolar"

Unfortunately,
we do not have a circuit for a bipolar design.
But if your system can synthesize one meeting the overall shape
and function requirements,
do so and call it 1.9.

.H 2 "Change the Circuit"

Just using the functional specifications (e.g. an 8 bit
accumulator with inputs at top, etc),
lay it out for minimum area. 
You may change the circuit any way you want (including using smaller FETs),
and/or use any levels you want.
Hint: there are redundant inverters on the clock lines in the flip-flops.
Call this 1.10.

Now just using the functional specifications (e.g. an 8 bit
accumulator with inputs at top, etc),
lay it out for maximum speed. 
You may change the circuit any way you want,
e.g. use carry lookahead,
and/or use any levels you want.
Hint: you may want to make an even/odd cell to reduce the number of
gates/stage.
Call this 1.11.

