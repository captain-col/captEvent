#include <TROOT.h>

#include <eventLoop.hxx>

/// Simple event loop to remove the digits from an event file.  Removing the
/// digits from an event file will drastically reduce the file size.  For an
/// MC file, the resulting file is generally between 3% and 4% of the original
/// file size.  This can be run on any event file, but it only does something
/// if there are digits.  All of the digit containers under ~/digits are
/// removed.
class TStripDigits: public CP::TEventLoopFunction {
public:
    TStripDigits() {
        fQuiet = false;
    }

    virtual ~TStripDigits() {};

    void Usage(void) {
        std::cout << "    -O quiet"
                  << std::endl;
    }

    virtual bool SetOption(std::string option,std::string value="") {
        if (value != "") return false;
        if (option == "quiet") fQuiet = true;
        return true;
    }

    bool operator () (CP::TEvent& event) {
        CP::THandle<CP::TDataVector> digits
            = event.Get<CP::TDataVector>("~/digits");
        if (!digits) return true;
        
        if (!fQuiet) digits->ls();

        CP::TDataVector::iterator d = digits->begin();
        while (d != digits->end()) {
            if (!fQuiet) {
                std::cout << "     REMOVE: " << (*d)->GetName() << std::endl;
            }
            d = digits->erase(d);
        }

        return true;
    }

private:
    bool fQuiet;
};

int main(int argc, char **argv) {
    TStripDigits userCode;
    CP::eventLoop(argc,argv,userCode);
}

