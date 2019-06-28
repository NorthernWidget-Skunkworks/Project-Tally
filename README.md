# Project-Tally
A minimal ultra low power digital event counter, designed to be a counterpart to a traditional data logger to measure and average rapid events like an anemometer reading

![](Tally_Top.png?raw=true "TallyBoardImage")

## Features:
- On board super capacitor (can be self powered between readings/recharge) 
- Extreme low power 
- 3.3/5v capable 
- I2C interface
- Measure up to 65536 (2^16) events before rollover
- Integrated input pullup/pulldown
- Integrated input conditioning (allows for non-binary inputs)

Using jumpers on bottom side of board, can configure input as pullup, pulldown (ultra low power mode), or direct input if driving from another device

#### Maximum input frequency: 

| Mode | Max Freq [kHz] | 
| ---- | ------------- |
| Pull Up | 16 |
| Pull Down | 10 |
| Direct, Square | 13 |
| Direct, Sine | 22 |
| Direct, Triangle | 22 |

---


> ##### ***"Errors using inadequate data are much less than those using no data at all."***
> - **[Charles Babbage](https://en.wikipedia.org/wiki/Charles_Babbage)**



**Concept and design by [Bobby Schulz](https://github.com/bschulz1701) at Northern Widget**

<br>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.