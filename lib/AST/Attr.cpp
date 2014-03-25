//===--- Attr.cpp - Swift Language Attr ASTs ------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
//  This file implements routines relating to declaration attributes.
//
//===----------------------------------------------------------------------===//

#include "swift/AST/Attr.h"
#include "swift/AST/ASTContext.h"
#include "swift/AST/ASTPrinter.h"
#include "swift/AST/Decl.h"
#include "swift/AST/Module.h"
#include "swift/AST/Types.h"

using namespace swift;


// Only allow allocation of attributes using the allocator in ASTContext.
void *AttributeBase::operator new(size_t Bytes, ASTContext &C,
                                  unsigned Alignment) {
  return C.Allocate(Bytes, Alignment);
}

/// A statically-allocated empty set of attributes.
const DeclAttributes Decl::EmptyAttrs;

DeclAttributes &Decl::getMutableAttrs() {
  // If we don't have mutable attribute storage yet, allocate some.
  if (&getAttrs() == &EmptyAttrs)
    AttrsAndIsObjC = {getASTContext().Allocate<DeclAttributes>(),
                      AttrsAndIsObjC.getInt()};
  return *const_cast<DeclAttributes*>(&getAttrs());
}

void DeclAttributes::print(llvm::raw_ostream &OS) const {
  StreamPrinter P(OS);
  print(P);
}

void DeclAttributes::print(ASTPrinter &Printer) const {
  if (NumAttrsSet == 0)
    return;

  if (isAssignment())
    Printer << "@assignment ";
  if (isConversion())
    Printer << "@conversion ";
  if (isTransparent())
    Printer << "@transparent ";
  if (isInfix())
    Printer << "@infix ";
  switch (getResilienceKind()) {
  case Resilience::Default: break;
  case Resilience::Fragile: Printer << "@fragile "; break;
  case Resilience::InherentlyFragile: Printer << "@born_fragile "; break;
  case Resilience::Resilient: Printer << "@resilient "; break;
  }
  if (isNoReturn())
    Printer << "@noreturn ";
  if (isPostfix())
    Printer << "@postfix ";
  if (isObjC())
    Printer << "@objc ";
  if (requiresStoredPropertyInits())
    Printer << "@requires_stored_property_inits ";
  if (isIBOutlet())
    Printer << "@IBOutlet ";
  if (isIBAction())
    Printer << "@IBAction ";
  if (isClassProtocol())
    Printer << "@class_protocol ";
  if (isExported())
    Printer << "@exported ";
  if (isOptional())
    Printer << "@optional ";
  Optional<bool> MutatingAttr = getMutating();
  if (MutatingAttr && MutatingAttr.getValue())
    Printer << "@mutating ";
  if (MutatingAttr && !MutatingAttr.getValue())
    Printer << "@!mutating ";
  if (isRequired())
    Printer << "@required ";
  if (isOverride())
    Printer << "@override ";

  for (auto DA : *this)
    DA->print(Printer);
}

void DeclAttribute::print(ASTPrinter &Printer) const {
  switch (getKind()) {
    case DAK_asmname:
      Printer << "@asmname(\"" << cast<AsmnameAttr>(this)->Name << "\")";
      break;
    case DAK_Count:
      llvm_unreachable("exceed declaration attribute kinds");
  }
  Printer << ' ';
}

void DeclAttribute::print(llvm::raw_ostream &OS) const {
  StreamPrinter P(OS);
  print(P);
}
