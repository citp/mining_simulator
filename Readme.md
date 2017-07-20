Bitcoin Mining Simulator
=======

This is a simulator intended for testing non-default bitcoin mining strategies.

Installation
------------

The Bitcoin Mining Simulator has been developed and tested on Mac OS X 10.11.4.
This software is currently setup to be run using Apple's Xcode IDE.

The code is dependent on the GNU Scientific Library (https://www.gnu.org/software/gsl/).
The xcode project file expects the headers to be located at /usr/local/include and the
library files to be located at /usr/local/lib so these must be updated if your
configuration differs.

The code for the mining simulator is written in standard C++14. In the future we will
release this code with a makefile so that it is easily compilable in a
cross-platform manner.

Usage
-----------

main.cpp inside of the StratSim folder is the main code example
for usage of this simulator.

The code is generally designed around the idea of mining strategies. Each
mining strategy prescribes set behaviors for a mining. We then create a set of
miners with configurable hash rate, cost per hash, and network latency. These
miners then compete to mine on a simulated blockchain with configurable block
reward and transaction fee accumulation rate.

In the future we will release more documentation and code examples for usage
of this software.

Output of Example Code
------------------------
StratSim produces a folder which includes a file for each miner that is competing
to mine blocks. This simulation works by running a series of simulations where
miners choose to use the strategy that they believe to be most effective. Each
output file gives a list of the percentage of miners who used the given strategy
in each round. This demonstrates the idea that miners will either all converge to
an optimal strategy or oscillate between strategies when there is no equilibrium.

SelfishSim evalutates the effectiveness of the selfish mining strategy. It
produces a single file which contains the percentage profit earned by the
selfish miner given the percentage of hash power which it controls. 

Disclaimer
-----------

Note that the Bitcoin Mining Simulator is under active development, and should
be considered experimental software.

Although the Bitcoin Mining Simulator is actively used by our group for research
studies, it is still possible there are unknown bugs in the simulator. We plan on 
writing comprehensive tests to verify the results of all included components.
Prior to using the Bitcoin Mining Simulator for your own research we encourage
you to write tests (and submit pull requests!) for any testing that isn't
currently included in our test scripts.

License
-------

Bitcoin Mining Simulator is licensed under GNU GPLv3. Additional code has been included from
[arithmetic_type](https://github.com/gnzlbg/arithmetic_type) which is licensed Boost Software License Version 1.0.
