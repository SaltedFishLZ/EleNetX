.PH ""
.ce
Finite State Machines

This section tests the ability to lay out irregular logic.
All of the benchmarks are finite state machines,
including both combinational  logic and flip flops.
The flip-flops include transistors for a self scanning feature.

Please lay them out assuming power available at the top and ground at the bottom.
You may provide access to all I/O nets on any side you choose.
The flip flops all include serial shifting to allow easier testing.
You may reorder the flip-flops on this testing line if you think it will decrease area.  
(E.g. place them without this net,
and then route this net in afterwards, keeping track of the order of the flip-flops).

Try to provide some estimate of the speed of your circuits.

1) Benchmark a is a traffic light controller and is found in highway.vpnl.

2 & 3) Try the circuits in ../stdcell/*.vpnr

NOTE: definitions of the leaf cells used in the benchmarks are found in
the file "db.vpnr", which is a symbolic link to "../stdcell/db.vpnr".
 
FOR YOU STANDARD CELL FANS:
Lay them out with your favorite polycell library,
preferably but not necessarily the one in db.vpnr,
call this 3.0.a, 3.0.b, 3.0.c;

Basic Cell Generation Benchmarks:
Generate the cells,
and glue them together,
or do the whole circuit all at once.
You may use 2nd level metal, or not (see below).
Call these 3.1.a, 3.1.b, 3.1.c.

TO GO FURTHER: If you used 2nd level metal,
eliminate it. 
If you did not use it,
use 2nd level metal.
Question: is the layout smaller and/or faster with 2nd level metal?
Call these 3.2.a, 3.2.b, 3.2.c.

TO SHOW OFF: Size the transistors, 
and/or reorder series devices, and lay it out again.
Question: is the layout smaller and/or faster with sized devices?
Call these 3.3.a, 3.3.b, 3.3.c.

FOR OVERACHIEVERS: Lay them out making use of 3rd level metal.
Call these 3.4.a, 3.4.b, 3.4.c.

FOR VISIONARIES: Lay them out in a Sea-of-Gates style. You choose the generic wafer.
Call these 3.5.a, 3.5.b, 3.5.c.

