





                          _B_I_B_L_I_O_G_R_A_P_H_Y




Beardslee, M., J. Burns, A. Casotto, M. Igusa, F. Romeo, and A.
     Sangiovanni-Vincentelli, "An Integrated Macro-Cell Layout
     System," _P_r_o_c_e_e_d_i_n_g_s _o_f _T_h_e _I_n_t_e_r_n_a_t_i_o_n_a_l _W_o_r_k_s_h_o_p _o_n _P_l_a_c_e_-
     _m_e_n_t _a_n_d _R_o_u_t_i_n_g, MCNC, Research Triangle Park, NC, May,
     1988.

     Paper describes the Mosaico Macro-Cell layout system.  Area
     and wire length Results (along with a plot) are cited for
     the PrimBBL1 benchmark.

Cai, H., "Connectivity Biased Channel Construction and Ordering
     for Building-Block Layout," _2_5_t_h _A_C_M/_I_E_E_E _D_e_s_i_g_n _A_u_t_o_m_a_t_i_o_n
     _C_o_n_f_e_r_e_n_c_e, pp. 560-565, IEEE Computer Society Press, Wash-
     ington D.C., June 1988.

     This paper introduces a number of techniques for the con-
     struction and ordering of routing channels for building-
     block layouts.  The algorithms use both the geometrical data
     (the placement) and the topological data (the connectivity)
     to determine the channel structure. The layout area, net
     length, number of vias, and CPU times (on Apollo DN3000) are
     reported for PrimBBL1 and PrimBBL2 benchmarks.

Cong, Jingsheng and Bryan Preas, "A New Algorithm for Standard
     Cell Global Routing," _D_i_g_e_s_t _o_f _T_e_c_h_n_i_c_a_l _P_a_p_e_r_s _I_C_C_A_D-_8_8,
     pp. 176-179, IEEE Computer Society Press, Washington D.C.,
     1988.

     A new global router for standard cell layouts is presented.
     The algorithm processes all nets in parallel.  Initially, it
     finds an optimal spanning forest on a net connection graph
     that contains all the interconnection information for pins
     in the design.  The number and the locations of all
     feedthroughs added to rows are thus determined. In the next
     stage, the net segments - to complete connections for each
     net - are chosen such that the total channel density is
     minimized. Results on chip width and chip height, total
     number of tracks and total feedthrough count for PrimSC1 and
     PrimSC2 are reported and compared with those obtained by
     TimberWolfSC version 4.2 global router.

Eschermann, Bernhard, Wei-Ming Dai, Ernest S. Kuh, and Massoud
     Pedram, "Hierarchical Placement for Macrocells: A ``Meet in
     the Middle'' Approach," _D_i_g_e_s_t _o_f _T_e_c_h_n_i_c_a_l _P_a_p_e_r_s _I_C_C_A_D-_8_8,
     pp. 460-463, IEEE Computer Society Press, Washington D.C.,
     1988.

     A hierarchical placement algorithm for building block layout
     is presented.  The algorithm combines the bottom-up shape








                              - 2 -



     propagation and the top-down enumeration techniques in order
     to minimize a linear combination of chip area, total wire
     length and aspect ratio mismatch. Other features of the sys-
     tem include hierarchical wiring area estimation (during
     placement) and shape optimization (after placement).  Chip
     area, wire length, and total via count for PrimBBL1 and
     PrimBBL2 benchmarks are reported and compared with those of
     MOSAICO, VITAL, Seatle Silicon, and Delft University.

Herrigel, A. and W. Fichtner, "An Analytic Optimization Technique
     for Placement of Macro-Cells," _2_6_t_h _A_C_M/_I_E_E_E _D_e_s_i_g_n _A_u_t_o_m_a_-
     _t_i_o_n _C_o_n_f_e_r_e_n_c_e, pp. 376-381, ACM, Inc., New York, June
     1989.

     This paper gives the total wire length and the chip area for
     PrimBBL1 benchmark.  (Post-layout result is also shown.)
     The authors map the placement problem (with rectangular and
     L-shaped blocks) into a nonlinear and nonconvex programming
     problem and use a modified penalty method to solve the prob-
     lem. Pin positions on the blocks are also optimized.

