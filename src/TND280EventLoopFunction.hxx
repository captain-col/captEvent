#ifndef TND280EventLoopFunction_hxx_seen
#define TND280EventLoopFunction_hxx_seen

#include "EoaCore.hxx"

#include "TND280Event.hxx"
#include "method_deprecated.hxx"

namespace CP {
    class TND280EventLoopFunction;
    class TVInputFile;
    class TND280Output;

    /// Terminate an event loop that is processing a particular file.  This
    /// should be thrown by the event loop user function when it wants to stop
    /// processing the current file and move to the next.
    OA_EXCEPTION(ENextEventLoopFile,EoaCore);
};

/// A base class for use with the CP::captainEventLoop() which defines the
/// command line options.  User classes should implement the
/// operator()(TND280Event& event) method that returns true if the event
/// should be saved to an output file.  If false is returned, then the event
/// is not saved.  If you want to have multiple output files, you can use the
/// Process(TND280Event&, int files) method which should return -1 (to not
/// save the event), or the index of the file to save the event into [0 to
/// files-1].  This should throw an ENextEventLoopFile to stop processing a
/// particular file.  Documentation on writing and using event loops is
/// provided by the tutorial in \ref eventLoop
///
/// The event loop code is used as follows:
///
/// \code
/// class TUserCode: public TND280EventLoopFunction {
/// public:
///     void Initialize() {
///         // Do something
///     }
///     bool operator () (TND280Event& event) {
///         // User Code
///         return true;
///     }
///     void Finalize(TND280Output* file) {
///         // Called before exiting.  If no file, then parameter is NULL.
///     }
/// };
///
/// int main(int argc, char **argv) {
///     TUserCode userCode;
///     CP::captainEventLoop(argc,argv,userCode);
/// }
/// \endcode
///
/// This calls the "()" operator once for each event.  The Initialize() code
/// is called before the input file is opened.  The Finalization() method is
/// called after the last input file is closed, but before the output file is
/// closed.
///
/*!
 * \htmlonly
 * There is skeleton code available in 
 * <a href="../doc/exampleEventLoop.cxx">exampleEventLoop.cxx</a> that can
 * be copied to a local directory and modified.  If you use this code, do a
 * global search and replace on "UserLoop", changing it to some more useful
 * name.  That will help keep the doxygen documentation useful.
 * \endhtmlonly
 */
class CP::TND280EventLoopFunction {
public:

    TND280EventLoopFunction ();
    virtual ~TND280EventLoopFunction ();

    /// Called for each event inside the event loop, and returns true if the
    /// event should be saved to the output file.  If the remainder of the
    /// current file should be skipped, this should through the
    /// ENextEventLoopFile exception.  if the daughter class defines this
    /// operator, it should not define the Process() method.
    virtual bool operator () (TND280Event& event);

    /// Process a file and write into multiple output files.  This is called
    /// for each event inside the event loop, and return the index of the file
    /// to save the event into (starting from zero for the first file on the
    /// command line).  The second argument provides the number of available
    /// output files.  If the event should not be saved, this should return a
    /// negative value.  In addition, if an index greater than "outputFiles"
    /// is returned the event will not be saved.  If the remainder of the
    /// current file should be skipped, this should through the
    /// ENextEventLoopFile exception.  If the daughter class defines this
    /// method it should not define the operator ().  The index of the output
    /// file is determined by the order on the command line.
    virtual int Process(TND280Event& event, int outputFiles);

    /// Called after the arguments are processes, and before the first event
    /// is read.  Any output files are open before Initialize is called, so
    /// histograms created in this method are saved.  All histograms being
    /// saved to the output file should be created in the Initialize method.
    /// The input file has not been opened before Initialized is called so you
    /// don't have access to the input data.  Extra (non-event) information in
    /// the input file can be accessed using the BeginFile method which is
    /// called after the input file is open, but before the first event is
    /// read.
    ///
    /// \note You cannot access the geometry in this method since it will not
    /// be correctly initialized.  Accessing the geometry *WILL* result in
    /// getting an incorrect geometry loaded.
    virtual void Initialize(void);

