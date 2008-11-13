step to create or update this directory from the mozilla tree:

- run autoconfig214
- run configure
- run make

- move all files from nanojit subdirectory to this directory
- move jsautocfg.h and js-config.h from config directory to thsi directory

- remove config, dist, editline, liveconnect, vprof and xpconnect  directories

- remove jscpucfg.cpp jskwgen.cpp jsoplengen.cpp

- move js.cpp to ../jstapp directory to have a shell for testing

- put our build.config directory with the proper options inside

- now acant should build the lib
- go to the ../jstapp directory, build the shell with acant
- go back here and run the correct-ac.sh script to test if everything is fine
