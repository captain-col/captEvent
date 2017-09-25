#ifndef TDigitProxy_hxx_seen
#define TDigitProxy_hxx_seen

#include <vector>
#include <TROOT.h>

#include "TDigit.hxx"
#include "TDigitContainer.hxx"

namespace CP {
    /// An exception that should be thrown when the needed digit could not be
    /// found by the TDigitProxy.
    EXCEPTION(EDigitNotFound,EDigit);

    /// An exception that should be thrown when the needed digit container
    /// could not be found by the TDigitProxy.
    EXCEPTION(EDigitNotAvailable,EDigitNotFound);

    /// An exception that should be thrown when the salt for the digit doesn't
    /// match.
    EXCEPTION(EDigitMismatch,EDigitNotFound);

    /// Thrown if the digit type is not valid.
    EXCEPTION(EDigitTypeInvalid,EDigitNotFound);

    // A handle to reference a TDigit object that may or may not be available.
    class TDigitProxy;

    class TDigitManager;
    class TDigitContainer;
}

/// A handle to reference a TDigit object that may or may not be available.
/// This differs from a THandle in that the proxy doesn't keep the object from
/// being deleted.  If the object doesn't exist when accessed, this will throw
/// an EDigitNotFound exception.
class CP::TDigitProxy {
    friend class TDigitManager;

public:
    enum ProxyType {
        kInvalid = 0,
        kTest,
        kDrift,
        kPhotosensor,
    };

    // The data type used to store the signature.
    typedef int ProxyData;

    TDigitProxy();
    virtual ~TDigitProxy();

    /// Construct a proxy for a digit in a container.  This constructs a proxy
    /// for a digit using the digit container, and the offset (index in the
    /// vector) of the digit within the container.  Use this interface if you
    /// are iterating over the index of the digits:
    /// \code
    /// for (unsigned int offset = 0; offset<container.size(); ++offset) {
    ///    TWritableData       
    /// }
    /// \endcode
    TDigitProxy(const CP::TDigitContainer& container, 
                unsigned int offset);
    
    /// Construct a proxy for a digit in a container.
    TDigitProxy(const CP::TDigitContainer& container, 
                const CP::TDigitContainer::iterator index);
    
    /// Construct a proxy for a digit in a container.
    TDigitProxy(const CP::TDigitContainer& container, 
                const CP::TDigitContainer::const_iterator index);
    
    /// Construct a digit from an integer value.  This is used to allow
    /// TDigit's to be written as a single integer an output file and saves
    /// significant space in the THit record.
    explicit TDigitProxy(int proxy);

    /// Flag if the digit could be found in the event.  If this returns
    /// invalid, the the digit is not available for some reason (perhaps the
    /// raw data has been stripped from the event.
    bool IsValid() const;

    /// Return the pointer to the indexed digit.  If the digit is not
    /// available the an CP::EDigitNotFound exception is thrown.
    TDigit* operator*() const;
    
    /// Return a reference the container that holds the digit.
    CP::TDigitContainer& GetContainer() const;

    /// Return the digit as a particular type.  This gets the digit and then
    /// converts it into the requested type.  If the digit is not available an
    /// EDigitNotFound exception is thrown.  If the digit is of the wrong type
    /// this method returns NULL.
    template <class T>
    T* As() const {
        TDigit* d = this->operator*();
        if (!d) return NULL;
        return dynamic_cast<T*>(d);
    }

    /// Check that the digit proxy salt matchs the container and digit.
    bool CheckSalt(unsigned int signature, const CP::TDigit* digit) const;

    /// Convert a proxy name into a proxy enum type.
    static enum ProxyType ConvertName(std::string name);

    /// Convert a proxy type into a proxy name.
    static std::string ConvertType(int type);

    /// Convert the proxy into a string.
    std::string AsString(void) const;

private:
    /// Get the proxy data type out of the proxy signature.
    enum ProxyType GetProxyType() const;

    /// Get the salt out of the proxy signature.  The salt is a simple check
    /// that the correct digit has been found.
    unsigned int GetProxySalt() const;

    /// Return the offset of the digit from the proxy signature.
    unsigned int GetProxyOffset() const;

    /// Return the cached digit pointer.
    CP::TDigit* GetProxyCache() const;

    /// Set the cached digit pointer.
    void SetProxyCache(CP::TDigit *const digit, 
                       CP::TDigitContainer *const container) const;

    /// Set the proxy type in the signature.
    void SetProxyType(int type);

    /// Set the proxy salt in the signature.  The salt is a simple check that
    /// the correct digit has been found.
    void SetProxySalt(int salt);

    /// Set the proxy offset of the digit in the proxy signature.
    void SetProxyOffset(int offset);

    /// The signature of the digit referenced in this proxy.  The signature
    /// fits in a 32 bit field and has the following bit definitions.
    ///
    /// dddd dsss ssss ssso oooo oooo oooo oooo
    ///
    /// - d: 5 bits specifying the detector
    /// - s: 10 bits of salt to verify the right digit is found.
    /// - o: 17 bits of offset within the collection of digits.
    /// 
    ProxyData fDigitSignature; 
    
    /// A cache for the digit referenced by this proxy.  This is mutable so it
    /// can be set even if the TDigitProxy is constant.
    mutable CP::TDigit* fDigit; //! Do not save.

    /// A cache to contain a pointer to the container that holds the digit.
    /// This is mutable so it can be set even if the TDigitProxy is constant.
    mutable CP::TDigitContainer* fContainer; //! Do not save.

    virtual bool operator == (const CP::TDigitProxy& rhs) const;

    ClassDef(TDigitProxy,1);
};
#endif



