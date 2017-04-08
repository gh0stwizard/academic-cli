# academic-cli


## Description

This is a command line utility to the [Academic Dictionaries and Encyclopedias][1]
website.

![Screenshot](/misc/screenshot.png)


## Dependencies

* [libuv][2]
* [curl][3]
* [sqlite3][6]
* [myhtml][4] - you have to update the submodule in this git repo.


## Features

At the moment, not much. There are big plans about that.

* 48 English-Russian dictionaries
* 17 Russian-English dictionaries
* 2 Chinese-Russian dictionaries
* 1 Russian-Chinese dictionary
* 1 English-Chinese dictionary


## Usage

```
shell> ./academic-cli --help
Usage:
academic-cli: -d ID1 [-d ID2...] [options] word1 word2 ... wordN
Options:
  -C CONNECT-TIMEOUT           A connection timeout in seconds.
  --dictionary ID, -d ID       Use this dictionary ID.
  -D RANGE                     Use range of dictionary IDs, e.g. "1-5,7,12".
  --help, -h, -?               Display this information.
  --list TYPE, -l TYPE         Display dictionary IDs by type. See --list-types.
  --language LANG, -L LANG     Choose output language. Default "en".
  --list-types, -T             Display dictionary types.
  --retries NUM, -r NUM        How many times to retry establish a connection.
  --retry-timeout MS, -t MS    Delay between connection retries in milliseconds.
  --quite, -q                  Be quite: print out valueable information only.
  --version, -v                Display version information.
```


## Build

```
shell> # first, build mythml
shell> make myhtml
shell> make
```


## Credits

[jsmn][5] - JSON parser by Serge A. Zaitsev


## License

    academic-cli -- a command line utility for ACADEMIC website.
    Copyright (C) 2017  Vitaliy V. Tokarev

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


[1]: http://www.enacademic.com/
[2]: http://libuv.org/
[3]: https://curl.haxx.se/
[4]: https://github.com/lexborisov/myhtml
[5]: https://github.com/zserge/jsmn
[6]: http://sqlite.org/
