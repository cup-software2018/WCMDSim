#include <iostream>

#include "MCPrimary.hh"
#include "MCPrimaryData.hh"

ClassImp(MCPrimaryData)

MCPrimaryData::MCPrimaryData()
  : TClonesArray("MCPrimary")
{
}

MCPrimaryData::MCPrimaryData(const MCPrimaryData & data)
  : TClonesArray(data)
{
}

MCPrimaryData::~MCPrimaryData() = default;

MCPrimary * MCPrimaryData::Add() { return new ((*this)[GetEntriesFast()]) MCPrimary(); }

void MCPrimaryData::Clear(Option_t * opt) { TClonesArray::Clear("C"); }

void MCPrimaryData::Print(Option_t * opt) const
{
  int n = GetN();
  std::cout << "===> MCPrimaryData: number of primary: " << n << "\n";
  for (auto obj : *this) {
    static_cast<MCPrimary *>(obj)->Print();
  }
}
