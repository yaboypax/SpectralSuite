<CsoundSynthesizer>
<CsOptions>
-d -n
</CsOptions>
<CsInstruments>
    0dbfs=1
    nchnls=2
	nchnls_i=2
	
        instr 1
        a1 inch 1
        a2 inch 2
        kshift chnget "shift"
        klowest=10
            fsiginL pvsanal   a1, 1024, 256, 1024, 1
            ftpsL pvshift fsiginL, kshift, klowest
            atpsL pvsynth   ftpsL

            fsiginR pvsanal   a2, 1024, 256, 1024, 1
            ftpsR pvshift fsiginR, kshift, klowest
            atpsR pvsynth   ftpsR
            
    outs atpsL,atpsR
    endin

</CsInstruments>
<CsScore>
f 0 z
i 1 0 z
</CsScore>
</CsoundSynthesizer>