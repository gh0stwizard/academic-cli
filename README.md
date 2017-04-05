# academic-cli


## Description

This is a command line utility to the [Academic Dictionaries and Encyclopedias][1]
website.


## Dependencies

* [libuv][2]
* [curl][3]
* [myhtml][4] - you have to update the submodule in this git repo.


## Features

At the moment, not much. There are big plans about that.


## Usage

```
shell> ./academic-cli --help
Usage:
academic-cli: [options] word1 word2 ... wordN
Options:
  --dictionary ID, -d ID   Use this dictionary ID.
  --help, -h, -?           Display this information.
  --list, -l               Display dictionary IDs.
  --version, -v            Display version information.
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
