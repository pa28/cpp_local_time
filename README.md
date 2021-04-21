# cpp_local_time
CPP Library that support display of local time for arbitrary time zones.

While some Linux/Unix systems now have access to a C Standard Library 
function called `localtime_rz()` that operates like `localtime()` but,
among other things, takes a time zone as one of its arguments. This allows
a programmer to easily display times in timezones that are not GMT or the
system local time zone. Unfortunately not all systems have this function
available yet.

Fortunately the underlying source code, found at [INNA](https://www.iana.org/time-zones),
has this function available by changing compilation options. This project
uses that code to build a small library to provide `localtime_rz()` and
a C++ class to wrap it in.
