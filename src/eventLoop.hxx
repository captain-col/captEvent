#ifndef eventLoop_hxx_seen
#define eventLoop_hxx_seen

#include "TEventLoopFunction.hxx"

namespace CP {
/// The CP::eventLoop() function provides a basic event loop that will call
/// user code for each event.  It will print a summary of the event file after
/// the last event is read.  The summary is a single line with the prefix
/// "Event_File_Summary:" that is followed by comma separated values.  The
/// values are: 1) the run number of the first event; 2) the subrun number of
/// the first event, 3) the event id of the first event; 4) the date string
/// (in UTC) of the first event; 5) the number of events read from the input
/// file; and 6) the number of events written to the output file.
/// 
/// See CP::TEventLoopFunction for a base class that can be used with this
/// event loop.  The simplest possible event loop code is done like this:
///
/// \code
/// class TDumpEvent: public CP::TEventLoopFunction {
/// public:
///     bool operator () (CP::TEvent& event) {
///         // User Code (just returns).
///         return true; // "true" to save event to output
///     }
/// };
///
/// int main(int argc, char **argv) {
///     TDumpEvent userCode;
///     return CP::eventLoop(argc,argv,userCode);
/// }
/// \endcode
///
/// The default number of events to be processed can be specified as the
/// fourth argument to CP::eventLoop().  If the argument is not
/// specified, or is less than or equal to zero, then all events in the file
/// will be read.  See \ref eventLoop for more usage documentation.
///
/// The above main program results in a basic event loop that provides several
/// user options and capabilities.  The program options can be listed by
/// running the program without an arguments.  The most common options are:
///
/// \code
// usage: dump-event.exe [options] [input-file] ...

// Options:
//     -o <file>         Set the name of an output file.
//                       Multiple output files can be provided.
//     -a                Read all events
//     -c <file>         Set the logging config file name
//     -d                Increase the debug level
//     -D <name>=[error,severe,warn,debug,trace]
//                       Change the named debug level
//     -f <event>        Look for an event number
//     -f <run>,<event>  Look for a run and event number
//     -G <file>         Force a geometry file
//     -g                Don't save geometry in output
//     -H                Debug THandle (slow)
//     -n <cnt>          Only read <cnt> events  [Default: 1]
//     -q                Decrease the verbosity
//     -r <override>     Override a parameter "name:value"
//                          example: -r "elecSim.simple.drift.life:0.1 ms"
//     -R <override>     Name of an run-time parameter override file
//     -s <cnt>          Skip <cnt> events
//     -u                Log the memory and CPU usage
//     -v                Increase the verbosity
//     -V <name>=[quiet,log,info,verbose]
//                       Change the named log level
//     -O <opt>[=<val>]  Set an option for the user code
/// \endcode
///
/// \htmlonly
/// There is skeleton code available in 
/// <a href="../doc/exampleEventLoop.cxx">exampleEventLoop.cxx</a> that can
/// be copied to a local directory and modified.  If you use this code, do a
/// global search and replace on "UserLoop", changing it to some more useful
/// name.  That will help keep the doxygen documentation useful.
/// \endhtmlonly
    int eventLoop(int argc, char** argv, 
                        CP::TEventLoopFunction& userCode, 
                        int defaultReadCount=0);
};
 
#endif
