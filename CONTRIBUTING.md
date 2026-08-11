# Contributing to ModbusTools

Thank you for contributing to ModbusTools.
This guide explains how to set up the project, make changes, and submit them for review.

## Project At a Glance

- Main repository builds two GUI apps: `mbclient` and `mbserver`
- Shared core library: `mbcore` in `src/core`
- Protocol library is a Git submodule: `modbus` (ModbusLib)
- Primary build system: CMake
- Legacy build/test files with qmake still exist

## Requirements

### General

- Git
- CMake >= 3.13
- C++ compiler
  - Windows: MSVC
  - Linux: GCC or Clang
- Qt 5.8+ (Qt 5.15.x recommended)

### For Server Scripting Features

- Python 3.6+
- PyQt5

## Clone and Prepare

```bash
git clone https://github.com/serhmarch/ModbusTools.git
cd ModbusTools
git submodule update --init --recursive
```

The `modbus` directory is a submodule. Always initialize/update it before building.

## Build (CMake)

Top-level CMake presets are defined in `CMakePresets.json`.

### Windows (PowerShell)

```powershell
cmake --preset Win64-MSVC
cmake --build --preset Win64-MSVC-Debug
cmake --build --preset Win64-MSVC-Release
```

Optional targets:

```powershell
cmake --build --preset Win64-MSVC-Debug --target mbclient
cmake --build --preset Win64-MSVC-Debug --target mbserver
```

Quick script from repo root:

```powershell
./build.bat
```

### Linux

```bash
cmake --preset Linux-Debug
cmake --build --preset Linux-Debug

cmake --preset Linux-Release
cmake --build --preset Linux-Release
```

Quick script from repo root:

```bash
./build.sh
```

## Build Options You May Need

Top-level options (see root `CMakeLists.txt`):

- `MBTOOLS_CLIENT_ENABLED=ON|OFF`
- `MBTOOLS_SERVER_ENABLED=ON|OFF`
- `MBTOOLS_PACKAGE=ON|OFF`

Example:

```bash
cmake --preset Win64-MSVC-Client
cmake --build --preset Win64-MSVC-Client-Release
```

## Testing

## 1) ModbusLib unit tests (CMake/CTest)

Unit tests are maintained in the `modbus` submodule and are enabled by `MB_TESTS_ENABLED`.

```bash
cmake -S modbus -B build/modbus-tests -DMB_TESTS_ENABLED=ON -DMB_EXAMPLES_ENABLED=OFF
cmake --build build/modbus-tests
ctest --test-dir build/modbus-tests --output-on-failure
```

Notes:

- If Qt-specific tests are needed, configure ModbusLib with `-DMB_QT_ENABLED=ON`
- On Windows, ensure Qt runtime DLLs are discoverable when running tests

## 2) Legacy Qt test project files

There is also a legacy qmake test layout under `tests/` (for example `tests/test_core/test_core.pro`).
Use this only if your change targets that workflow.

## Documentation

Documentation is generated via `doc/CMakeLists.txt` when Doxygen is found.

- Doxygen is auto-detected
- Graphviz `dot` is optional (enables diagrams)
- Qt help generator is optional (produces qch/qhc)

Build docs by running a regular CMake build. Relevant targets are created automatically when tools are found.

## Code Structure

- `src/core` - shared application core (`mbcore`)
- `src/client` - client application (`mbclient`)
- `src/server` - server application (`mbserver`)
- `modbus/src` - protocol library implementation (submodule)
- `doc` - doxygen config/templates and help project files
- `package` - CPack and installer integration

## Coding Guidelines

No strict formatter config is committed (no repo-wide `.clang-format`/`.editorconfig`), so consistency with nearby code is required.

### C++ codestyle used in this repository

The patterns below are based on current files in `src/core`, `src/client`, `src/server`, and `modbus/src`.

#### 1) Naming and class layout

