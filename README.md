# Armenian Unicode Converter

A small wxWidgets desktop utility that converts pasted text between legacy Times Armenian character mappings and Armenian Unicode. Enter text on either side to update the other.

## What it converts

The left panel contains legacy character positions represented as UTF-8 text; the right contains Armenian Unicode. For example, legacy `²´¶` becomes `ԱԲԳ`. The mapping includes Armenian letters and selected punctuation, including the ellipsis.

This is not a general encoding detector or raw ANSI-file converter. It does not parse Word documents or repair every kind of mojibake. Both conversion functions accept UTF-8 byte strings. Unmapped sequences are preserved, but characters also used by the legacy mapping are interpreted as legacy symbols when converting toward Armenian. Keep a copy of your original text; punctuation mappings are not a guarantee of lossless round trips for arbitrary text.

To display the legacy side with Armenian glyphs, a compatible legacy font must be installed separately. Fonts are not bundled.

## Build and run

Requires CMake 3.16+, a C++17 compiler and wxWidgets development libraries (core and base).

```sh
git clone https://github.com/senqx/ArmUnicodeConverter.git
cd ArmUnicodeConverter
cmake -S . -B build
cmake --build build
./build/arm_unicode_converter
```

On Debian/Ubuntu, the wxWidgets development package is commonly `libwxgtk3.2-dev`; on Arch Linux, `wxwidgets-gtk3`. Package names depend on your distribution.

## Validate the conversion library

The conversion library has no wxWidgets dependency:

```sh
cmake -S . -B build-tests -DARM_BUILD_GUI=OFF
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure
```

Tests cover every mapping entry, ASCII, unrelated Unicode, and truncated input sequences. The conversion library is verified on Linux; the GUI requires a separate wxWidgets build and manual interaction check.

## License

[MIT](LICENSE).
