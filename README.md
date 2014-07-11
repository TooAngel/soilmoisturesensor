Soil moisture sensor
=========

Soil Moisture Sensor Probe Wiring Table

Bare    Ground
Red     POWER:  3.3V to 20 VDC.
Black   OUT: (0 to 3V related  to moisture content.)


Soil moisture (SM)
Raspberry PI (RPI)

SM Ground -> Steckbrett -> Stecklitze -> RPI Ground
SM 3.3 V -> Steckbrett -> Stecklitze -> -> RPI 3.3 V
SM OUT -> Steckbrett -> Stecklitze -> -> RPI GPI


Homemade
========

http://gardenbot.org/howTo/soilMoisture/

galvanized steel wire -- 12 gage or equivalent (2 mm stahl draht)
packing foam block (e.g. inside a product box for home electronics) -- the slightly flexible kind is better (less brittle than styrofoam)
soldering iron and solder
lead wires



TODO
=====
Make image to text


Arduino
=====

    ino build
    ino upload


Compare
=====
With FOM/mts - Field Operated Meter
Date      |    this  | FOM
------------------------------
2014-07-03 19:00 |   227  | 9%
2014-07-04 09:00 |   287  | 19,2%
2014-07-04 09:30 |   271  | 12,5%
2014-07-04 10:00 |   268  | 10,8%
2014-07-04 10:13 |   356  | 22,9%
2014-07-06 20:44 |   262  | 10,5%
2014-07-09 08:40 |   352?  | 17,3%
2014-07-09 18:48 |   326  | 11,7%
2014-07-11 07:54 |   267  | 3,5%
2014-07-11 08:09 |   299  | 13,6%
2014-07-11 08:22 |   359  | 24,0%


