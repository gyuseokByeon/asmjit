AsmJit
------

Machine code generation for C++.

  * [Official Home Page (asmjit.com)](https://asmjit.com)
  * [Official Repository (asmjit/asmjit)](https://github.com/asmjit/asmjit)
  * [Public Chat Channel](https://gitter.im/asmjit/asmjit)
  * [Zlib License](./LICENSE.md)

See [asmjit.com](https://asmjit.com) page for more details.

Documentation
-------------

  * [Documentation Index](https://asmjit.com/doc/index.html)

Introduction
------------

AsmJit is a complete JIT and AOT assembler for C++ language. It can generate native code for x86 and x64 architectures and supports the whole x86/x64 instruction set - from legacy MMX to the newest AVX512. It has a type-safe API that allows C++ compiler to do semantic checks at compile-time even before the assembled code is generated and/or executed.

AsmJit, as the name implies, started as a project that provided JIT code-generation and execution. However, AsmJit evolved and it now contains features that are far beyond the scope of a simple JIT compilation. To keep the library small and lightweight the functionality not strictly related to JIT is provided by a sister project called [asmtk](https://github.com/asmjit/asmtk).


Minimal Example
---------------

```c++
#include <asmjit/x86.h>
#include <stdio.h>

using namespace asmjit;

// Signature of the generated function.
typedef int (*Func)(void);

int main(int argc, char* argv[]) {
  JitRuntime rt;                          // Runtime specialized for JIT code execution.

  CodeHolder code;                        // Holds code and relocation information.
  code.init(rt.codeInfo());               // Initialize to the same arch as JIT runtime.

  x86::Assembler a(&code);                // Create and attach x86::Assembler to `code`.
  a.mov(x86::eax, 1);                     // Move one to 'eax' register.
  a.ret();                                // Return from function.
  // ----> x86::Assembler is no longer needed from here and can be destroyed <----

  Func fn;
  Error err = rt.add(&fn, &code);         // Add the generated code to the runtime.
  if (err) return 1;                      // Handle a possible error returned by AsmJit.
  // ----> CodeHolder is no longer needed from here and can be destroyed <----

  int result = fn();                      // Execute the generated code.
  printf("%d\n", result);                 // Print the resulting "1".

  // All classes use RAII, all resources will be released before `main()` returns,
  // the generated function can be, however, released explicitly if you intend to
  // reuse or keep the runtime alive, which you should in a production-ready code.
  rt.release(fn);

  return 0;
}
```


AsmJit Summary
--------------

  * Complete x86/x64 instruction set - MMX, SSE+, BMI+, ADX, TBM, XOP, AVX+, FMA+, and AVX512+.
  * Different emitters providing various abstraction levels (Assembler, Builder, Compiler).
  * Support for sections for separating code and data.
  * Built-in CPU vendor and features detection.
  * Advanced logging, formatting, and error handling.
  * JIT memory allocator - interface similar to malloc/free for JIT code-generation and execution.
  * Lightweight and easily embeddable - ~300kB compiled with all built-in features.
  * Modular design - unneeded features can be disabled at compile-time to make the library smaller.
  * Zero dependencies - no external libraries, no STL/RTTI - easy to embed and/or link statically.
  * Doesn't use exceptions internally, but allows to attach a "throwable" error handler of your choice.


Advanced Features
-----------------

  * AsmJit contains a highly compressed instruction database:
    * Instruction names - allows to convert instruction id to its name and vice versa.
    * Instruction metadata - access (read|write) of all operand combinations of all instructions.
    * Instruction signatures - allows to strictly validate if an instruction (with all its operands) is valid.
  * AsmJit allows to precisely control how instructions are encoded if there are multiple variations.
  * AsmJit is highly dynamic, constructing operands at runtime is a common practice.
  * Multiple emitters with the same interface - emit machine code directly or to a representation that can be post-processed.


Important
---------

Breaking the API is sometimes inevitable, what to do?
  * See asmjit tests, they always compile and provide an implementation of many use-cases:
    * [asmjit_test_x86_asm.cpp](./test/asmjit_test_x86_asm.cpp) - Tests that demonstrate the purpose of emitters.
    * [asmjit_test_x86_cc.cpp](./test/asmjit_test_x86_cc.cpp) - A lot of tests targeting Compiler infrastructure.
    * [asmjit_test_x86_sections.cpp](./test/asmjit_test_x86_sections.cpp) - Multiple sections test.
  * Visit our [Official Chat](https://gitter.im/asmjit/asmjit) if you need a quick help.


TODO
----

  * [ ] Add support for user external buffers in CodeHolder.


Supported Environments
----------------------

### C++ Compilers:

  * Requirements:
    * AsmJit won't build without C++11 enabled. If you use older GCC or Clang you would have to enable at least c++11 through compiler flags.
  * Tested:
    * **Clang** - tested by Travis-CI - Clang 3.9+ (with C++11 enabled) is officially supported (older Clang versions having C++11 support are probably fine, but are not regularly tested).
    * **GNU** - tested by Travis-CI - GCC 4.8+ (with C++11 enabled) is officially supported.
    * **MINGW** - tested by Travis-CI - Use the latest version, if possible.
    * **MSVC** - tested by Travis-CI - VS2017+ is officially supported, VS2015 is reported to work.
  * Untested:
    * **Intel** - no maintainers and no CI environment to regularly test this compiler.
    * Other c++ compilers would require basic support in [core/build.h](./src/asmjit/core/build.h).

### Operating Systems:

  * Tested:
    * **Linux** - tested by Travis-CI - any distribution is generally supported.
    * **OSX** - tested by Travis-CI - any version is supported.
    * **Windows** - tested by Travis-CI - Windows 7+ is officially supported.
  * Untested:
    * **BSDs** - no maintainers, no CI environment to regularly test these OSes.
    * **Haiku** - not regularly tested, but reported to work.
    * Other operating systems would require some testing and support in [core/build.h](./src/asmjit/core/build.h), [core/osutils.cpp](./src/asmjit/core/osutils.cpp), and [core/virtmem.cpp](./src/asmjit/core/virtmem.cpp).

### Backends:

  * **X86** - tested by both Travis-CI - both 32-bit and 64-bit backends are fully functional.
  * **ARM** - work-in-progress (not public at the moment).


Project Organization
--------------------

  * **`/`**        - Project root.
    * **src**      - Source code.
      * **asmjit** - Source code and headers (always point include path in here).
        * **core** - Core API, backend independent except relocations.
        * **arm**  - ARM specific API, used only by ARM and AArch64 backends.
        * **x86**  - X86 specific API, used only by X86 and X64 backends.
    * **test**     - Unit and integration tests (don't embed in your project).
    * **tools**    - Tools used for configuring, documenting and generating data files.


Configuring & Feature Selection
-------------------------------

AsmJit is designed to be easy embeddable in any project. However, it depends on some compile-time macros that can be used to build a specific version of AsmJit that includes or excludes certain features. A typical way of building AsmJit is to use [cmake](https://www.cmake.org), but it's also possible to just include AsmJit source code in your project and just build it. The easiest way to include AsmJit in your project is to just include **src** directory in your project and to define `ASMJIT_STATIC`. AsmJit can be just updated from time to time without any changes to this integration process. Do not embed AsmJit's [/test](./test) files in such case as these are used for testing.

### Build Type:

  * `ASMJIT_BUILD_DEBUG` - Define to always turn debugging on (regardless of compile-time options detected).
  * `ASMJIT_BUILD_RELEASE` - Define to always turn debugging off (regardless of compile-time options detected).

By default none of these is defined, AsmJit detects build-type based on compile-time macros and supports most IDE and compiler settings out of box. By default AsmJit switches to release mode when `NDEBUG` is defined.

### Build Mode:

  * `ASMJIT_STATIC` - Define to build AsmJit statically - either as a static library or as a part of another project. No symbols are exported in such case.

By default AsmJit build is configured to be built as a shared library, this means `ASMJIT_STATIC` must be explicitly enabled if you want to compile AsmJit statically.

### Build Backends:

  * `ASMJIT_BUILD_ARM` - Build ARM backends (not ready, work-in-progress).
  * `ASMJIT_BUILD_X86` - Build X86 backends (X86 and X86_64).
  * `ASMJIT_BUILD_HOST` - Build only the host backend (default).

If none of `ASMJIT_BUILD_...` is defined AsmJit bails to `ASMJIT_BUILD_HOST`, which will detect the target architecture at compile-time. Each backend automatically supports 32-bit and 64-bit targets, so for example AsmJit with X86 support can generate both 32-bit and 64-bit code.

### Disabling Features:

  * `ASMJIT_NO_BUILDER` - Disables both `Builder` and `Compiler` emitters (only `Assembler` will be available). Ideal for users that don't use `Builder` concept and want to have AsmJit a bit smaller.
  * `ASMJIT_NO_COMPILER` - Disables `Compiler` emitter. For users that use `Builder`, but not `Compiler`.
  * `ASMJIT_NO_JIT` - Disables JIT execution engine, which includes `JitUtils`, `JitAllocator`, and `JitRuntime`.
  * `ASMJIT_NO_LOGGING` - Disables logging (`Logger` and all classes that inherit it) and instruction formatting.
  * `ASMJIT_NO_TEXT` - Disables everything that uses text-representation and that causes certain strings to be stored in the resulting binary. For example when this flag is set all instruction and error names (and related APIs) will not be available. This flag has to be disabled together with `ASMJIT_NO_LOGGING`. This option is suitable for deployment builds or builds that don't want to reveal the use of AsmJit.
  * `ASMJIT_NO_INST_API` - Disables instruction query features, strict validation, read/write information, and all additional data and APIs that can output information about instructions.

NOTE: Please don't disable any features if you plan to build AsmJit as a shared library that will be used by multiple projects that you don't control (for example asmjit in a Linux distribution). The possibility to disable certain features exists mainly for customized builds of AsmJit.


Using AsmJit
------------

AsmJit library uses one global namespace called `asmjit` that provides the whole functionality. Core functionality is within `asmjit` namespace and arthitecture specific functionality is always in its own namespace like `x86`. For example API targeting both X86 and X64 architectures is found in `asmjit::x86` namespace.

### CodeHolder & Emitters

AsmJit provides two classes that are used together for code generation:

  * `CodeHolder` - Provides functionality to hold generated code and stores all necessary information about code sections, labels, symbols, and possible relocations.
  * `BaseEmitter` - Provides functionality to emit code into `CodeHolder`. `BaseEmitter` is abstract and provides just basic building blocks that are then implemented by `BaseAssembler`, `BaseBuilder`, `BaseCompiler`, and their architecture-specific implementations like `x86::Assembler`, `x86::Builder`, and `x86::Compiler`.

Code emitters:

  * `[Base]Assembler` - Emitter designed to emit machine code directly into a `CodeBuffer` held by `CodeHolder`.
  * `[Base]Builder` - Emitter designed to emit code into a representation that can be processed afterwards. It stores the whole code in a double linked list consisting of nodes (`BaseNode` and all derived classes). There are nodes that represent instructions (`InstNode`), labels (`LabelNode`), and other building blocks (`AlignNode`, `DataNode`, ...). Some nodes are used as markers (`SentinelNode` and comments (`CommentNode`).
  * `[Base]Compiler` - High-level code emitter that uses virtual registers and contains high-level function building features. Compiler extends `[Base]Builder` functionality and introduces new nodes like `FuncNode`, `FuncRetNode`, and `FuncCallNode`. Compiler is the simplest way to start with AsmJit as it abstracts lots of details required to generate a function that can be called from a C/C++ language.

### Instructions & Operands

Instructions specify operations performed by the CPU, and operands specify the operation's input(s) and output(s). Each AsmJit's instruction has it's own unique id (`Inst::Id` for example) and platform specific code emitters always provide a type safe intrinsic (or multiple overloads) to emit such instruction. There are two ways of emitting an instruction:

  * Using `BaseEmitter::inst(operands...)` - A type-safe way provided by platform specific emitters - for example `x86::Assembler` provides `x86::Assembler::mov(x86::Gp, x86::Gp)`.
  * Using `BaseEmitter::emit(instId, operands...)` - Allows to emit an instruction in a dynamic way - you just need to know instruction's id and provide its operands.


Other Topics
------------

This section provides quick answers to some recurring questions and topics.

### Instruction Validation

AsmJit by default prefers performance when it comes to instruction encoding. The Assembler implementation would only validate operands that must be validated to select a proper encoding of the instruction. This means that by default it would accept instructions that do not really exist like `mov rax, ebx`. This is great in release mode as it makes the assembler faster, however, it's not that great for development as it allows to silently pass even when the instruction's operands are incorrect. To fix this Asmjit contains a feature called **Strict Validation**, which allows to validate each instruction before the Assembler tries to encode it. This feature can also be used without an Assembler instance through `BaseInst::validate()` API.

Emitter options are configured through CodeHolder:

```c++
CodeHolder code;

// Enables strict instruction validation for all emitters attached to `code`.
code.addEmitterOptions(BaseEmitter::kOptionStrictValidation);

// Use either ErrorHandler attached to CodeHolder or Error code returned by
// the Assembler.
x86::Assembler a(&code);
Error err = a.emit(x86::Inst::kIdMov, x86::eax, x86::al);
if (err) { /* failed */ }
```

### Label Offsets and Links

When you use a label that is not yet bound the Assembler would create a `LabelLink`, which is then added to CodeHolder's `LabelEntry`. These links are also created for labels that are bound but reference some location in a different section. Firstly, here are some functions that can be used to check some basics:

```c++
CodeHolder code = ...;
Label L = ...;

// Returns whether the Label `L` is bound.
bool bound = code.isLabelBound(L);

// Returns true if the code contains either referenced, but unbound labels,
// or cross-section label links that are not resolved yet.
bool value = code.hasUnresolvedLinks();     // Boolean answer.
size_t count = code.unresolvedLinkCount();  // Count of links.
```

Please note that there is not API to return a count of unbound labels as this is completely unimportant from CodeHolder's perspective. If a label is not used then it doesn't matter whether it's bound or not, only used labels matter. After a Label is bound you can query it's offset relative to the start of the section where it was bound:

```c++
CodeHolder code = ...;
Label L = ...;

// After you are done you can check the offset. The offset provided
// is relative to the start of the section, see below for alternative.
// If the given label is not bound then the offset returned will be zero.
uint64_t offset = code.labelOffset(L or L.id());

// If you use multiple sections and want the offset relative to the base.
// NOTE: This function expects that the section has already an offset and
// the label-link was resolved (if this is not true you will still get an
// offset relative to the start of the section).
uint64_t offset = code.labelOffsetFromBase(L or L.id());
```

### Sections

Sections is a relatively new feature that allows to create multiple sections. It's supported by Assembler, Builder, and Compiler. Please note that using multiple sections is advanced and requires more understanding about how AsmJit works. There is a test-case [asmjit_test_x86_sections.cpp](./test/asmjit_test_x86_sections.cpp) that shows how sections can be used.

```c++
CodeHolder code = ...;

// Text section is always provided as the first section.
Section* text = code.textSection(); // or code.sectionById(0);

// To create another section use `code.newSection()`.
Section* data;
Error err = code.newSection(&data,
  ".data",  // Section name
  SIZE_MAX, // Name length if the name is not null terminated (or SIZE_MAX).
  0,        // Section flags, see Section::Flags.
  8);       // Section alignment, must be power of 2.

// When you switch sections in Assembler, Builder, or Compiler the cursor
// will always move to the end of that section. When you create an Assembler
// the cursor would be placed at the end of the first (.text) section, which
// is initially empty.
x86::Assembler a(&code);
Label L_Data = a.newLabel();

a.mov(x86::eax, x86::ebx); // Emits in .text section.

a.section(data);           // Switches to the end of .data section.
a.bind(L_Data);            // Binds label in this .data section
a.db(0x01);                // Emits byte in .data section.

a.section(text);           // Switches to the end of .text section.
a.add(x86::ebx, x86::eax); // Emits in .text section.

// References a label bound in .data section in .text section. This
// would create a LabelLink even when the L_Data is already bound,
// because the reference crosses sections. See below...
a.lea(x86::rsi, x86::ptr(L_Data));
```

The last line in the example above shows that a LabelLink would be created even for bound labels that cross sections. In this case a referenced label was bound in another section, which means that the link couldn't be resolved at that moment. If your code uses sections, but you wish AsmJit to flatten these sections (you don't plan to flatten them manually) then there is an API for that.

```c++
// ... (continuing the previous example) ...
CodeHolder code = ...;

// Suppose we have some code that contains multiple sections and
// we would like to flatten them by using AsmJit's built-in API:
Error err = code.flatten();
if (err) { /* Error handling is necessary. */ }

// After flattening all sections would contain assigned offsets
// relative to base. Offsets are 64-bit unsigned integers so we
// cast them to `size_t` for simplicity. On 32-bit targets it's
// guaranteed that the offset cannot be greater than `2^32 - 1`.
printf("Data section offset %zu", size_t(data->offset()));

// The flattening doesn't resolve unresolved label links, this
// has to be done manually as flattening can be done separately.
err = code.resolveUnresolvedLinks();
if (err) { /* Error handling is necessary. */ }

if (code.hasUnresolvedLinks()) {
  // This would mean either unbound label or some other issue.
  printf("FAILED: UnresoledLinkCount=%zu\n", code.unresovedLinkCount());
}
```


Support
-------

AsmJit is an open-source library released under a permissive ZLIB license, which makes it possible to use it freely in any open-source or commercial product. Free support is available through issues and gitter channel, which is very active. Commercial support is currently individual and can be negotiated on demand. It includes consultation, priority bug fixing, review of code that uses AsmJit, porting code to the latest AsmJit, and implementation of new AsmJit features.

If you use AsmJit in a non-commercial project and would like to appreciate the library in the form of a donation you are welcome to support us. Donations are anonymous unless the donor lets us know otherwise. The order and format of listed donors is not guaranteed and may change in the future. Additionally, donations should be considered as an appreciation of past work and not used to gain special privileges in terms of future development. AsmJit authors reserve the right to remove a donor from the list in extreme cases of disruptive behavior against other community members. Diversity of opinions and constructive criticism will always be welcome in the AsmJit community.

Donation Addresses:

  * BTC: 14dEp5h8jYSxgXB9vcjE8eh78uweD76o7W
  * ETH: 0xd4f0b9424cF31DF5a5359D029CF3A65c500a581E
  * Please contact us if you would like to donate through a different channel or to use a different crypto-currency. Wire transfers and SEPA payments are both possible.

Donors:

  * [ZehMatt](https://github.com/ZehMatt)


Authors & Maintainers
---------------------

  * Petr Kobalicek <kobalicek.petr@gmail.com>
