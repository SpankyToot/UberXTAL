# UberXTAL
A joint project by VajskiDs and SpankyToot<br />
Combining existing projects by VajskiDs, PSXTAL and tehUberChip.<br />
Arduino Nano based PSX modchip and with the combination of a DFO board from VajskiDs, also an automatic video output clock switcher.<br />
<br />
## Installation<br />
You can find reference images of each model, along with its pinout, in its own directory inside the Installation directory.<br />
There is also a list of tested PSX models inside the Installation directory.<br />
<br />
## Compatibility<br />
Currently tested working on PU-18 PSX, but should work on any PSX revision PU-20 or lower.<br />
Goal is to expand compatibility to all PSX models.<br />
<br />
### Revisions
V1.00: Started adding the PSXTAL components to Uberchip V2.1<br />
V1.01: Used updated UberChip version 'PU-18 with region select' from VajSkid and did some cleaning of existing combined code to suit the new code.<br />
V1.02: Add serial output for debugging, start work on PSX revision options<br />
V1.03: Added a lot of debugging and changed to 5V power source from 3.5V to increase stability Now using 3.5V as a trigger for debug. Also reorganized a few things for cleanliness and stability.<br />
V1.10: Got it all working, changed XTAL power delivery as there was too little power given to active crystal and too much to inactive. Housecleaning to improve human readability.<br />
