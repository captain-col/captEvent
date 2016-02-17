////////////////////////////////////////////////////////////////
// $Id: THandle.hxx,v 1.34 2013/01/11 16:52:32 mcgrew Exp $
//
#ifndef THandle_hxx_seen
#define THandle_hxx_seen

#include <iostream>
#include <typeinfo>

#include <TObject.h>

#include "ECore.hxx"
#include "TCaptLog.hxx"

namespace CP {
    EXCEPTION(EHandle,ECore);
    EXCEPTION(EHandleBadReference,EHandle);

    class TVHandle;
    class THandleBase;
    class THandleBaseDeletable;
    class THandleBaseUndeletable;

    template <class T> class THandle;
    template <class T> T* GetPointer(const THandle<T>& handle);

    template <class T, class U> bool operator < (const THandle<T>& a,
                                                 const THandle<U>&b);

    /// An abstract base class for handles that's used to maintain the
    /// reference count.  This is a deep internal class that can't be directly
    /// accessed.  The original didn't define a constructor, or destructor so
    /// it won't interfere with templated objects that derive from this class,
    /// but they are needed by root so this class has them...
    class TVHandle: public TObject {
    protected:
        TVHandle();
        virtual ~TVHandle();

        /// Define default values for this object.
        void Default(THandleBase* handle);

        /// Add a reference to the object being held by adding this THandle to
        /// the reference list.
        void Link(const TVHandle& rhs);

        /// Remove a reference to the object being held by removing this
        /// THandle from the reference list.  Returns true if the last
        /// reference is removed.
        void Unlink();

        /// If it's possible, delete the object pointed to by the handle.  The
        /// handle is not deleted and will contain a NULL object pointer.
        void DeleteObject(void);

        /// Delete the pointer to the handle for the object.  This will also
        /// delete the object if that is allowed.
        void DeleteHandle(void);
        
        /// Safely get the pointer value for this handle.  This hides the
        /// underlying storage model from the THandle template.
        TObject* GetPointerValue() const;

    public:
        /// Release the ownership of the object being held by this handle.
        /// The responsiblity to delete the object passes to the calling
        /// routine.
        void Release();

        /// Equality operator for all THandle objects.
        bool operator == (const TVHandle& rhs) const;

        /// A deep debugging tool, DO NOT CALL!!!!
        THandleBase* GetInternalHandle() const {return fHandle;}

        /// Print the hit information.
        virtual void ls(Option_t *opt = "") const;
        
    private:
        /// The reference counted handle. This handle contains the pointer to
        /// the actual data object.
        THandleBase* fHandle;

        ClassDef(TVHandle,10);
    };

    /// A handle class that will manage the lifetime (and ownership) of
    /// objects that are derived from TObject.  This doesn't require any
    /// cooperation from the object that will be reference counted.  This
    /// works fine even if the object is owned by ROOT, but really shines if
    /// the object is owned by the program.  The resulting handle is a smart
    /// pointer and (in most respects) looks like a normal pointer.
    /// 
    /// \warning THandle objects which are used in an event loop \b must \b
    /// not be saved between events.  Saving a THandle between events will
    /// cause a memory leak when the output file is read by a later program.
    /// In addition, keep in mind that this is a reference counted object (see
    /// Wikipedia for details).  This means that "reference loops" will
    /// prevent objects from being deleted.
    ///
    /// A reference to a NULL THandle will throw an CP::EHandleBadReference
    /// which means it won't generate a core dump.  For debugging, you can run
    /// the problem under gdb, and set a break point for the
    /// CP::EHandleBadReference constructor.
    ///
    /// \code
    /// catch throw 'CP::EHandleBadReference::EHandleBadReference()'
    /// \endcode
    template <class T> 
    class THandle : public TVHandle {
        template <class U> friend class THandle;
        template <class U> friend U* GetPointer(const THandle<U>& handle);

    public:
        /// Allow a null handle to be constructed. 
        THandle();

        /// Explicitly construct a THandle from a T pointer.  If this isn't
        /// explicit, then C++ will use this constructor as a type conversion
        /// operator.  Making it explicit gives the following behavior:
        /// \code
        /// Pointee* p = new Pointee;
        /// THandle<Pointee> h(p);     // O.K.
        /// THandle<Pointee> g = h;    // O.K.
        /// THandle<Pointee> j = p;    // won't compile
        /// \endcode
        /// While conversion from a pointer to a handle good on "paper", it's
        /// not.  Internally, "h", and "g" share the ownership of "p" and
        /// manage so that "p" remains active until both "g" and "h" are
        /// destroyed.  On the other hand, "h" and "j" both assume full
        /// ownership of "p" which gets deleted as soon as the first one gets
        /// deleted (leaving a dangling reference, a.k.a. BUG).
        explicit THandle(T* pointee);

