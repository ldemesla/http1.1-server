# http1.1-server
HTTP 1.1 conditionally compliant server written in C++

## Compiling
Run `make`.

## Running
Run `./webserver [config_file]`. If you don't precise a configuration file, the server
will launch the default presentation website.

## Configuration file
Take a look at `data/default/default.conf`, where all the parameters
are shown. The only rule is to have at least one server (each server must
have at least a port field).

## License
This project is licensed under the GNU General Public License 3.
