# PE Bliss 2

## The most advanced and thorough library to read and write Portable executable files

**This is a work-in-progress active development project! Everything may change, and the library may contain critical bugs, do not use in production!**

The [Portable executable](https://en.wikipedia.org/wiki/Portable_Executable) is a popular executable file format, which is extensively used in Windows family operating systems. It is as well utilized by the X-Box, EFI and some other environments. The **PE Bliss 2** library is a successor of the old and unsupported [PE Bliss](https://code.google.com/archive/p/portable-executable-library/) library.

The library is aimed at being able to provide as much information about the PE file as possible, while being robust and keeping track of possible parsing or format errors and irregularities. Currently, the library is compiled using the MSVC compiler, but the code itself is cross-platform.

## Features
- Reading PE/PE+ binaries (unit-tested)
  - Reading/modifying binaries either from a file, or from a memory buffer
  - Reading/modifying binaries, which are already loaded into memory by the Windows loader
  - Supports copying the full binary sections memory or referencing the existing memory buffer (read-only mode)
  - Thread-safe when working with stateless buffers (i.e. an image does not reference a file) with the same image from different threads
  - Reading either physical only or physical and virtual data from PE files with bounds checking
- Rebuilding PE/PE+ binaries to a file or a memory buffer (unit-tested)
- DOS, File, Optional header parsing (unit-tested)
- Data directories parsing (unit-tested)
- DOS Stub, Rich header parsing (unit-tested)
  - Rich COMPID database (unit-tested)
- Section table parsing (unit-tested)
- PE checksum calculation for aligned executables (unit-tested)
- Shannon entropy calculation (unit-tested)
- Directory support:
  - Bound import:
    - Read (unit-tested)
    - Modify (untested)
  - Exception (read-only, unit-tested)
    - x64
    - ARM
    - ARM64
    - ARM64 + x64 combined
  - Exports:
    - Read (unit-tested)
    - Modify (untested)
  - Imports (or same style delay imports):
    - Read (unit-tested)
    - Modify (untested)
  - Load configuration (including recent versions up to Windows 10 22H1, read-only, unit-tested):
    - Lock prefix table
    - Global flags
    - Process heap flags
    - Dependent load flags
    - SafeSEH handler table
    - Guard flags
    - CF Guard (stride, function table, address taken IAT entry table, long jump target table)
    - CHPE (hybrid portable executable: ARM, ARM64X, ARM64EC)
      - Range entries, metadata
    - Dynamic relocation tables (as full support as possible, many of them are undocumented)
      - v1, v2 tables
      - IMPORT_CONTROL_TRANSFER_DYNAMIC_RELOCATION
      - INDIR_CONTROL_TRANSFER_DYNAMIC_RELOCATION
      - SWITCHTABLE_BRANCH_DYNAMIC_RELOCATION
      - ARM64X_DYNAMIC_RELOCATION (zero fill, copy data, add delta)
      - FUNCTION_OVERRIDE_DYNAMIC_RELOCATION
      - Prologue, epilogue dynamic relocations
    - Enclave descriptors
    - Volatile metadata (descriptors, access RVA tables, range tables)
    - Exception handling (EH) continuation targets
    - Extended flow guard
    - CastGuard
    - memcpy guard
  - Relocations:
    - Relocation types: ABSOLUTE, HIGHLOW, DIR64, HIGH, LOW, HIGHADJ, THUMB_MOV32
    - Read (unit-tested)
    - Modify (untested)
    - Image rebase (unit-tested)
  - TLS:
    - Read (unit-tested)
    - Modify (untested)
  - Resources:
    - Read (unit-tested)
    - Supported resource types:
      - Accelerators (read-only, unit-tested)
      - Bitmap (read-write, unit-tested)
      - String tables (read-write, unit-tested)
      - Manifests (read-only, unit-tested)
        - Assembly and Application
          - Assembly identity
          - noInherit/noInheritable
          - Supported OS list
          - Dependencies
          - Windows settings
          - MSIX identity
          - Requested execution level
          - Files, COM classes, typelibs, COM interface proxy stubs, window classes
      - Message tables (read-only, unit-tested)
      - Version info (read-only, unit-tested)
  - Debug directory support (read-only, unit-tested)
    - MISC
    - Repro (PE determinism or reproducibility)
    - SPGO (sample profile-guided optimization info)
    - CodeView
      - PDB2.0
      - PDB7.0
      - Generic support for other signatures
    - OMAP to SRC
    - SRC to OMAP
    - FPO (Frame pointer omission info)
    - POGO (profile-guided optimization info)
    - COFF (no line numbers and aux symbols support)
    - VC Feature
    - Intel MPX
    - EX DLL Characteristics (CET Shadow Stack flags)
    - ILTCG (Incremental link-time code generation flag)
    - PDB Hash
    - MPDB (Embedded Portable PDB)
- Address conversions (RVA/VA/file offsets, unit-tested)
- Overlay at the end of file parsing (unit-tested)
- Simple console dumper of PE/PE+ files (no extended configuration yet)

## Approximate roadmap
- High priority
  - Icons, cursors resource support
  - MUI resource support
  - Security directory support (authenticode)
    - Image signature verification/calculation
    - Special signers
  - Basic .NET directory support (read-only)
  - Import redirects (apisetschema.dll)
- Medium priority
  - Menu resource support
  - Dialogs resource support
  - CMake project and cross-platform compilation support
  - More features to console_dumper
  - .NET tables support (read-only)
  - .NET resources support (read-only)
  - Trustlets
  - Entropy calculation
- Low proirity
  - Unit tests for exports modification code
  - Unit tests for imports modification code
  - Unit tests for bound import modification code
  - Relocations modification code and unit tests
  - TLS modification code and unit tests
  - Support of adding new sections/modification of existing sections and unit tests
  - Usage examples
  - Full documentation
- Very low priority
  - Toolbar resource support
  - Ribbon resource support
  - COFF debug aux symbols support
  - COFF debug line numbers support
  - VB5/VB6 structures (project info, DLLCall imports, modules, object table)

## Library requirements
- C++20 or newer.
- [Boost](https://www.boost.org/) libraries (header-only): **endian**, **pfr**
- Two external libraries are embedded (`external` directory): **googletest**, **pugixml**
