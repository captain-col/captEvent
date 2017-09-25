#ifndef TSHAHashValue_hxx_seen
#define TSHAHashValue_hxx_seen

#include <stdexcept>
#include <string>

#include <TROOT.h>
#include <TObject.h>

namespace CP {
    class TSHAHashValue;
    std::ostream& operator<<(std::ostream& s, const CP::TSHAHashValue& c);
    bool operator<(const CP::TSHAHashValue& lhs,
                   const CP::TSHAHashValue& rhs);
    bool operator==(const CP::TSHAHashValue& lhs, 
                    const CP::TSHAHashValue& rhs);
    bool operator!=(const CP::TSHAHashValue& lhs, 
                    const CP::TSHAHashValue& rhs);
}

/// Save a SHA hash code value.  The hashcode value can be ordered using the
/// "<" operator and compared for equality using the "==" operator.  The hash
/// code is represented as 5 32 bit unsigned integers.
class CP::TSHAHashValue {
public:
    TSHAHashValue();
    TSHAHashValue(const TSHAHashValue& hc);
    TSHAHashValue(unsigned int hc[5]);
    TSHAHashValue(unsigned int hc0, 
                  unsigned int hc1, 
                  unsigned int hc2, 
                  unsigned int hc3, 
                  unsigned int hc4);
    virtual ~TSHAHashValue();

    /// Return the value of the ith quad.
    unsigned int operator ()(int i) const {
        if (i<0) throw std::out_of_range("TSHAHasValue");
        if (i>4) throw std::out_of_range("TSHAHasValue");
        return fHash[i];
    }

    /// Return the hash value as a string.
    std::string AsString() const;

    /// Set the value of the ith quad.
    void Set(int i, unsigned int v) {
        if (i<0) throw std::out_of_range("TSHAHasValue");
        if (i>4) throw std::out_of_range("TSHAHasValue");
        fHash[i] = v;
    }

    /// Return true if the compared hash code matchs the current hash code
    /// where zero elements are considered wild cards.  So (35,0,0,0,0) matchs
    /// (35,36,0,0,0) and (35,1,2,3,4), but not (36,0,0,0,0).  Note that two
    /// hash codes must share at least one non-zero element to be considered
    /// equivalent so that (0,35,0,0,0) is not equivalent to (36,0,0,0,0).
    /// Likewise two invalid hash codes are not equivalent.
    bool Equivalent(const TSHAHashValue& hc) const;

    /// Return true if the hash code is set.  This returns false if all of the
    /// elements of the hash code are "free" (e.g. equal to zero).
    bool Valid() const;

private:

    /// The hash value.
    unsigned int fHash[5];

    ClassDef(TSHAHashValue,1);
};

#endif
