#ifndef TGeomIdManager_hxx_seen
#define TGeomIdManager_hxx_seen

#include <string>
#include <vector>
#include <map>

#include <TVector3.h>
#include <TFile.h>

class TGeoManager;

#include "ECore.hxx"
#include "TGeometryId.hxx"
#include "TSHA1.hxx"
#include "TSHAHashValue.hxx"
#include "TAlignmentId.hxx"
#include "TEventContext.hxx"

namespace CP {
    class TGeomIdFinder;
    class TGeomIdManager;
    class TManager;
    class TEvent;
}

/// Translate TGeometryId objects into TGeoNode pointers used with
/// TGeoManager.  This is a singleton object managed and owned by TManager.
/// The geometry identifier provides a non-dynamic (and stable) way for the
/// software to uniquely identify a particular volume (e.g. a scintillator
/// bar, a TPC pad, &c).  However, for efficiency ROOT needs to use TGeoNode
/// identifiers which must be assigned dynamically when a geometry is open.
/// This method builds a map between the persistent geometry identifiers and
/// the dynamic node identifiers.
///
/// The proper way to find out ROOT based information from a geometry
/// identifier is to use TGeomIdManager::CdId(), and the interrogate the
/// TGeoManager object.  Some convenience routines have been provided for
/// common cases (TGeomIdManager::GetPosition(), and
/// TGeomIdManager::GetGeometryId()).  The name of the volume associated with
/// a TGeometryId object is available through TGeometryId::GetName().
class CP::TGeomIdManager {
    friend class CP::TManager;
    friend class TGeometryId;

public:
    /// A type used to find a root TGeoNode in the gGeoManager, that has been
    /// abstracted so that it's possible to change the key type.  As long as
    /// the geometry is small enough, this key will be a node id.
    typedef int RootGeoKey;

    /// An abstraction of the TGeometryId value that is used to find the
    /// RootGeoKey.  The GeomIdKey is built from the TGeometryId.
    typedef int GeomIdKey;

    /// A map between a GeomIdKey key that is derived from a TGeometryId object
    /// and the RootGeoKey keys used to find the geometry nodes.
    typedef std::map<GeomIdKey,RootGeoKey> GeomIdMap;

    /// A map between a RootGeoKey and a GeomIdKey
    typedef std::map<RootGeoKey,GeomIdKey> RootIdMap;

    ~TGeomIdManager();

    /// Change the current node to the TGeometryId.  This changes the state of
    /// the gGeoManager.
    bool CdId(TGeometryId id) const;

    /// Get the TGeometryId value associated with the current ROOT node
    /// identifier.  If the current node id doesn't have a geometry
    /// identifier, move up one level in the root geometry and checks again.
    /// If a geometry identifier is not found, then this returns false.
    bool FindGeometryId(TGeometryId& id) const;
    
    /// Get the TGeometryId value associated with a global position.  The
    /// return value will be false if the position doesn't correspond to a
    /// volume identified by a TGeometryId.
    bool GetGeometryId(double x, double y, double z, TGeometryId& id) const;

    /// Get the position of the center of the geometry id.  This changes the
    /// state of the gGeoManager to make the node associated with the geometry
    /// id into the current node.  This position includes any affects of
    /// alignment applied to the gGeoManager.  The return value will be false
    /// if the TGeometryId object is invalid.
    bool GetPosition(TGeometryId id, TVector3& position) const;

    /// Get the hash keys for the currently loaded geometry.
    const TSHAHashValue& GetHash() const {return fGeomIdHashCode;}

    /// Get the alignment id currently applied to the loaded geometry.
    const TAlignmentId& GetAlignmentId() const {return fGeomIdAlignmentId;}

    /// Provide a geometry matching a particular hash.  This opens the file
    /// and reads the geometry.  The result is provided using the TGeoManager
    /// global gGeoManager.  If this is successful, it returns true (and
    /// gGeoManager will non-NULL), otherwise it returns false.  If false is
    /// returned, then the gGeoManager pointer will not have changed.  This
    /// requires the first hash, but the other hashs may be left to the
    /// default value.  If the other hashs have a non-zero value, then the
    /// file name is searched for that hash.
    /// 
    /// The geometry is found by looking in a standarized location
    /// (${CAPTEVENTROOT}/${CAPTEVENTCONFIG}/geometry) for a file which
    /// advertises a geometry matching the requested hash value.  This uses
    /// LoadGeometry to read the geometry file.
    bool ReadGeometry(const TSHAHashValue& hc);

