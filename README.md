# SiViS
New methods for C. elegans assays lifespan automation have been evaluated, these are for improving C. elegans deteccion, and these are described in research article [1].
In addition, other methods are used for vision system inspection [2] and for c-elegans detection [3], all of these are evaluated.

### Research article [1]
[1] "A small and flexible machine for monitoring Caenorhabditis elegans based on active vision and image processing techniques: Lifespan validation" REVIEW XXXXxxxxx
Copyright (c) 2020 UPV. J.Puchalt, A.Sánchez, E.Ivorra, R.Martínez, S.Genovés and P.Martorell

### Research article [2]
[2] Puchalt JC, Sánchez-Salmerón AJ, Martorell Guerola P, Genovés Martínez S (2019) Active backlight for automating visual monitoring: An analysis of a lighting control technique for Caenorhabditis elegans cultured on standard Petri plates. PLOS ONE 14(4): e0215548. https://doi.org/10.1371/journal.pone.0215548

### Research article [3]
[3] Puchalt, J.C., Sánchez-Salmerón, A., Ivorra, E. et al. Improving lifespan automation for Caenorhabditis elegans by using image processing and a post-processing adaptive data filter. Sci Rep 10, 8729 (2020). https://doi.org/10.1038/s41598-020-65619-4

#### Example assemby and code on [2]
* It is described in document https://github.com/JCPuchalt/c-elegans_smartLight

#### Example code on [3]
* It is described in document https://github.com/AntonioJoseSanchezSalmeron/lifespan

#### Hardware description on [1]
In this repository are the .stl files to print in 3D printer. 
* bridge.stl
* door.stl
* filter.stl
* lid.stl
* pre_rack.stl
* rack.stl
* electronicBox.stl
* displayBox.stl
* frame.stl

Then assemby vibration system and adjust to vision system as follow 
https://github.com/JCPuchalt/SiViS/blob/master/AssemblyManual.pdf

#### Pallet assembly
These are the parts:
* pallet_rigidPart.stl
* pallet_middlePart.stl
* pallet_flexiblePart.stl

Assembly the pallet part as follow https://github.com/JCPuchalt/SiViS/blob/master/AssemblyPallet.pdf

## The Code
You shall download this repository on Ubuntu 18.04 machine in the folder you wish. 
Once download, open a terminal and set in the SiViS folder.
### CaptureCE_App
Then follow the README.md file of Code/CaptureCE_App folder.
### ProcessorCE_App
Then follow the README.md file of Code/ProcessorCE_App folder.
### SiViS_App
Then follow the README.md file of Code/SiViS_App folder.


