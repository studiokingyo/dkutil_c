# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**dkutil_c** (dKingyo Utility Toolkit for C) is a comprehensive C utility library providing cryptography, compression, data structures, file system operations, and algorithms. It's a legacy Windows-focused project (BSD License, v0.210, circa 2006).

## Build Commands

### Visual C++ 6.0
Open `dkutil_c.dsw` workspace and build. Configurations: Debug, Release, DebugMT, ReleaseMT.

### NMAKE (Command Line)
```cmd
nmake /f dkutil_c.mak CFG="dkutil_c - Win32 ReleaseMT"
```
Valid configurations: `Release`, `Debug`, `DebugMT`, `ReleaseMT`

### Dev-C++/MinGW
```cmd
make -f Makefile.win
```
Output: `devcpp/dkutil_c.a`

### Documentation
```cmd
doxy_release.bat
```
Generates Doxygen docs in `html/`, `latex/`, `xml/` directories.

### Cleanup
```cmd
clean.bat
```

## Architecture

### Main Entry Point
`dkc.h` - Master header that includes all library components.

### Module Organization

| Category | Components |
|----------|------------|
| **Data Structures** | dkc2Tree (binary), dkcBTree (B-Tree), dkcRedBlackTree, dkcAVLTree (AVL Tree), dkcHashSet, dkcHashMap, dkcHashMultiSet, dkcHashMultiMap, dkcMPH (Minimal Perfect Hash - CHD), dkcBloomFilter, dkcCuckooHash (Cuckoo Hash Table/Filter), dkcLRUCache, dkcLRUCache2 (library reuse), dkcSegmentTree (Segment Tree), dkcFenwickTree (BIT), dkcUnionFind (DSU), dkcSparseSet, dkcSingleList, dkcStack, dkcQueue, dkcDeque, dkcDynamicArray, dkcHash, dkcLightHash |
| **Streams** | dkcStream, dkcCircularStream, dkcMemoryStream |
| **Cryptography** | Rijndael, Blowfish, ARC4, Camellia, Twofish, Threefish, HC256, SNOW2.0, KCipher-2, Vernam |
| **Message Digests** | MD2, MD4, MD5, SHA1, SHA256, SHA384, SHA512, Keccak, SHA3-224/256/384/512, BLAKE-224/256/384/512, BLAKE2s, BLAKE2b, BLAKE3, HMAC, CRC |
| **Compression** | dkcLZSS, dkcLZW, dkcRLE, dkcBlockSort |
| **File Systems** | dkcFileSystems, dkcSJISFS (Shift-JIS support), dkcSafeFS |
| **Threading** | dkcThread, dkcThreadLock (pthread/Win32) |
| **Utilities** | dkcDLL, dkcRegex, dkcMath, dkcMemoryPool, dkcUniqueID (UUID/ULID/Snowflake/KSUID/XID/NanoID/CUID), dkcOS |

### Standard Library Abstraction
`dkutil_cstd/` contains platform-independent wrappers for memory, stdlib, and x86 optimizations.

### Higher-Level Utilities
`dKingyoUtility3/` provides additional utility layer built on top of the core library.

## Naming Conventions

- `dkc` - Main library functions
- `dkcm` - Macros (function-like)
- `dkcd` - Constants/defines
- `DKC_` - Typedefs and structures
- `edu` / `enu` - Enumerations
- `_func_type` suffix - Function pointers

## Platform Notes

- **Target**: Windows/x86 (x86 assembly optimizations, WIN32 API)
- **Calling Convention**: WINAPI for exported functions
- **Endianness**: Little-endian default (ORDER_DCBA macro)
- **Threading**: pthread for POSIX, Windows threads for Win32
- **Memory**: Custom allocators via DKC_ALLOCATOR interface

## Key Implementation Details

- Self-contained with no external dependencies
- Supports 64-bit data types (ULONGLONG, LONGLONG)
- Stream-based I/O abstraction for flexible data handling
- Bit-level manipulation utilities for efficient storage
- DCF (dKingyo Cryption File) format for encrypted file handling

## Testing

### Test Suite Location
`test/test_main.c` - Comprehensive test suite (VC6 compatible, C89)

### Building Tests
Open `test/test_dkutil_c.dsw` in VC6, build dkutil_c first, then build test_dkutil_c.

### Test Coverage

| Category | Modules Tested |
|----------|---------------|
| **Containers** | Stack, Queue, Deque, HashSet, HashMap, HashMultiSet, HashMultiMap, SingleList, MemoryStream, CircularMemoryStream, Buffer, MemoryPool |
| **Trees** | BTree, 2Tree (Binary Tree), RedBlackTree, AVLTree, NaryTree, SegmentTree, FenwickTree (BIT), UnionFind (DSU), SparseSet |
| **Hash/Digest** | CRC32, CRC-CCITT, MD2, MD4, MD5, SHA1, SHA256, SHA384, SHA512, SHA3-224, SHA3-256, SHA3-384, SHA3-512, BLAKE-224, BLAKE-256, BLAKE-384, BLAKE-512, BLAKE2s, BLAKE2b, BLAKE3, HMAC-MD5, HMAC-SHA1 |
| **Ciphers** | Arcfour (RC4), Blowfish, Rijndael (AES), Camellia, Twofish, Threefish, SNOW 2.0, HC256, KCipher-2, Vernam |
| **Compression** | LZSS, LZW, RLE (PackBits) |
| **Regex** | Regular Expression matching and search |
| **Utilities** | Memory allocation, Sort (Shell/Comb/Bubble), Bit operations, XorShift RNG, SJIS String, UniqueID (UUID v1-v8, ULID, Snowflake, KSUID, XID, NanoID, CUID), MPH (Minimal Perfect Hash), LightHash (DJB2, FNV, MurmurHash, xxHash, CityHash, wyhash, FxHash等), BloomFilter, CuckooHash/CuckooFilter, LRUCache, LRUCache2 |

### Notes on Untested Modules
- All major modules are covered by the test suite.
