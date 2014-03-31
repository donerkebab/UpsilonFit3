UpsilonFit3
===========

Markov chain Monte Carlo scan of the pMSSM parameter space, in order to evaluate the SUSY-Yukawa Sum Rule.  Based on work in [arXiv:1201.5839](http://arxiv.org/abs/1201.5839).

UpsilonFit3 is written by Mike Saelim, and is a complete rewrite of the UpsilonFit2 code he wrote for [arXiv:1201.5839](http://arxiv.org/abs/1201.5839).  It is written in C++, and requires the GNU Scientific Library (GSL) and SuSpect, a FORTRAN program written to calculate SUSY spectra.  It currently uses GSL version 1.16 and SuSpect version 2.43.  Developed in NetBeans 7.4.


**Interjection: The physics part of the code (package UpsilonFit3) is not done yet, but you can see plots of the MCMC code at work on toy problems, in ToyScan1_plots.pdf and ToyScan2_plots.pdf!**


The MCMC algorithm is an adaptive Metropolis-Hastings algorithm with simulated annealing, based on the work in [arXiv:hep-ph/0602187](http://arxiv.org/abs/hep-ph/0602187).  It is adaptive because the step size changes to reflect the estimated size of the posterior distribution.  Note that, while GSL has a simulated annealing package, we found it very limited.





Structure
---------

It has three main parts:

1. The Mcmc package, which implements a general Markov chain Monte Carlo scan over a parameter space, given generic constraints.  Class McmcScan is abstract, and requires a subclass to implement key functions.

2. The ToyScans package, which implements scans on small toy problems as tests/examples of the MCMC code at work.

3. The UpsilonFit3 package, which uses the Mcmc package to implement the scan over a subspace of the phenomenological Minimal Supersymmetric Standard Model (pMSSM) parameter space, constrained by a set of measurements made at the Large Hadron Collider (LHC) or a future linear electron-positron collider like the International Linear Collider (ILC).  Its main class, PmssmScan, inherits from Mcmc::McmcScan.  It also calculates the Upsilon parameter, defined in the SUSY-Yukawa Sum Rule ([arXiv:1004.5350](http://arxiv.org/abs/1004.5350)), for the resulting posterior distribution.








Acknowledgments
---------------
Mike Saelim would like to thank 
* Maxim Perelstein for the original project and his guidance over the years, and
* Curran Muhlberger for his expert development and programming advice.

**SuSpect** is written by Abdelhak Djouadi, Jean-Loic Kneur, and Gilbert Moultaka: [website](http://www.coulomb.univ-montp2.fr/perso/jean-loic.kneur/Suspect/), [arXiv:hep-ph/0211331](http://arxiv.org/abs/hep-ph/0211331).
