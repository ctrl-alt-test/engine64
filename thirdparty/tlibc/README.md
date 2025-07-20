# Tiny C Runtime Library

The C run-time library used in the Ctrl-Alt-Test demo engine.

## Overview

In the October 1996 and January 2001 editions of the MSDN Magazine,
Matt Pietrek wrote two articles on how to write a minimal C run-time
(CRT) library, which he called LIBCTINY.

The articles are long gone but Internet Archive has copies of them:
- https://web.archive.org/web/20031213002926/http://www.microsoft.com/msj/archive/S569.aspx
- https://web.archive.org/web/20031220225033/http://msdn.microsoft.com/msdnmag/issues/01/01/hood/

In August 2006, a user by the handle Mike_V wrote an article on the
website [Code Project](http://www.codeproject.com) with a proposed
updated version of that CRT library:
- https://www.codeproject.com/Articles/15156/Tiny-C-Runtime-Library

## Notes

The file manipulation code (see file.cpp) is based on assumptions
regarding the underlying data structure behind the type FILE, which do
not hold true anymore for the toolset distributed with recent versions
of Visual Studio. As a consequence, the related code does not compile
anymore.

Since we do not use file operations when we have size constraints, we
have simply disabled this code with the macro ENABLE_FILE_IO.

## License

See `CPOL.htm`.
