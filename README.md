# PARSIFAL

## Authors
<ul>
<li>Riccardo Farinelli <rfarinelli@fe.infn.it></li>
<li>Lia Lavezzi        <lia.lavezzi@to.infn.it></li>
</ul>
All rights reserved

## License
For the licensing terms see $PARSIFAL/LICENSE

## ChangeLog

## REQUIREMENTS
A working installation of <a href="https://root.cern.ch/">ROOT</a>. Tested on ROOT version 6.12/04

## HOWTO
<ul>
<li> set the ROOT environment variables:<code>source $ROOTSYS/bin/thisroot.sh</code></li>
<li> go to the directory where you copied PARSIFAL</li>
<li> initialize PARSIFAL with the command: <code>source init_PARSIFAL.sh</code></li>
<li> set the PARSIFAL environment variables: <code>source $PARSIFAL/bash_PARSIFAL2</code></li>
<li> get the PARSIFAL command list: <code>parsifal -h</code></li>
<li> compile the code: <code>parsifal -m</code></li>
</ul>

### Example
Run the command: <code>parsifal -S 1 0 </code></br>

It runs the configuration 1 contained in run_list.txt and generates muon tracks with incident angle 0 degrees with respect to the direction orthogonal to the detector plane. </br>
In run_list.txt, configuration 1 is:
<code>1 13.0 1.2 1.0</code>
which are respectively the run number, the tuning factor on the gain, the tuning factor on the spatial diffusion, the tuning factor on the temporal diffusion.

## LIST OF VARIABLES AND THEIR DESCRIPTION
The code needs a long list of parameters: the simulation strongly depends on these parameters and an evaluation of these values allows the user to simulate different MPGD configurations. In addition to the parameters needed for the simlation, there is a another set of variables used to run the code and apply the approximations needed to speed it up. In order to modify each of the two sets of parameters, the user needs a deep knowledge of the code.

### List of parametrization variables 
In the file 'PARSIFAL/src/Common/Common.h', the following variables are used to parametrize the APV electronics.</br>
<code>tau_APV, jitter_APV, timebin_APV, timestep_APV, noise_APV, thr_APV, fC_to_ADC, ele_to_fC </code></br>
</br>
In the file 'PARSIFAL/src/DetectorGain/DetectorGain.h', the following variables are used to parametrize the GEM gain properties.</br>
<code>collection_efficiency_stage1, extraction_efficiency_stage1, collection_efficiency_stage2, extraction_efficiency_stage2, collection_efficiency_stage3, extraction_efficiency_stage3, h_gain_eff, f_gain_eff </code></br>
</br>
In the file 'PARSIFAL/src/ElectronDrift/ElectronDrift.h', the following variables are used to parametrize the electron diffusion.</br>
<code>SpatialShiftDrift, SpatialShiftTrans, SpatialShiftInduc, SpatialDiffusionDrift, SpatialDiffusionTrans, SpatialDiffusionInduc, SpatialShiftDriftMagField, SpatialShiftTransMagField, SpatialShiftInducMagField, SpatialDiffusionDriftMagField, SpatialDiffusionTransMagField, SpatialDiffusionInducMagField, TemporalShiftDriftMagField, TemporalShiftTransMagField, TemporalShiftInducMagField, TemporalDiffusionDriftMagField, TemporalDiffusionTransMagField, TemporalDiffusionInducMagField</code></br>
</br>
In the file 'PARSIFAL/src/Particle/Particle.h', the following variables are used to parametrize the ionization.</br>
<code>Set_MeanPrimary, prob_electron_per_cluster</code></br>
</br>

### List of code variables 
In the file 'PARSIFAL/src/Common/Common.h', the following varialbes are used to enable of disable section of the code.</br> 
<code>NO_PrimaryIonization, NO_SecondaryIonization, NO_Gain, NO_Drift, NO_Resistive, NO_Readout, NO_Noise, NO_Capacitive, NO_uTPC_Correction, NO_Hit, NO_Cluster, Bfield</code></br>
For a typical simulation in PARSIFAL, they should be set 'true'. If some of those variable are set 'false', then the output of the code is strongly affected: i.e. if no ionization occurs, then there are no electrons to diffuse or to amplify and only noise will be readout by the electronics.</br>
</br>
In the file 'PARSIFAL/src/Common/Common.h', the following variable is used to speed up the code.</br>
<code>fast_simulation</code></br>
If it is 'true' then the 'Signal' class is used and it produce the readout signal starting from the primary ionization; if 'false' then the same output is produced through the classes 'ElectronDrift', 'DetectorGain' and 'Readoout'. The 'false' configuration produces two collections of interest, 'Secondary' and 'Channel' where more information on the produced simulation is included.</br>
</br>
In the file 'PARSIFAL/src/Signal/Signal.C' the following variable is used to speed up the code.</br>
<code>gain_speedup</code></br>
This variable reduces the loops on the electron multiplied by the MPGD by a factor 'gain_speedup' and it assign the same weigth to the induction of each electron.</br>

## Future upgrades
Up to now, the settings of the triple-GEM are introduced by the variable setup (=0)
This determines the following variables:
<ul>
<li> collection efficiency &times 3</li>
<li> extraction efficiency &times 3</li>
<li> GEM gain &times 3</li>
</ul> 
These variables depend on the gain of the detector </br>
&rarr; the check on the gain file is missing

The ionization depends on the particle, now only muons are considered. </br>
This determines the following variables:
<ul>
<li> number of primary clusters of electrons</li>
<li> number of secondary electrons for each primary cluster</li>
</ul>
These variables depend on the particle type, its kinetic energy and the gas mixture
