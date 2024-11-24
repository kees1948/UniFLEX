2021-06-28:

After a few iterations I arrived at the final version for this board

![CPUXXCMI board](./20210629_084739a.jpg)

See 'description' for the details

One special Note: The orientation of U17 is OPPOSITE the the regular
orientation of the other IC's. !!!

Errata, prior to version 1.1 there were some errors around U17.

CPUXXCMI_210629.zip is version 1.1

[202411-23]
In last momths it was found that a 'real' MC68X09 in the CPUXXXCMI
did work not work. The reason was that the X-Tal oscillator inside 
the CPU chip failed to run.
A simple solution for this: apply a resistor of 10MOhm across the
X-Tal pins. The best here is a SMT resistor.
With this resistor, MC68X09, MC68X02 and HD63X09 chips work fine.
 