    /// Provide a standard method to load the geometry out of a TFile.  The
    /// TFile must be open for input before this is called.  This will return
    /// true if the geometry was found and it matched the hash code (or there
    /// was a geometry in the file named "CAPTAINGeometry"), otherwise it will
    /// return false.  If false is returned, then the gGeoManager pointer will
    /// not have changed.  This uses ResetGeometry to update the state of the
    /// geometry manager GeometryId tables and hash codes.
    ///
    /// If an alignment id is provided, this will load the geometry matching
    /// both the hash code and the alignment.  If it can't find a geometry
    /// matching both, but does find a geometry matching the hash code, that
    /// geometry will be loaded.
    bool LoadGeometry(TFile& file, 
                      const TSHAHashValue& hc = CP::TSHAHashValue(),
                      const TAlignmentId& align = CP::TAlignmentId());

    /// Update the state of the geometry id and hash code tables using the
    /// gGeoManager geometry.  If gGeoManager isn't a valid geometry, then the
    /// tables are left in an "empty" state.  This is used by LoadGeometry,
    /// and can be used when you have a geometry already loaded in memory
    /// (e.g. to update the TGeomIdManager to match the MC geometry in the
    /// detector simulation). After this has been successfully called
    /// (returned true), gGeoManager will point to a geometry named
    /// CAPTAINGeometry-xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx-xxxxxxxx" where
    /// "x" stands for a lower case hexidecimal digit.
    void ResetGeometry();

    /// A map of geometry id represented as integers to the associated ROOT
    /// node id.  This can be used to iterate over all of the geometry
    /// identifiers associated with a geometry.  This exposes integers since
    /// it is exposing an internal table that is optimized for space.  This
    /// table can be used
    ///
    /// \code
    /// for (TGeomIdManager::GeomIdMap::iterator g 
    ///         = CP::TManager::Get().GeomId().GetGeomIdMap().begin();
    ///      g != CP::TManager::Get().GeomId().GetGeomIdMap().end();
    ///      ++g) {
    ///     TGeometryId gid(g->first);
    /// }
    /// \endcode
    const GeomIdMap& GetGeomIdMap() {return fGeomIdMap;}
    
    /// Provide a file to override the standard geometry.  Setting the
    /// geometry file to any value other than "" will cause TManager to
    /// override the standard geometry.
    void SetGeometryFileOverride(std::string f) {fGeometryFileOverride = f;}

    /// Get the name of a file to override the standard geometry.
    const std::string& GetGeometryFileOverride() const {
        return fGeometryFileOverride;
    }

    /// Set a geometry hash value to override the standard geometry.  This
    /// resets the required hash if called without an argument.
    void SetGeometryHashOverride(const TSHAHashValue& h) {
        fGeometryHashOverride = h;
    }

    /// Get the hash code that has been set to override the default geometry.
    /// This will be "zero" if the hashcode hasn't been set.
    const TSHAHashValue& GetGeometryHashOverride() const {
        return fGeometryHashOverride;
    }

    /// Apply the alignment to the current geometry.  The alignment is usually
    /// done automatically when the geometry is loaded, but this can be called
    /// multiple times by the user as well.  Each time it is called, the
    /// current set of physical nodes is cleared and the alignment is
    /// reapplied to the baseline geometry.  
    ///
    /// Be careful, the ROOT physical geometry alignment code is a relatively
    /// slow.  In practical terms, the alignment should only be applied when
    /// the geometry is loaded.  
    void ApplyAlignment(const CP::TEvent* const event);

private:
    /// Construct a new TGeomIdManager.  This is private since it should only
    /// be constructed by the friend class TManager.
    TGeomIdManager();
    
    /// Make the copy constructor private.
    TGeomIdManager(const TGeomIdManager&);

    /// Can't assign the class.
    bool operator =(const TGeomIdManager&);

    /// An abstraction of finding the geometry node based on the RootGeoKey.
    /// This is the only method that needs to change if the RootGeoKey
    /// definition changes.  This method has the effect of changing the
    /// TGeoManager current node.
    bool CdKey(const RootGeoKey& key) const;

    /// Using the current gGeoManager node, build a RootGeoKey.  This must
    /// preserve the gGeoManager state.
    RootGeoKey MakeRootGeoKey() const;
    
    /// Build a GeomIdKey from a TGeometryId.
    GeomIdKey MakeGeomIdKey(TGeometryId id) const;

    /// Make a TGeometryId object out of a GeomIdKey;
    TGeometryId MakeGeometryId(GeomIdKey key) const;

    /// Find the geometry path associated with a geometry identifier.  This
    /// returns "" if the path is not available.  This a private method used
    /// to implement TGeometryId::GetName().
    std::string GetPath(TGeometryId id) const;

    /// Build the map between the geometry identifier and the root TGeoManager
    /// volumes in gGeoManager.
    void BuildGeomIdMap();

    /// An internal method to recurse through the entire ROOT geometry.  This
    /// does a depth first recursion through the geometry.  It passes a vector
    /// containing the volume names for the current recursion, and will keep
    /// recursing down a particular branch as long as keepGoing is not zero.
    /// This returns the value of keepGoing which will be zero the recursion
    /// has been truncated.
    int RecurseGeomId(std::vector<std::string>& names, int keepGoing);

