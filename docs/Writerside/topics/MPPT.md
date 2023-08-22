# MPPT (Maximum Power Point Tracking)

## Introduction to MPPT

MPPT, or Maximum Power Point Tracking, is a method used to optimize the amount of photovoltaic power that's extracted under certain conditions. It's essential in applications where the efficiency of power extraction is critical, such as in a solar panel battery bank.

## Why MPPT?

When working with a solar panel battery bank, it's imperative to ensure that the solar panel operates at its optimal power point, maximizing the energy harnessed. This is where MPPT comes into play. MPPT algorithms help track the Maximum Power Point (MPP) and adjust the operating conditions to ensure the solar panel is always functioning at this point.

## The Incremental Conductance Algorithm (IncCond)

We utilized the Incremental Conductance (IncCond) algorithm for our project. This algorithm's foundation is based on the power curve derivative of the solar panel concerning voltage:

- At the Maximum Power Point (MPP), dP/dV = 0.
- To the left of the MPP, dP/dV > 0.
- To the right of the MPP, dP/dV < 0.

This can be further deduced as:
dP/dV = d(IV)/dV = I + V * (dI/dV)

From the information provided:
- At the MPP: dI/dV = -I/V
- Left of the MPP: dI/dV > -I/V
- Right of the MPP: dI/dV < -I/V

The crux of the IncCond algorithm is comparing the incremental conductance (change in I over change in V) to the instantaneous conductance (I/V). Depending on this comparison, the panel's operating voltage is either increased or decreased until the MPP is achieved. Unlike other algorithms, such as the Perturb & Observe (P&O), which oscillates around the MPP, the Incremental Conductance algorithm stabilizes the operating voltage once the correct value is attained. If there's a shift in the panel current, the MPP tracking restarts. Depending on environmental factors, the same outcome can be achieved using the initial equation dP/dV.

## INCCOND Algorithm
Below is the representation of the INCCOND algorithm:
[![](https://mermaid.ink/img/pako:eNqNk_9qwjAQx18lBEo3sOj8s7iOgWPkj21lSmFY_4jpqcWaljTdGLVv4XP5TEtSlfpr2v7Ru-Rzd9-7ciVmaQTYxZZVopjH0kUlsqdJ-sPmVEjbuKwQ36BMO5eQPU8lCBtV-rWskId8Jmg2R8O-tpF6cqki70YhHmgjxON75DgeYjRho806QI8o6OXFxFv02vqDnJ3vPGxPehPR9tBmTRRLjlhygfUV6x-x_iE73inUUmpNc2CLoKxVdZ6q7bU51cDqPV3Vrq8pH7XRZfYL8i1Mylq7oRqc3wAFyELwFx6d3jeKvg47jbreUV1zvw-JgAWjPjABNAf0kYGgMuYzFKSJpDMY14GaMr3vBehf9Wkc_a_OpjeKY67yE34lv6b-z9-oQA77JXW75KRRcmZuF2WTE9Xd22R3b5wLOZ5697axX8mPW3gJYknjSO1jqcNCLOewhBC7yoxgSotE7VPIK4XSQqaDX86wK0UBLVxkEZXQj6laxyV2pzTJ1SlEsUzFW73jZtWrPxxZScY?type=png)](https://mermaid.live/edit#pako:eNqNk_9qwjAQx18lBEo3sOj8s7iOgWPkj21lSmFY_4jpqcWaljTdGLVv4XP5TEtSlfpr2v7Ru-Rzd9-7ciVmaQTYxZZVopjH0kUlsqdJ-sPmVEjbuKwQ36BMO5eQPU8lCBtV-rWskId8Jmg2R8O-tpF6cqki70YhHmgjxON75DgeYjRho806QI8o6OXFxFv02vqDnJ3vPGxPehPR9tBmTRRLjlhygfUV6x-x_iE73inUUmpNc2CLoKxVdZ6q7bU51cDqPV3Vrq8pH7XRZfYL8i1Mylq7oRqc3wAFyELwFx6d3jeKvg47jbreUV1zvw-JgAWjPjABNAf0kYGgMuYzFKSJpDMY14GaMr3vBehf9Wkc_a_OpjeKY67yE34lv6b-z9-oQA77JXW75KRRcmZuF2WTE9Xd22R3b5wLOZ5697axX8mPW3gJYknjSO1jqcNCLOewhBC7yoxgSotE7VPIK4XSQqaDX86wK0UBLVxkEZXQj6laxyV2pzTJ1SlEsUzFW73jZtWrPxxZScY)


<details>
  <summary>Mermaid Code</summary>
```mermaid
%%{ init: { 'flowchart': { 'curve': 'stepAfter' } } }%%

graph TD

    start(["Start"]) --> calc[ΔV = V<sub>k</sub> - V<sub>k-1</sub> <br/> ΔI = I<sub>k</sub> - I<sub>k-1</sub> <br/> ΔP = P<sub>k</sub> - P<sub>k-1</sub>]

    calc --> checkV{ΔV = 0?}
    checkV -->|No| checkPV{ΔP / ΔV = 0?}
    checkV -->|Yes| checkI{ΔI = 0?}

    checkPV -->|Yes| returnEnd(["Return"])
    checkPV -->|No| checkPVGT0{ΔP / ΔV > 0?}
    checkPVGT0 -->|No| decV[Decrease Operating Voltage]
    decV --> returnEnd
    checkPVGT0 -->|Yes| incV[Increase Operating Voltage]
    incV --> returnEnd

    checkI -->|No| checkIGT0{ΔI > 0?}
    checkI -->|Yes| returnEnd
    checkIGT0 -->|Yes| incV2[Increase Operating Voltage]
    incV2 --> returnEnd
    checkIGT0 -->|No| decV2[Decrease Operating Voltage]
    decV2 --> returnEnd

</details>


## Citation

[Practical guide to implementing solar panel MPPT algorithms](https://ww1.microchip.com/downloads/en/appnotes/00001521a.pdf).
