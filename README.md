# SQLC - Multi function display for the DGIIIAI Delta Glider.

[![DOI](https://zenodo.org/badge/202592309.svg)](https://zenodo.org/badge/latestdoi/202592309)


## Overview:

Multi function display (MFD) designed to interact with the onboard expert 
system that runs the DGIIIAI Delta Glider for Orbiter Space Flight Simulator.


## Dependencies:

* Orbiter Space Flight Simulator 2010 (OSFS) or newer 
- http://orbit.medphys.ucl.ac.uk/

* MS Visual Studio 2010 or later, with OpenMP compatibility.

* Sqlite3 - https://www.sqlite.org/index.html

* DGIIIAI - https://github.com/PESchoenberg/DGIIIAI.git


## Installation:

* Backup your original Orbiter installation or install a new one before
installation.

* Carefully review the contents of the compressed file of this project
in order to see what will be copied.

* Make sure that you already have installed DGIIIAI

* Copy the contents of \orbiter folder into your Orbiter simulator main folder.


## Uninstall:

* You will have to manually remove the files provided. Therefore it is
advisable that you install this project on a development installation of
Orbiter instead of you main one.


## Usage:

* Enable this MFD as any other standard MFD or dll for OSFS.

* Load one of the provided scenarios. The ship will soon start acting on its
own.

* This MFD requires that a Sqlite3 database is installed and has been specially
designed to work with DGIIIAI. It is not compatible with other ships for OSFS.


## Related projects:

* gexsys - https://github.com/PESchoenberg/gexsys - This is a Scheme-based 
expert system developed directly from DGIIIAI. There are some differences 
between both, being the main one that the system installed on DGIIIAI is 
specific for this space ship, while gexsys is mre general-purpose, but broadly 
speaking they work very similarly. Thus, you could use both for double feedback 
and testing.


## Credits and Sources:

* Schweiger, M. (2000). Orbiter 2016 Space Flight Simulator. [online]
Orbit.medphys.ucl.ac.uk. Available at: http://orbit.medphys.ucl.ac.uk/
[Accessed 26 Aug. 2019].

* Credits for the original DGIII and MFD template files are kept as in the
original versions.

* Please notify me in case that I have missed some credits.



## License:

* Please read the contents of file COPYING for license info.