        /// Create a THandle for an object with an explicit ownership.  This
        /// allows THandle objects to refer to static objects, objects that
        /// are allocated in a buffer, or objects allocated on the stack.  If
        /// the object is owned by the handle (and can be deleted), the owner
        /// argument is true.  If the object is *not* owned by the handle,
        /// the owner argument is false.
        THandle(T* pointee, bool owner);

        /// The copy constructor for this handle.
        THandle(const THandle<T>& rhs);
    
        // Copy between classes.
        template <class U> 
        THandle(const THandle<U>& rhs) {
            Default(NULL);
            if (dynamic_cast<T*>(rhs.GetPointerValue())) {
                Link(rhs);
            }
        }
    
        /// The destructor for the THandle object which may delete the pointer. 
        virtual ~THandle();

    
        /// @{ Assign one THandle object to another.  This should be designed
        /// to recast the pointee between the assignments so that an implicit
        /// conversion takes place.  If the recast fails, the new pointer will
        /// be null.
        ///
        /// \code
        /// THandle<THit> h(new TMCHit);
        /// THandle<TMCHit> mc;
        /// THandle<TComboHit> combo;
        /// mc = h;                  // casts pointer value of h to an TMCHit.
        /// if (mc) cout<<"true";    // prints "true"
        /// combo = h;               // dynamic casts fails so combo holds NULL.
        /// if (combo) cout<<"true"; // No output since combo is NULL.
        /// \endcode
        ///
        /// This provides both const and non-const versions of the assignment.
        THandle<T>& operator = (THandle<T>& rhs);
        const THandle<T>& operator = (const THandle<T>& rhs);
        template <class U> THandle<U>& operator = (THandle<U>& rhs);
        template <class U> const THandle<U>& operator = (const THandle<U>& rhs);
        /// @}
    
        /// The reference operator 
        T& operator*() const;

        /// The redirection operator
        T* operator->() const;

#ifndef __CINT__
    private:
        /// Internal \internal
        /// Prevent the delete operator, while allowing 
        /// \code
        /// THandle<Pointee> h
        /// if (h) return true;
        /// \endcode
        class Tester {
            void operator delete(void*);
        public:
            void bogo() {}
        };
    
    public:
        /// A conversion to a bogus type.  This lets code like this
        ///
        /// \code
        /// THandle<Pointee> h(new Pointee);
        /// if (h) return true;
        /// \endcode
        /// 
        /// compile and work as expected.  The trade off for this behavior is
        /// that we can't implement converters to the pointer type:
        ///
        /// \code
        /// THandle<Pointee> h(new Pointee);
        /// Pointee *p = h;                  // Won't work.
        /// Pointee *p = GetPointer(h);      // Will work.
        /// \endcode
        ///
        /// since it would introduce ambiguities and the compiler won't pick a
        /// method.  Actually, that's probably a benefit, The conversion to a
        /// pointer should be done with GetPointer();
        operator Tester*() const {
            if (!GetPointerValue()) return NULL;
            static Tester test;
            return &test;
        }
#endif

        ClassDefT(THandle,10);
    };
    ClassDefT2(THandle,T);

    /// Turn a THandle object into a pointer.  This is implemented as a
    /// function so that it is *really* clear that something funky is going
    /// on.  You should always pass a THandle, or a THandle reference.
    template <class T> 
    T* GetPointer(const THandle<T>& handle) {
        TObject* object = handle.GetPointerValue();
        if (!object) return NULL;
        T* pointer = dynamic_cast<T*>(object);
        return pointer;
    }
    
    /// Make a comparision between two handles based on the pointer value.
    template <class T, class U>
    bool operator <(const THandle<T>& a, const THandle<U>& b) {
        T* aPtr = GetPointer(a);
        U* bPtr = GetPointer(b);
        return (aPtr < bPtr);
    } 
    

    /// An abstract base class to implement the reference counted internal
    /// object.  The THandleBase objects contain the actual pointer that is
    /// being reference counted.  When the THandleBase object is deleted, the
    /// pointer is also deleted.  This object maintains the reference count.
    class THandleBase : public TObject {
    public:
        THandleBase();
        virtual ~THandleBase();

        int GetReferenceCount() const {return fCount;}
        void DecrementReferenceCount() {--fCount;}
        void IncrementReferenceCount() {++fCount;}
        // Return the current pointer to the object.
        virtual TObject* GetObject() const = 0;
        // Delete the object.  This should check that fObject is a valid
        // pointer (e.g. not NULL) that can be deleted before freeing the
        // memory.  The fObject pointer should always be set to NULL.
        virtual void DeleteObject() = 0;
        void Release() {SetBit(kPointerReleased);}
        bool IsOwner() {return !TestBit(kPointerReleased);}

    private:
        /// Define the status bits used by the THandle object.  These can't
        /// collide with any status bits defined in TObject (the parent class
        /// for THandleBase), and none of the THandleBase children can define
        /// a status bit that collides with these definitions.  Bits 14 to 23
        /// are available for use.
        enum EStatusBits {
            kPointerReleased = BIT(20)
        };
        int fCount;