- Application/core classes use `mb` prefix and PascalCase, for example `mbCoreProject`, `mbServerProject`.
- Use `mbCore...` prefix `mbcore`-lib classes.
- Use `mbClient...` prefix for `mbclient`-app classes.
- Use `mbServer...` prefix for `mbserver`-app classes.
- Modbus library classes use `Modbus...` naming, for example `ModbusClientPort`.
- Member fields use `m_` prefix, for example `m_project`, `m_ctrlRun`.
- Qt signal/slot helper methods commonly use `slot...` names.
- Public API in headers is often grouped with labeled sections such as `public: // settings`, `public: // devices`.

#### 2) Qt idioms and macros

- Prefer Qt macros/idioms already used in the project:
  - `Q_OBJECT`, `Q_SIGNALS`, `Q_EMIT`
  - `QStringLiteral(...)` for compile-time string literals
  - existing containers/types such as `QList`, `QHash`, `QVariant`
- Existing code still uses `Q_FOREACH` in many places. Do not mass-convert loops in unrelated files.

#### 3) Formatting conventions

- Use 4 spaces indentation, no tabs.
- Keep opening brace on the new line for functions, conditionals, and loops:
  
```cpp
void func1(int a, int b)
{
    if (a < b)
    {
        doSomething();
        for (int i = 0; i < MAX; i++)
        {
            // for loop
        }
    }
    else
    {
        doSomethingElse();
        while (condition)
        {
            // while loop
        }
    }
}
```

- Use readable multiline conditions, for example:

```cpp
if ((index >= 0) && (index < m_ports.count()))
    m_ports.insert(index, port);
```

- Keep early returns for guard clauses.
- Use pointer/reference symbol at variable name, like `Type *v` or `Type &v`,
  because in C/C++ pointer/reference is related to a variable, not a type.

#### 4) File/header conventions

- New source/header files should include the project license header block used in existing files.
- Keep include order stable and minimal:
  - own header first in `.cpp`
  - then Qt/standard/project headers in the file's existing style
- Do not reorder includes broadly unless required for correctness.

#### 5) API and behavior changes

- Avoid broad refactoring in feature/bugfix PRs.
- Keep behavior-compatible changes unless the PR explicitly targets a breaking change.
- For shared abstractions (`src/core` and `modbus/src`), check impact on both client and server paths.

#### 6) Comments and documentation

- Prefer comments that explain intent and constraints, not obvious mechanics.
- Use Doxygen-style comments for public library APIs where surrounding code already uses Doxygen style (especially in `modbus/src`).

#### 7) Practical rule for contributors

- When in doubt, copy the style from the nearest existing file in the same module.
- Keep diffs focused: functional change first, formatting-only changes only when necessary.

## Commit Guidelines

- Make small, logical commits
- Use imperative commit titles
- Reference issues when applicable

Examples:

- `Fix scanner unit range validation`
- `Add timeout guard for TCP read loop`
- `Update server scripting docs for Python 3.6+`

## Pull Request Guidelines

When opening a PR, include:

- What changed
- Why it changed
- How it was tested
- Platform(s) tested (Windows/Linux)
- Any compatibility or migration notes

Before requesting review:

- Build succeeds for the affected target(s)
- Relevant tests pass
- Documentation is updated when behavior/UI changes
- Submodule changes are intentional and clearly described

## Submodule Changes (Important)

If your PR updates code in `modbus`:

1. Commit changes in the ModbusLib repository first (or use your fork)
2. Update this repository to the new submodule commit
3. In the PR description, explain exactly why the submodule pointer changed

Avoid accidental submodule pointer updates.

## Packaging and Install Notes

- Packaging is controlled by `MBTOOLS_PACKAGE`
- Install layout variables are in `cmake/InstallDirs.cmake`
- On Windows, CMake install logic includes Qt runtime DLLs/plugins

If your change affects runtime dependencies, verify `cmake --install` output before merging.

## Reporting Bugs and Requesting Features

Use GitHub issues with:

- Reproduction steps
- Expected behavior vs actual behavior
- OS, compiler, Qt version
- Logs/screenshots when useful

Also check project planning notes in `TODO` to avoid duplicates.

## License

By contributing, you agree your contributions are licensed under GPL-3.0 (see `LICENSE`).
