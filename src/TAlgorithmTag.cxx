#include <iostream>
#include <TROOT.h>
#include <TClass.h>
#include <TStreamerInfo.h>

#include "TAlgorithm.hxx"
#include "TAlgorithmTag.hxx"
#include "TOADatabase.hxx"
#include "TPackageSet.hxx"

ClassImp(CP::TAlgorithmTag);

CP::TAlgorithmTag::~TAlgorithmTag() {}

CP::TAlgorithmTag::TAlgorithmTag() 
    : TNamed("unnamed","Algorithm Tag") { }

CP::TAlgorithmTag::TAlgorithmTag(const CP::TAlgorithm& algo) 
    : TNamed(algo.GetName(), "T2K Algorithm Tag") {

    std::string tag = 
        std::string(algo.GetName()) + ", " + std::string(algo.GetVersion());
    Add(tag);

    const CP::TPackageSet& packages = CP::TOADatabase::Get().PackageSet();
    for (CP::TPackageSet::const_iterator p = packages.begin();
         p != packages.end();
         ++p) {
        tag = std::string((*p)->GetName())
            + ", " + (*p)->GetVersion() 
            + ", " + (*p)->GetCompilationDate();
        Add(tag);
    }
}

void CP::TAlgorithmTag::Add(const char* info) {
    fTag.push_back(info);
}

void CP::TAlgorithmTag::Add(const std::string& info) {
    fTag.push_back(info);
}

void CP::TAlgorithmTag::ls(Option_t* opt) const {
    CP::ls_header(this,opt);
    std::cout << ": \"" << GetTitle() << "\""
              << std::endl;
    std::string option(opt);
    if (option.find("dump") != std::string::npos
        || option.find("tag") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        for (std::vector<std::string>::const_iterator v = fTag.begin();
             v != fTag.end(); 
             ++v) {
            TROOT::IndentLevel();
            std::cout << "Tag: " << *v << std::endl;
        }
        TROOT::DecreaseDirLevel();
    }
}
