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
### 2021.04.19
Implemented the uRWELL simulation w/o resistive
### 2021.04.27
Implemented the resistive package w/o any formula
### 2024.08.30
Implementato una funzione in ReadOut chiamata Injection_External_Signal per generare un onda quadra sull'istogramma di corrente
### 2024.10.14
Implemented White noise, ion tail, time information on the cluster
### 2024.11.22
Added flag on channels above threshold and channel signal lenght
### 2024.12.18
Added noise amplitude tuning with sigma noise fC expected from with noise simulation
### 2025.02.03
now µRWELL 1° electron diffusion and 2° gain while GEM 1° gain and 2° diffusion
signal induction splitted in fast and slow keeping unitary total charge IT_signal_ratio_fast_slow
modified run_list.txt to increase the number of custom parameters
### 2025.02.13
modified APV channel above threshold sigma_noise->sigma_noise/2 + charge_cut
### 2025.03.14
modified noise (1/f) to white noise
included buffer current to improbe noise at t=0
updated the electron drift parameter in µRWELL
now noise is evaluated for each channel (slower sim)
### 2025.03.19
updated calibration white noise fixed amplitude w/ shaping time
charged resistive model from Morello to T2K
### 2025-05-10
modified TIGER saturation
### 2025-05-21
included noise TIGER on E Branch (prev was fixed to0.1fC)
### 2025-07-03
parallelized the code with OpenMP
### 2025-08-08
reduced the freq range in the noise. modifield the white noise
moved from Morello to T2K resistive model	
### 2025.10.15
modified noise calibrations