Igusa, Mitsuru, Mark Beardslee, and Alberto Sangiovanni-
     Vincentelli, "ORCA A Sea-of-gates P&R System," _2_6_t_h _A_C_M/_I_E_E_E
     _D_e_s_i_g_n _A_u_t_o_m_a_t_i_o_n _C_o_n_f_e_r_e_n_c_e, pp. 122-127, ACM, Inc., New
     York, June 89.

     The active area percentage and the CPU time on a VAX8650 for
     PrimGA1 and PrimGA2 benchmarks are reported. The total net
     length for PrimGA2 is compared against that of other systems
     e.g. LTX2, GE, Proud.  The system uses a min-cut floor-
     planner, an exhaustive enumeration local placer and an area
     router with rip-up-and-reroute algorithm.

Kleinhans, Jurgen M., George Sigl, and Frank M. Johannes, "GORDI-
     AN: A New Global Optimization / Rectangle Dissection Methods
     for Cell Placement," _D_i_g_e_s_t _o_f _T_e_c_h_n_i_c_a_l _P_a_p_e_r_s _I_C_C_A_D-_8_8,
     pp. 506-509, IEEE Computer Society Press, Washington D.C.,
     1988.

     A new placement method for cell-based layout styles is
     presented. It is composed of alternating and interacting
     global optimization and partitioning phases. In the global
     optimization phase, a constrained quadratic optimization
     problem is solved to determine the global placement of
     cells.  In the partitioning phase, each region is divided
     into two subregions, subsets of cells are assigned to these
     subregions, and new geometric constraints on cells are gen-
     erated.  Total half perimeter lengths and minimum spanning
     trees with squared Euclidean metric lengths for PrimSC1 and
     PrimSC2 are reported.  The total half perimeter length, chip
     area, and CPU time for PrimBBL2 are given as well.

Lee, Kai-Win and Carl Sechen, "A New Global Router for Row-based
     Layout," _D_i_g_e_s_t _o_f _T_e_c_h_n_i_c_a_l _P_a_p_e_r_s _I_C_C_A_D-_8_8, pp. 180-183,








                              - 3 -



     IEEE Computer Society Press, Washington D.C., 1988.

     A new global router for row-based layout styles such as
     sea-of-gates, gate array, and standard cell circuits is
     presented. The algorithm finds an approximate Steiner tree
     for each net and then converts all nonvertical and nonhor-
     izontal edges in the tree to L-shaped segments. A coarse
     global routing step determines the orientations of switch-
     able L-shaped segments (in order to minimize the total chan-
     nel density).  Next, specific feedthrough paths are as-
     signed.  Effort is made to minimize the horizontal spans of
     segments and to perfectly align the feedthroughs.  Results
     on number of tracks and CPU time (on SUN3/260) for PrimSC1,
     PrimSC2, PrimGA1 and PrimGA2 are reported and compared with
     TimberWolfSC version 4.2 global router.

Lokanathan, Badri and Edwin Kinnen, "Performance Optimized Floor
     Planning by Graph Planarization," _2_6_t_h _A_C_M/_I_E_E_E _D_e_s_i_g_n _A_u_t_o_-
     _m_a_t_i_o_n _C_o_n_f_e_r_e_n_c_e, pp. 116-121, ACM, Inc., New York, June
     1989.

     Placement and post-layout results for PrimBBL2 benchmark are
     shown. (All blocks have fixed dimensions and pins.) The
     floorplanning procedure, based on rectangular dualization,
     maximizes adjacency of modules that are heavily connected or
     are connected by critical nets.

Preas, Bryan, "Benchmarks for Cell-Based Layout Systems," _2_4_t_h
     _A_C_M/_I_E_E_E _D_e_s_i_g_n _A_u_t_o_m_a_t_i_o_n _C_o_n_f_e_r_e_n_c_e, pp. 319-320, ACM,
     Inc., New York, June 1987.

     This paper presents a representative set of cell-based lay-
     out benchmarks that are real and representative of modern
     designs.  Example benchmarks are provided to span the design
     space along several axes: regularity (a register array and
     irregular control logic), size (137 cells to 3804 cells),
     design style (gate arrays, standard cells, general cells).
     Layout rules for the benchmarks are specified.

