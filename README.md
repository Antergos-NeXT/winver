# winver — Antergos NeXT system information dialog

KDE-styled version of Windows `winver`, built with **Qt6** and **KF6**.

Shows OS version, kernel, architecture, CPU, and memory info with Antergos NeXT branding.

## Build

```bash
cmake -B build -DCMAKE_PREFIX_PATH="/usr/lib/cmake"
cmake --build build
./build/winver
```

## Dependencies

- `qt6-base`
- `qt6-svg`
- `kf6-ki18n`
- `kf6-kcoreaddons`
- `cmake`
- `gcc` or `clang`

## Install

```bash
sudo cmake --install build
```
