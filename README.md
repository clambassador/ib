# ib

The ib project is a collection of simple routines for simplifying routine tasks
in C++ programming. It was designed by iteratively using it and improving the
interface for programmer's usability. They are meant to be natural to use and
general purpose.

At the moment it consists of the following:

- marshalled: a templated data packer / unpacker that includes a templated
call-into for complicated classes.
- logger: a templated logger with printf-like semantics which includes runtime
and colours.
- sensible_time: gives program runtime in seconds or microseconds.
- config: a parser to create a configuration list.
- limiter: a class that slows down a repeated operation if, e.g., it fails.

## Config

Config is a singleton class that loads a configuration file. It allows the
programmer to get strings, uint64_t, and lists of uint64_t anywhere by calling
Config::_()->get("name")

The configuration file is a sequence of entries of the form:

var 32
prefix_suffix 53
prefix_5_suffix 12

The get() function takes the full name, or a list of pieces, like: get("prefix",
5, "suffix");

Config files can #include otherfile.cfg

They also allow
   prefix column_1 {
	   row_1 10
	   row_2 20
  } 
in lieu of column_1_row_1 10, etc.

Strings are done by putting
   string name Name with spaces
and it ends at the new line. Each line is its own config entry.

While a singleton, configs can also be created for special purposes. Values can
be set into them and they can be saved. The static _() function simply returns a
globally accessible config.



## Limiter

The limiter returns false if you can keep trying, and true if you should give
up. Just call the class like a function. It also slows you down as you go.
The constructor takes a list of up to 6 numbers: how many times to wait at each
'level' before going to the next one. The levels are 1, 10, 100, 1000, 1e4, 1e5
milliseconds. The sum of the numbers is how many times it will return false
before true; the dot product with the levels is how long it will sleep before
returning true.
