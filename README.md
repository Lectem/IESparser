# IESparser
A public Granado espada ies files parser


Usage : 
IESParser file.ies tablename
or
IESParser folder tablename

Not really meant to used by other people but it might be useful...


## How to compile for those who never used did before
###Requirements :
* MinGW + MSYS or any other compiler
* CMake
* Git (obviously)

1. start a cmd prompt
2. git clone https://github.com/Lectem/IESparser.git
3. cd IESparser
4. mkdir build
5. cd build
6. cmake .. -G"MSYS Makefiles"
7. make