Rose, Jonathan, "LocusRoute: A Parallel Global Router for Stan-
     dard Cells," _P_r_o_c. _o_f _2_5_t_h _A_C_M/_I_E_E_E _D_e_s_i_g_n _A_u_t_o_m_a_t_i_o_n
     _C_o_n_f_e_r_e_n_c_e, pp. 189-195, IEEE Computer Society Press, Wash-
     ington D.C., June, 1988.

     Results are cited on Primary1, Primary2 and Test06 from the
     standard cell benchmark set.

Sechen, Carl and Kai-Win Lee, "An Improved Simulated Annealing
     Algorithm for Row-Based Placement," _D_i_g_e_s_t _o_f _T_e_c_h_n_i_c_a_l _P_a_-
     _p_e_r_s _I_C_C_A_D-_8_7, pp. 478-481, IEEE Computer Society Press,
     Washington D.C., 1987.

     This paper presents TimberWolfSC version 4.2. The main
     features of the this version include problem normalization








                              - 4 -



     and negative-feedback control of the key simulated annealing
     parameters. Wire length and number of tracks for PrimSC1 and
     PrimSC2 are reported.

Sha, Lu and Tom Blank, "ATLAS - A Technique for Layout Using Ana-
     lytic Shapes," _D_i_g_e_s_t _o_f _T_e_c_h_n_i_c_a_l _P_a_p_e_r_s _I_C_C_A_D-_8_7, pp. 84-
     87, IEEE Computer Society Press, Washington D.C., 1987.

     This paper introduces a new objective function for building
     block placement which takes into account the translation,
     rotation, and mirror operations simultaneously.  Two methods
     (the Penalty Function Method and the Sequential Quadratic
     Programming) have been used to solve the resulting non-
     linear programming problem.  Wire length and CPU time (VAX
     Station II) for PrimBBL2 are reported and compared with
     those of TimberWolf version 3.2.

Suaris, Peter and Gershon Kedem, "A Quadrisection-Based Combined
     Place and Route Scheme for Standard Cells," _I_E_E_E _T_r_a_n_s_a_c_-
     _t_i_o_n_s _o_n _C_o_m_p_u_t_e_r-_A_i_d_e_d _D_e_s_i_g_n, vol. 8, No. 3, pp. 234-244,
     March, 1989.

     Area, wire length and CPU time are cited for Primary1 and
     Primary2 benchmark circuits.

Zhang, Xueqing, Lawrence T. Pillage, and Ronald A. Rohrer, "Effi-
     cient Final Placement Based on Nets-as-Points," _2_6_t_h
     _A_C_M/_I_E_E_E _D_e_s_i_g_n _A_u_t_o_m_a_t_i_o_n _C_o_n_f_e_r_e_n_c_e, pp. 578-581, ACM,
     Inc., New York, June 1989.

     This paper reports half-perimeter and minimum spanning tree
     wire lengths for PrimGA1 and PrimGA2 benchmarks.  Maximum
     horizontal and vertical congestions and CPU times are
     presented as well.  A pseudo Steiner tree model for the
     net-points, which selects the gate positions to best minim-
     ize the wiring congestion, along with a virtual snap-to-grid
     procedure , which places the gates in rows to best alleviate
     the wiring congestion of the channels, are used.

Zimmerman, Gerhard, "A New Area and Shape Function Estimation
     Technique for VLSI Layouts," _2_5_t_h _A_C_M/_I_E_E_E _D_e_s_i_g_n _A_u_t_o_m_a_t_i_o_n
     _C_o_n_f_e_r_e_n_c_e, pp. 60-65, IEEE Computer Society Press, Washing-
     ton D.C., June 1988.

     A shape function estimation method for standard cell and
     general cell layouts is presented.  The estimation technique
     relies on knowledge about the geometry and about cell neigh-
     borhoods in order to produce accurate area estimates. Shape
     function and area estimates for PrimSC1 benchmark over a
     wide range of aspect ratios










