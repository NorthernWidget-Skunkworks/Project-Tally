[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2525460.svg)](https://doi.org/10.5281/zenodo.2525460)

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

### Connectivity:
Device can easily be interfaced to using I2C at either 3.3v or 5v (logic and power). 

A library has been developed for interfacing with Arduino based devices:
[Tally Library](https://github.com/NorthernWidget-Skunkworks/Tally_Library)
### Power Consumption: 

| Mode | Current @ 3.3v | 
| ---- | ------------- |
| Default | ~4mA |
| Sleep | <100uA |
| Switched | N/A<sup>1</sup> |

<sup>1</sup>When the device is disconnected from the power bus, it draws no external power and runs off the on board super cap. Tested in pull up mode with 100Hz, 50% duty cycle switching input. Ran for 30 minutes, resulting in a consumption of ~ 0.275J of energy = 7.64e-5 Watt Hours -> 15.26 uW avg -> ~ 5uA @ 3v avg. 

#### Maximum input frequency: 

| Mode | Max Freq [kHz] | 
| ---- | ------------- |
| Pull Up | 16 |
| Pull Down | 10 |
| Direct, Square | 9 |
| Direct, Sine | 22 |
| Direct, Triangle | 22 |

Measured using a function generator with a 2.5v amplitude for push pull tests, and driving an appropriately selected MOSFET for the pull up and pull down variations to act as an analogous switch, once again driven by a function generator. 

---


> ##### ***"Errors using inadequate data are much less than those using no data at all."***
> - **[Charles Babbage](https://en.wikipedia.org/wiki/Charles_Babbage)**



**Concept and design by [Bobby Schulz](https://github.com/bschulz1701) at Northern Widget**

<br>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
