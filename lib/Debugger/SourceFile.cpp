//===-- SourceFile.cpp - SourceFile implementation for the debugger -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SourceFile class for the LLVM debugger.
//
//===----------------------------------------------------------------------===//

#include "llvm/Debugger/SourceFile.h"
#include <cassert>

using namespace llvm;

/// readFile - Load Filename
///
void SourceFile::readFile() {
  std::string ErrMsg;
  if (!File.map(&ErrMsg))
    throw ErrMsg;
}

/// calculateLineOffsets - Compute the LineOffset vector for the current file.
///
void SourceFile::calculateLineOffsets() const {
  assert(LineOffset.empty() && "Line offsets already computed!");
  const char *BufPtr = (const char *)File.getBase();
  const char *FileStart = BufPtr;
  const char *FileEnd = FileStart + File.size();
  do {
    LineOffset.push_back(BufPtr-FileStart);

    // Scan until we get to a newline.
    while (BufPtr != FileEnd && *BufPtr != '\n' && *BufPtr != '\r')
      ++BufPtr;

    if (BufPtr != FileEnd) {
      ++BufPtr;               // Skip over the \n or \r
      if (BufPtr[-1] == '\r' && BufPtr != FileEnd && BufPtr[0] == '\n')
        ++BufPtr;   // Skip over dos/windows style \r\n's
    }
  } while (BufPtr != FileEnd);
}


/// getSourceLine - Given a line number, return the start and end of the line
/// in the file.  If the line number is invalid, or if the file could not be
/// loaded, null pointers are returned for the start and end of the file. Note
/// that line numbers start with 0, not 1.
void SourceFile::getSourceLine(unsigned LineNo, const char *&LineStart,
                               const char *&LineEnd) const {
  LineStart = LineEnd = 0;
  if (!File.isMapped()) return;  // Couldn't load file, return null pointers
  if (LineOffset.empty()) calculateLineOffsets();

  // Asking for an out-of-range line number?
  if (LineNo >= LineOffset.size()) return;

  // Otherwise, they are asking for a valid line, which we can fulfill.
  LineStart = (const char *)File.getBase()+LineOffset[LineNo];

  if (LineNo+1 < LineOffset.size())
    LineEnd = (const char *)File.getBase()+LineOffset[LineNo+1];
  else
    LineEnd = (const char *)File.getBase() + File.size();

  // If the line ended with a newline, strip it off.
  while (LineEnd != LineStart && (LineEnd[-1] == '\n' || LineEnd[-1] == '\r'))
    --LineEnd;

  assert(LineEnd >= LineStart && "We somehow got our pointers swizzled!");
}