        ClassDef(THandleBase,2);
    };

    /// A concrete version of the THandleBase class for pointers that should
    /// be deleted when the last reference goes away.  The reference count is
    /// maintained in THandleBase.
    class THandleBaseDeletable : public THandleBase {
    public:
        THandleBaseDeletable();
        THandleBaseDeletable(TObject* object);
        virtual ~THandleBaseDeletable();

        TObject* GetObject() const {return fObject;}
        void DeleteObject();

    private:
        /// The actual pointer that will be reference counted.
        TObject* fObject;

        ClassDef(THandleBaseDeletable,2);
    };

    /// A concrete version of the THandleBase class for pointers that cannot
    /// be deleted when the last reference goes away.  The reference count is
    /// maintained in THandleBase.
    class THandleBaseUndeletable : public THandleBase {
    public:
        THandleBaseUndeletable();
        THandleBaseUndeletable(TObject* object);
        virtual ~THandleBaseUndeletable();

        TObject* GetObject() const {return fObject;}
        void DeleteObject();

    private:
        /// The actual pointer that will be reference counted.
        TObject* fObject;

        ClassDef(THandleBaseUndeletable,2);
    };
}

#ifndef __CINT__
//////////////////////////////////////////////////////////////////
// Implementation of methods.
//////////////////////////////////////////////////////////////////
template <class T>
CP::THandle<T>::THandle(T* pointee) {
    THandleBase *base = NULL;
    if (pointee) base = new THandleBaseDeletable(pointee);
    Default(base);
}

template <class T>
CP::THandle<T>::THandle() {
    Default(NULL);
}

template <class T>
CP::THandle<T>::THandle(T* pointee, bool owner) {
    if (pointee) {
        if (owner) 
            Default(new THandleBaseDeletable(pointee));
        else 
            Default(new THandleBaseUndeletable(pointee));
    }    
    else {
        Default(NULL);
    }
}

template <class T>
CP::THandle<T>::THandle(const THandle<T>& rhs) : TVHandle(rhs) {
    Default(NULL);
    Link(rhs);
}

template <class T>
CP::THandle<T>::~THandle() {
    Unlink();
}
    
template <class T>
CP::THandle<T>& CP::THandle<T>::operator = (THandle<T>& rhs) {
    if (operator == (rhs)) return rhs;
    // Going to replace the value of this smart pointer, so unref and
    // possibly delete.
    Unlink();
    // Make sure the handle in the default state.
    Default(NULL);
    // Compatible types
    if (dynamic_cast<T*>(rhs.GetPointerValue())) {
        Link(rhs);
    }
    return rhs;
}
    
template <class T> 
const CP::THandle<T>& CP::THandle<T>::operator = (const THandle<T>& rhs) {
    // Going to replace the value of this smart pointer, so unref and
    // possible delete.
    Unlink();
    // Make sure the handle in the default state.
    Default(NULL);
    // Compatible types
    if (dynamic_cast<T*>(rhs.GetPointerValue())) {
        Link(rhs);
    }
    return rhs;
}

template <class T> 
template <class U>
CP::THandle<U>& CP::THandle<T>::operator = (THandle<U>& rhs) {
    // Going to replace the value of this smart pointer, so unref and
    // possible delete.
    Unlink();
    // Make sure the handle in the default state.
    Default(NULL);
    // Compatible types
    if (dynamic_cast<T*>(rhs.GetPointerValue())) {
        Link(rhs);
    }
    return rhs;
}

template <class T> 
template <class U>
const CP::THandle<U>& CP::THandle<T>::operator = (const THandle<U>& rhs) {
    // Going to replace the value of this smart pointer, so unref and
    // possible delete.
    Unlink();
    // Make sure the handle in the default state.
    Default(NULL);
    // Compatible types
    if (dynamic_cast<T*>(rhs.GetPointerValue())) {
        Link(rhs);
    }
    return rhs;
}

template <class T>
T& CP::THandle<T>::operator*() const {
    TObject* object = GetPointerValue();
    if (!object) {
        CaptError("Dereferencing a NULL handle " << typeid(T).name());
        
        throw EHandleBadReference();
    }
    T* pointer = dynamic_cast<T*>(object);
    if (!pointer) {
        CaptError("Dereferencing with an invalid cast "
                  << typeid(T).name());
        throw EHandleBadReference();
    }
    return *pointer;
}

template <class T> 
T* CP::THandle<T>::operator->() const {
    TObject* object = GetPointerValue();
    if (!object) {
        CaptError("Referencing a NULL handle " << typeid(T).name());
        throw EHandleBadReference();
    }
    T* pointer = dynamic_cast<T*>(object);
    if (!pointer) {
        CaptError("Referencing with an invalid cast"
                  << typeid(T).name());
        throw EHandleBadReference();
    }
    return pointer;
}
#endif

#endif
