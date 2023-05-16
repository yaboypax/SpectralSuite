<CsoundSynthesizer>
<CsOptions>
-odac
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

instr 1

kdeltime linseg 0.1, p3, 0.5 ; Delay time
asig in
adel delayr 1.0           ; Maximum delay time
adlyl delayw asig         ; Write to delay line
aoutl deltap3 kdeltime    ; Read from delay line
aoutr = asig
outs aoutl, aoutr

endin

</CsInstruments>
<CsScore>

f 0 86400
e

</CsScore>
</CsoundSynthesizer>
