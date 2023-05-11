<CsoundSynthesizer>
<CsOptions>
-n -d
</CsOptions>
<CsInstruments>
; Initialize the global variables. 

    0dbfs=1
    
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
            a1 = atpsL
            a2 = atpsR
    outs atpsL, atpsR
    endin

</CsInstruments>
<CsScore>
f0 z
i1 0 z
</CsScore>
</CsoundSynthesizer>