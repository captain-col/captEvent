#include "TND280MCHeader.hxx"
#include "HEPUnits.hxx"

ClassImp(ND::TND280MCHeader);

ND::TND280MCHeader::TND280MCHeader()
    : ND::TDataVector("mcHeader", "Monte Carlo Header"),
      fOffAxisField(0.0), fIntensity(0.0) {}

ND::TND280MCHeader::TND280MCHeader(const char* name, const char* title)
    : ND::TDataVector(name,title), fOffAxisField(0.0), fIntensity(0.0) {}

ND::TND280MCHeader::~TND280MCHeader() {}

void ND::TND280MCHeader::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Geometry Hash: " << GetGeometryHash() 
              << std::endl;
    TROOT::IndentLevel();
    std::cout << "Off Axis Magnetic Field: " << GetOffAxisField()/unit::tesla
              << " T"
              << std::endl;
    TROOT::IndentLevel();
    std::cout << "Intensity:               " << GetIntensity()
              << std::endl;
    TROOT::DecreaseDirLevel();
}
