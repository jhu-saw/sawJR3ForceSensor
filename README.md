sawJR3ForceSensor
=================

#### License
CISST License:  
https://github.com/jhu-cisst/cisst/blob/master/license.txt

#### Hardware 
* JR3 (Model number ?)
* PCI Card: comes with JR3 force sensor

#### Driver 
**Build**   
sawJR3ForceSensor depends on CISST library.  
The easist way is to get cisst-saw. Turn on 
```sawJR3ForceSensor``` flag in cmake and build. 

**Example** 


**Supported System** 
* Linux with Comedi 
* QNX (Not sure check with Min)

**Notes** 
* JR3 uses left-hand coordinate frame and US unit
* Reported FT in SI unit 

#### TODO
* Add Xenomai Support 
  * http://xenomai.org/analogy-general-presentation/
* ROS Support & Example
