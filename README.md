# Unicode identifier validator in C++11

Simple UTF8 Unicode identifier validator in C++11.

Based on UAX31.

## Usage

```C++
#include "unicode-xid.hh"

bool unicode_xid::is_xid_start(uint32_t cp);
bool unicode_xid::is_xid_continue(uint32_t cp);
```

To convert UTF-8 string to codepoint values, see `test_unicode_xid.cc`.

## Unicode version

5.1.0

## Generate table.

See `gen` folder.

## License

MIT license.

Table generation script is based on `unicode-rs`, which is licensed under Apache 2.0

## References

* unicode-rs : Rust implementation https://github.com/unicode-rs/unicode-xid
