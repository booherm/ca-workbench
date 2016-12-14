# CA Workbench

CA Workbench is a utility for exploring computational complexity through networks of connected agents.  Such models include Cellular Automata (CA),
Random Boolean Networks (RBNs), and Recurrent Neural Networks (RNNs).

## One Dimensional CA Module
[Math World: 1D Cellular Automata](http://mathworld.wolfram.com/ElementaryCellularAutomaton.html)

The 1D CA module supports generation of basic 2 state (k = 2) CAs with variable neighborhood sizes.
For each neighborhood count n, there are k^(k^n) possible rules.  n = 3 yeilds the Wolfram elementary 1D CAs: 2^(2^3) = 256 rules.  Each row of the output
represents the evolution of the previous row through one time step.  With extremely simple update rules, unpredictable complex patterns can emerge which
posess a striking mixture of order and chaos.

A UI has not yet been added for this module.  Keyboard controls:

* Left: decrement neighborhood size
* Right: increment neighborhood size
* Up: increment active rule number
* Down: decrement active rule number

Rule 18:
![Rule 18](http://i.imgur.com/AyrGb7i.png)

Rule 30:
![Rule 30](http://i.imgur.com/fzd3Zvg.png)

Rule 45:
![Rule 45](http://i.imgur.com/19XWUkX.png)

## Two Dimensional CA Module
[Wikipedia: Cellular automaton](https://en.wikipedia.org/wiki/Cellular_automaton)

The extension of 1D CA, 2D CA update the entire lattice of cells on each time step, which can be viewed as a movie.


[![2D CA](http://i.imgur.com/t5ieSWu.png)](https://youtu.be/9Z-a_6BgTa8?t=0s "2D CA")



## Random Boolean Network Module
[Wikipedia: Random Boolean Networks](https://en.wikipedia.org/wiki/Boolean_network)

Kauffman style random boolean networks.  Demonstrates emergent self organization of boundary structures and critical thresholds.

[![RBN](http://i.imgur.com/uVjaHxN.png)](https://youtu.be/Kz4qXHlr0is?t=0s "RBN")

A UI has not yet been added for this module.  Keyboard controls:

* Space: reset initial site states
* R: update input site states to random values
* F: feed forward
* T: toggle auto feed forward
* A: toggle auto new input on cycle detection
* Up: increment site connectivity
* Down: decrement site connectivity
* Left: set neighborhood connections off
* Right: set neighborhood connections on
* Keypad 1: decrement external input rows (blue rows)
* Keypad 7: increment external input rows (blue rows)
* Keypad 2: decrement feedback input rows (green rows)
* Keypad 8: increment feedback input rows (green rows)
* Keypad 3: decrement external output rows
* Keypad 9: increment external output rows
* Insert: shift input data
* I: toggle fading out of inactive sites

## Recurrent Neural Network Module
[Wikipedia: Recurrent Neural Network](https://en.wikipedia.org/wiki/Recurrent_neural_network)

Recurrent Neural Network as described by John Holland in "Emergence: From Chaos to Order", 1998