    /// Save a hash code into the current gGeoManager object name.
    void SaveHashCode(const CP::TSHAHashValue& hc);

    /// Get the hash code out of the current gGeoManager object name.  This
    /// will return false if the gGeoManager object name does not contain the
    /// hash code.
    bool GetHashCode(CP::TSHAHashValue& hc) const;

    /// Save an alignment id into the current gGeoManager object name.
    void SaveAlignmentCode(const CP::TAlignmentId& hc);

    /// Get the alignment id out of the current gGeoManager object name.  This
    /// will return false if the gGeoManager object name does not contain the
    /// id.
    bool GetAlignmentCode(CP::TAlignmentId& hc) const;

    /// Parse the hash code out of a string.  This will return false if the
    /// string does not contain the hash code.  The string should be stripped
    /// of all characters up to the first hexidecimal digit, and can be either
    /// a the geometry hash code or the alignment id hash code.
    bool ParseHashCode(std::string hashCode, CP::TSHAHashValue& hc) const;

    /// Calculate the geometry hash code for the current gGeoManager.  Be
    /// aware that the result depends on the machine where it is being run.
    void BuildHashCode();

    /// An internal method to recurse through the entire ROOT geometry.  This
    /// does a depth first recursion through the geometry.  It passes a vector
    /// containing the volume names for the current recursion.
    void RecurseHashCode(std::vector<std::string>& names);

    /// Find a file with a geometry matching a particular hash.  Looks in a
    /// standarized location for a file which advertises a geometry matching
    /// the requested hash value.  If the file is found, this returns the file
    /// name, otherwise it returns an empty string.  This requires the first
    /// hash, but the other hashs may be left to the default value.  If the
    /// other hashs have a non-zero value, then the file name is searched for
    /// that hash.
    std::string FindGeometryFile(const CP::TSHAHashValue& hc) const;

    /// Find and load the geometry that best matchs this event.  If this
    /// returns true, then a new geometry was loaded.  If event is non-null,
    /// then it will be used to try and select the best geometry.  This uses
    /// ReadGeometry, LoadGeometry and ResetGeometry to load the actual
    /// geometry manager so the geometry id and hash code tables are updated
    /// by this method.
    bool FindAndLoadGeometry(CP::TEvent* event);

    /// Determine if the geometry should be looked for.  If this returns true,
    /// the a new geometry should be loaded.
    bool CheckGeometry(const CP::TEvent* const event);

    /// Check if the alignment might have changed.
    bool CheckAlignment(const CP::TEvent* const event);

    /// Load the correct geometry.  This is provided to TManager to
    /// implement CP::TManager::Geometry().
    TGeoManager* GetGeometry(CP::TEvent* event);

    /// Set the pointer to the current geometry manager.  This sets both the
    /// fGeoManager field and the gGeoManager global.
    void SetGeoManager(TGeoManager* g);

    /// Return a pointer to the currently loaded geometry and make sure that
    /// gGeoManager is pointing to the correct geometry.
    TGeoManager* GetGeoManager();

    /// Set the geometry time stamp.
    void SetGeomEventContext(const CP::TEventContext& c) {fGeomEventContext=c;}

    /// Get the geometry time stamp.
    const CP::TEventContext& GetGeomEventContext() const {
        return fGeomEventContext;
    }

    /// The current geometry.
    TGeoManager* fGeoManager;

    /// The map between the GeomIdKey and the RootIdKey
    GeomIdMap fGeomIdMap;

    /// The map between the RootIdKey and the GeomIdKey.
    RootIdMap fRootIdMap;

    /// The hash code for the geometry associated with fGeomIdMap and
    /// fRootIdMap.  This is used to short circuit the BuildGeomIdMap method.
    TSHAHashValue fGeomIdHashCode;

    /// Keep track of the hash code of the last geometry the user routines
    /// have been told about via the ApplyGeometryCallbacks method.
    TSHAHashValue fGeomIdChangedHash;

    /// The alignment identifier associated with the current geometry.
    TAlignmentId fGeomIdAlignmentId;

    /// The context of the last event that was checked to make sure that the
    /// geometry was valid.  This is used to short circuit the geometry check
    /// when the event geometry hash code and alignment id haven't been set.
    CP::TEventContext fGeomEventContext;

    /// The class to calculate the SHA1 message digest.  This is used in
    /// RecurseHashCode.
    CP::TSHA1 fSHA1;

    /// A vector of TGeomIdFinder objects.
    std::vector<TGeomIdFinder*> fFinders;

    /// The name of the geometry override file (if it has been set).
    std::string fGeometryFileOverride;

    /// The hash code for the geometry to be used (if it has been set).  If
    /// this is set, then a geometry with this hash code will override the
    /// default geometry. 
    TSHAHashValue fGeometryHashOverride;

};
#endif
