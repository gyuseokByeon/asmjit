// AsmJit - Machine code generation for C++
//
//  * Official AsmJit Home Page: https://asmjit.com
//  * Official Github Repository: https://github.com/asmjit/asmjit
//
// Copyright (c) 2008-2020 The AsmJit Authors
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef ASMJIT_CORE_ARCH_H_INCLUDED
#define ASMJIT_CORE_ARCH_H_INCLUDED

#include "../core/globals.h"
#include "../core/operand.h"

ASMJIT_BEGIN_NAMESPACE

//! \addtogroup asmjit_arch_and_cpu
//! \{

// ============================================================================
// [asmjit::ArchInfo]
// ============================================================================

//! Architecture information.
class ArchInfo {
public:
  union {
    struct {
      //! Architecture id.
      uint8_t _id;
      //! Architecture sub-id.
      uint8_t _subId;
      //! Default size of a general purpose register.
      uint8_t _gpSize;
      //! Count of all general purpose registers.
      uint8_t _gpCount;
    };
    //! Architecture signature (32-bit int).
    uint32_t _signature;
  };

  //! Architecture id.
  enum Id : uint32_t {
    //! No/Unknown architecture.
    kIdNone = 0,

    //! 32-bit X86 architecture.
    kIdX86 = 1,
    //! 64-bit X68 architecture also known as X64, X86_64, and AMD64.
    kIdX64 = 2,

    //! 32-bit ARM architecture also known as AArch32.
    kIdArm32 = 3,
    //! 64-bit ARM architecture also known as AArch64.
    kIdArm64 = 4,

    //! Architecture detected at compile-time (architecture of the host).
    kIdHost  = ASMJIT_ARCH_X86 == 32 ? kIdX86 :
               ASMJIT_ARCH_X86 == 64 ? kIdX64 :
               ASMJIT_ARCH_ARM == 32 ? kIdArm32 :
               ASMJIT_ARCH_ARM == 64 ? kIdArm64 : kIdNone
  };

  //! Architecture sub-type or execution mode.
  enum SubType : uint32_t {
    //! Baseline (or no specific mode).
    kSubIdNone = 0,

    //! Code generation uses AVX by default (VEC instructions).
    kSubIdX86_AVX = 1,
    //! Code generation uses AVX2 by default (VEC instructions).
    kSubIdX86_AVX2 = 2,
    //! Code generation uses AVX512_F by default (+32 vector regs).
    kSubIdX86_AVX512 = 3,
    //! Code generation uses AVX512_VL by default (+VL extensions).
    kSubIdX86_AVX512VL = 4,

    //! THUMB|THUMBv2 sub-type (only ARM in 32-bit mode).
    kSubIdArm32_Thumb  = 8,

#if   ASMJIT_ARCH_X86 && defined(__AVX512VL__)
    kSubIdHost = kSubIdX86_AVX512VL
#elif ASMJIT_ARCH_X86 && defined(__AVX512F__)
    kSubIdHost = kSubIdX86_AVX512
#elif ASMJIT_ARCH_X86 && defined(__AVX2__)
    kSubIdHost = kSubIdX86_AVX2
#elif ASMJIT_ARCH_X86 && defined(__AVX__)
    kSubIdHost = kSubIdX86_AVX
#elif ASMJIT_ARCH_ARM == 32 && (defined(_M_ARMT) || defined(__thumb__) || defined(__thumb2__))
    kSubIdHost = kSubIdArm32_Thumb
#else
    kSubIdHost = 0
#endif
  };

  //! \name Construction & Destruction
  //! \{

  //! Creates an architecture information initialized to none (see \ref kIdNone).
  inline ArchInfo() noexcept : _signature(0) {}
  inline ArchInfo(const ArchInfo& other) noexcept : _signature(other._signature) {}
  inline explicit ArchInfo(uint32_t type, uint32_t subType = kSubIdNone) noexcept { init(type, subType); }
  inline explicit ArchInfo(Globals::NoInit_) noexcept {}

  //! Creates `\ref ArchInfo compatible with the host architecture.
  inline static ArchInfo host() noexcept { return ArchInfo(kIdHost, kSubIdHost); }

  //! Tests whether the ArchInfo has been initialized with correct architecture.
  inline bool isInitialized() const noexcept { return _id != kIdNone; }

  ASMJIT_API void init(uint32_t type, uint32_t subType = kSubIdNone) noexcept;

  //! Resets ArchInfo to default constructed state, which is \ref kIdNone architecture.
  inline void reset() noexcept { _signature = 0; }

  //! \}

  //! \name Overloaded Operators
  //! \{

  inline ArchInfo& operator=(const ArchInfo& other) noexcept = default;

  inline bool operator==(const ArchInfo& other) const noexcept { return _signature == other._signature; }
  inline bool operator!=(const ArchInfo& other) const noexcept { return _signature != other._signature; }

  //! \}

  //! \name Accessors
  //! \{

  //! Returns the architecture id, see `Id`.
  inline uint32_t archId() const noexcept { return _id; }

  //! Returns the architecture sub-id, see `SubType`.
  //!
  //! X86 Specific
  //! ------------
  //!
  //! Architecture subtype describe the highest instruction-set level that can
  //! be used.
  //!
  //! ARM Specific
  //! ------------
  //!
  //! Architecture mode means the instruction encoding to be used when generating
  //! machine code, thus mode can be used to force generation of THUMB and THUMBv2
  //! encoding or regular ARM encoding.
  inline uint32_t archSubId() const noexcept { return _subId; }

  //! Tests whether this architecture is 32-bit.
  inline bool is32Bit() const noexcept { return _gpSize == 4; }
  //! Tests whether this architecture is 64-bit.
  inline bool is64Bit() const noexcept { return _gpSize == 8; }

  //! Tests whether this architecture is X86, X64.
  inline bool isX86Family() const noexcept { return isX86Family(_id); }
  //! Tests whether this architecture is ARM32 or ARM64.
  inline bool isArmFamily() const noexcept { return isArmFamily(_id); }

  //! Returns the native size of a general-purpose register.
  inline uint32_t gpSize() const noexcept { return _gpSize; }
  //! Returns number of general-purpose registers.
  inline uint32_t gpCount() const noexcept { return _gpCount; }

  //! \}

  //! \name Static Functions
  //! \{

  static inline bool isX86Family(uint32_t archId) noexcept { return archId >= kIdX86 && archId <= kIdX64; }
  static inline bool isArmFamily(uint32_t archId) noexcept { return archId >= kIdArm32 && archId <= kIdArm64; }

  //! \}
};

// ============================================================================
// [asmjit::ArchRegs]
// ============================================================================

//! Information about registers of a CPU architecture.
struct ArchRegs {
  //! Register information and signatures indexed by `BaseReg::RegType`.
  RegInfo regInfo[BaseReg::kTypeMax + 1];
  //! Count (maximum) of registers per `BaseReg::RegType`.
  uint8_t regCount[BaseReg::kTypeMax + 1];
  //! Converts RegType to TypeId, see `Type::Id`.
  uint8_t regTypeToTypeId[BaseReg::kTypeMax + 1];
};

// ============================================================================
// [asmjit::ArchUtils]
// ============================================================================

//! Architecture utilities.
struct ArchUtils {
  ASMJIT_API static Error typeIdToRegInfo(uint32_t archId, uint32_t& typeIdInOut, RegInfo& regInfo) noexcept;
};

//! \}

ASMJIT_END_NAMESPACE

#endif // ASMJIT_CORE_ARCH_H_INCLUDED
