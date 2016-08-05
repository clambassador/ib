# ib

The ib project is a collection of simple routines for simplifying routine tasks
in C++ programming. It was designed by iteratively using it and improving the
interface for programmer's usability.

Ultimately it will consist of many such tools. At the moment it consists of the
following:

- marshalled: a templated data packer / unpacker that includes a templated
call-into for complicated classes.
- logger: a templated logger with printf-like semantics which includes runtime
and colours.
- sensible_time: gives program runtime in seconds or microseconds.