    /// Called before the first event of a file is read, but you should prefer
    /// Initialize() for general initialization such as creating histograms
    /// that will be saved in the output file.  This method will be called
    /// once for each input file and should be reserved for initialization
    /// that requires reading information from the input file.  This takes a
    /// pointer to the input file which must then be dynamic cast to the
    /// correct type.
    ///
    /// \note You should access the geometry in this method since it will not
    /// be correctly initialized.  Accessing the geometry *WILL* result in
    /// getting an incorrect geometry loaded.  Loading the correct geometry
    /// requires looking at the event header information to determine the
    /// detector configuration.
    ///
    /// \note C++ trivia: When determining which method to call,
    /// "TVInputFile*const" and "TVInputFile*" are equivalent so a method may
    /// be declared as one and defined as the other (but it's bad form).  If
    /// used in the definition (i.e. in the actual code) then
    /// "TVInputFile*const" prevents the pointer from being changed in the
    /// body.  Therefore, the way to read this declaration is that "const"
    /// documents that the method *must* not change the value of the pointer.
    virtual void BeginFile(TVInputFile *const);

    /// Called after the last event of a file is read, but you should prefer
    /// Finalize() for general finalization.  This method will be called once
    /// for each input file.  This takes a pointer to the input file
    /// which must then be dynamic cast to the correct type.
    ///
    /// \note C++ trivia: When determining which method to call,
    /// "TVInputFile*const" and "TVInputFile*" are equivalent so a method may
    /// be declared as one and defined as the other (but it's bad form).  If
    /// used in the definition (i.e. in the actual code) then
    /// "TVInputFile*const" prevents the pointer from being changed in the
    /// body.  Therefore, the way to read this declaration is that "const"
    /// documents that the method *must* not change the value of the pointer.
    virtual void EndFile(TVInputFile *const);

    /// Method has been deprecated and will be removed after v8r9.
    /// User code should prefer the Finalize(CP::TND280Output*const) method.
    /// This is called before a valid output file is being closed, but only if
    /// the Finalize(CP::TND280Output*const) method is undefined in the user
    /// code, and an output file is open.  If those conditions are met, then
    /// it will be called once per open file.  If you want to have a finalized
    /// method that is called even if there isn't an output file use the
    /// Finalize(CP::TND280Output * const file) version.
    virtual void Finalize(TND280Output& file) METHOD_DEPRECATED;

    /// Called after the last event has been processed, but before any open
    /// output files are closed.  This is called at least once, and will be
    /// called for each open file.  If there aren't any open output files, the
    /// TND280Output pointer will be NULL.
    ///
    /// \note C++ trivia: When determining which method to call,
    /// "TVInputFile*const" and "TVInputFile*" are equivalent so a method may
    /// be declared as one and defined as the other (but it's bad form).  If
    /// used in the definition (i.e. in the actual code) then
    /// "TVInputFile*const" prevents the pointer from being changed in the
    /// body.  Therefore, the way to read this declaration is that "const"
    /// documents that the method *must* not change the value of the pointer.
    virtual void Finalize(TND280Output * const file);

    /// Called when there is a usage error.  This code should print a usage
    /// message and then return. 
    virtual void Usage(void);

    /// Set an option and return true if it is valid.  This is called by the
    /// event loop command line argument processing code for each "-O
    /// [name]=[value]" option found on the command line.  If the command line
    /// has "-O [name]" without a value, then the value string will be equal
    /// to "".
    /// 
    /// The return value is used to flag errors during option handling.  If
    /// the options are valid, then SetOption should return true to indicate
    /// success.  If there is a problem processing the options, then SetOption
    /// should return false.  If this returns false, then the event loop will
    /// print the Usage message and exit with a non zero value (i.e. indicate
    /// failure).
    virtual bool SetOption(std::string option,std::string value="");

};
#endif
