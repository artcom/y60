step to create or update this directory from the mozilla tree:

- run autoconfig214
- run configure
- run make

- move all files from nanojit subdirectory to this directory
- move jsautocfg.h and js-config.h from config directory to thsi directory

- remove config, dist, editline, liveconnect, vprof and xpconnect  directories

- put our build.config directory with the proper options inside

- now acant should build the lib
