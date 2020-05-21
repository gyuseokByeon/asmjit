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

#include <asmjit/x86.h>
#include <stdio.h>

using namespace asmjit;

static void printInfo(uint32_t archId, const BaseInst& inst, const Operand_* operands, size_t opCount) {
  StringTmp<256> sb;

  InstRWInfo rw;
  InstAPI::queryRWInfo(archId, inst, operands, opCount, &rw);

  sb.append("Instruction:\n");
  sb.append("  ");
  Formatter::formatInstruction(sb, 0, nullptr, archId, inst, operands, opCount);
  sb.append("\n");

  sb.append("Operands:\n");
  for (uint32_t i = 0; i < rw.opCount(); i++) {
    const OpRWInfo& op = rw.operand(i);
    const char* rw = op.isReadOnly() ? "R" : op.isWriteOnly() ? "W" : "X";

    sb.appendFormat("  [%u] RW=%s ReadBytes=%016llX WriteBytes=%016llX %016llX",
                    i, rw, op.readByteMask(), op.writeByteMask(), op.extendByteMask());
    sb.append("\n");
  }

  BaseFeatures features;
  InstAPI::queryFeatures(archId, inst, operands, opCount, &features);
  if (!features.empty()) {
    sb.append("Features:\n");
    sb.append("  ");

    bool first = true;
    BaseFeatures::Iterator it(features.iterator());
    while (it.hasNext()) {
      uint32_t featureId = uint32_t(it.next());
      if (!first)
        sb.append(" & ");
      Formatter::formatFeature(sb, archId, featureId);
      first = false;
    }
    sb.append("\n");
  }

  printf("%s\n", sb.data());
}

template<typename... Args>
static void printInfoSimple(uint32_t archId, uint32_t instId, Args&&... args) {
  BaseInst inst(instId);
  Operand_ opArray[] = { std::forward<Args>(args)... };
  printInfo(archId, inst, opArray, sizeof...(args));
}

int main() {
  using namespace x86;
  uint32_t archId = ArchInfo::kIdX64;

  printInfoSimple(archId,
                  x86::Inst::kIdAdd,
                  x86::eax, x86::ebx);

  printInfoSimple(archId,
                  x86::Inst::kIdPshufd,
                  x86::xmm0, x86::xmm1, imm(0));

  printInfoSimple(archId,
                  x86::Inst::kIdPextrw,
                  x86::eax, x86::xmm1);

  printInfoSimple(archId,
                  x86::Inst::kIdPextrw,
                  x86::ptr(rax), x86::xmm1);

  printInfoSimple(archId,
                  x86::Inst::kIdVaddpd,
                  x86::ymm0, x86::ymm1, x86::ymm2);

  printInfoSimple(archId,
                  x86::Inst::kIdVaddpd,
                  x86::ymm0, x86::ymm30, x86::ymm31);

  printInfoSimple(archId,
                  x86::Inst::kIdVaddpd,
                  x86::zmm0, x86::zmm1, x86::zmm2);

  return 0;
}
