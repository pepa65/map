# map
**Map lines from stdin to a commandline**

## Usage
```
map '<commandline>'
  For each line of stdin, variable i is set to the line number (starting at 0)
  and variable m is set to the content of the line, and each <commandline>
  (that can refer to $i and $m) gets executed. In map (unlike mapf),
  all lines of stdin get mapped regardless of any returncodes.

mapf '<commandline>'
  For each line of stdin, variable i is set to the line number (starting at 0)
  and variable m is set to the content of the line, and each <commandline>
  (that can refer to $i and $m) gets executed. In mapf (unlike map),
  the mapping stops on a non-zero returncode of a commandline.
```

## Description
Executes the given commandline for each line from stdin in sequence, while
assigning the line number to variable i and the line content to variable m.

Example:
```console
$ ls
LICENSE   README.md makefile  map.1   map.c
```

```console
$ ls |map 'printf "%d %s-" $i $m; rev <<<"$m"'
0 LICENSE-ESNECIL
1 README.md-dm.EMDAER
2 makefile-elifekam
3 map.1-1.pam
4 map.c-c.pam
```

To prevent variable substitution by the shell, the commandline best be wrapped
in single quotes.

## Installation
Install `map` and `mapf` into `/usr/local/bin` with: `make install`.

Use `make PREFIX=/some/other/directory install` to install elsewhere.
Use `make uninstall` to uninstall.

## Motivation
There are many ways to accomplish what you can do with `map`,
including `find`, `xargs`, `awk`, and shell for-loops. The approach
taken by `map` is extremely pragmatic and allows me to express
concisely what I want. Given the fact that it's designed as a filter,
it can operate on any kind of list, not only lists of files.

The problem that prompted me to think about `map` was the following:
given a list of files, I wanted to execute two commands on each.
Here's how you can do it with different tools:

With `map`:
```sh
ls *.c |map 'foo $m; bar $m'
```

With `xargs`:
```sh
ls *.c |xargs -I % sh -c 'foo %; bar %;'
```

With `awk`:
```sh
ls *.c |awk '{ system("foo "$0"; bar "$0) }'
```

With `find`:
```sh
find . -name \*.c -maxdepth 1 -exec foo {} \; -exec bar {} \;
```

With a `bash` for-loop:
```bash
for f in *.c
do
  foo $f
  bar $f
done
```

With a `csh` for-loop:
```sh
foreach f (*.c)
  foo $f
  bar $f
end
```

Map's modest claim is that it improves on the ergonomics of existing
tools. It's not only that sometimes it allows you to type less, but
also the conceptual model needed to operate it is simpler.

Let's consider these tasks:

1. Execute a command `foo` on each C file:
  - `ls *.c |map 'foo $m'`
  - `ls *.c |xargs foo`
  - `find . -name *.c -maxdepth 1 -exec foo {} \;`
2. Execute commands `foo` and `bar` on each C file:
  - `ls *.c |map 'foo $m; bar $m'`
  - `ls *.c |xargs -I % sh -c 'foo %; bar %;'`
  - `find . -name *.c -maxdepth 1 -exec foo {} \; -exec bar {} \;`
3. Download files from a list of URLs in a file:
  -  `cat urls |map 'curl -O $m'`
  - `cat urls |xargs -n 1 curl -O`
4. Sleep three times for one second and echo "done" after each elapsed second:
  - `printf "1\n1\n1\n" |map 'sleep $m && echo done'`
  - `printf "1\n1\n1\n" |xargs -n 1 -I % sh -c 'sleep % && echo done'`
5. Same as number 4, but run the commands in parallel:
  - `printf "1\n1\n1\n" |map 'sleep $m && echo done &'`
  - `printf "1\n1\n1\n" |xargs -n 1 -P 3 -I % sh -c 'sleep % && echo done'`

Examples 3-5 cannot be done with `find` as it only operates on file systems.

When using `map`, the commands use the shell's familiar syntax, so there is
less cognitive load in needing to remember the syntax of `xargs` or `find`.

As with anything in life, familiarity helps and if you use a tool
in a certain way over and over it will seem simple to operate, but
we can still analyze the conceptual models and determine how much
information is needed in each case. I would say the advantage of
`map` is that it requires less knowledge.

Of course `xargs` and `find` are much larger tools, with a bigger
feature set. Map is tiny. For comparison, here's the
[source code of GNU xargs][xargs]. No doubt `xargs` will offer
a lot more features, but so far with `map` I've completely stopped
using `xargs` and for-loops. Another way to think about `map` vs
`xargs`: if `map` had been specified in POSIX and `xargs` was just
released, I'm not sure I would install it unless `map` proved to
be unfit for a given use case.

[xargs]: https://fossies.org/dox/findutils-4.7.0/xargs_8c_source.html

Something important to keep in mind is that `map` works on lines,
not on files. My reasoning was that in the context of a POSIX
environment, a map function can be expected to filter lines from
stdin. In that regard, it is very generic because a line can represent
anything.

## Known issues
In the examples above I frequently use the output of `ls`. In my
projects, filenames don't contain spaces, newlines, or other special
characters because I find them inconvenient, even though they are
valid. As `map` works on lines, filenames that contain newlines
should be handled separately. For filenames that contain whitespace,
the common solution is to wrap the variable in double quotes, so
instead of `$m` you would use `"$m"`.

## Contributing
If you find a bug, please create an issue detailing the ways to
reproduce it. If you have a suggestion, create an issue detailing
the use case: https://github.com/pepa65/map
