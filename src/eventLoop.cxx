#include "eventLoop.hxx"

#include "ECore.hxx"

#include "TEvent.hxx"
#include "TEventContext.hxx"
#include "TRootInput.hxx"
#include "TRootOutput.hxx"
#include "TManager.hxx"
#include "THandleHack.hxx"
#include "TCaptLog.hxx"
#include "TMemoryUsage.hxx"
#include "TRuntimeParameters.hxx"
#include "TInputManager.hxx"

#include <iostream>
#include <sstream>
#include <limits>
#include <memory>
#include <unistd.h>
#include <csignal>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

#include <TROOT.h>
#include <TObjString.h>

namespace {
    void eventLoopUsage(std::string programName, 
                             CP::TEventLoopFunction& userCode,
                             int readCount) {
        std::cout << "usage: " << programName 
                  << " [options] [input-file] ..." 
                  << std::endl
                  << std::endl;
        
        std::cout << "Options:" << std::endl;
        
        std::cout << "    -o <file>         Set the name of an output file."
                  << std::endl
                  << "                        Multiple output files can"
                  << " be provided."
                  << std::endl;
        
        std::cout << "    -a                Read all events";
        if (readCount<1) std::cout << " [Default]";
        std::cout << std::endl;
        
        std::cout << "    -c <file>         Set the logging config file name"
                  << std::endl;
        
        std::cout << "    -d                Increase the debug level"
                  << std::endl;

        std::cout << "    -D <name>=[error,severe,warn,debug,trace]"
                  << std::endl
                  << "                      Change the named debug level"
                  << std::endl;

        std::cout << "    -f <event>        Look for an event number"
                  << std::endl;

        std::cout << "    -f <run>,<event>  Look for a run and event number"
                  << std::endl;

        std::cout << "    -G <file>         Force a geometry file"
                  << std::endl;
        std::cout << "         'CAPTAIN' for default CAPTAIN geometry"
                  << std::endl
                  << "         'miniCAPTAIN' for default miniCAPTAIN geometry"
                  << std::endl;
        
        std::cout << "    -g                Don't save geometry in output"
                  << std::endl;
        
        std::cout << "    -H                Debug THandle (slow)"
                  << std::endl;
        

        std::cout << "    -n <cnt>          Only read <cnt> events";
        if (readCount>0) std::cout << "  [Default: " << readCount << "]";
        std::cout << std::endl;
        
        std::cout << "    -q                Decrease the verbosity"
                  << std::endl;

        std::cout << "    -r <override>     Override parameter \"name:value\""
                  << std::endl;
        std::cout << "                      Example: -r "
                  << "\"elecSim.simple.drift.life:0.1 ms\""
                  << std::endl;

        std::cout << "    -R <override>     Name of an run-time parameter "
                  << "override file"
                  << std::endl;

        std::cout << "    -s <cnt>          Skip <cnt> events"
                  << std::endl;
        
        std::cout << "    -t <type>         Set the type of the input file"
                  << std::endl;

        for (int i = 0; i<CP::TManager::Get().Input().BuilderCount(); ++i) {
            std::string name
                = CP::TManager::Get().Input().Builder(i).GetName();
            std::string doc 
                = CP::TManager::Get().Input().Builder(i).GetDocumentation();
            std::cout << "        " << name
                      << " : " << doc << std::endl;
        }

        std::cout << "    -u                Log the memory and CPU usage"
                  << std::endl;

        std::cout << "    -v                Increase the verbosity"
                  << std::endl;
        
        std::cout << "    -V <name>=[quiet,log,info,verbose]"
                  << std::endl
                  << "                      Change the named log level"
                  << std::endl;

        std::cout << "    -O <opt>[=<val>]  Set an option for the user code"
                  << std::endl;
        
        std::cout << std::endl;
        
        userCode.Usage();
        std::exit(1);
    }
};

int CP::eventLoop(int argc, char** argv,
                       CP::TEventLoopFunction& userCode, 
                       int defaultReadCount) {
    if (defaultReadCount<0) defaultReadCount = 0;
    int readCount = defaultReadCount;
    int skipCount = 0;
    int debugLevel = 0;
    std::map<std::string, CP::TCaptLog::ErrorPriority> namedDebugLevel;
    int logLevel = -1; // Will choose default logging level...
    std::map<std::string, CP::TCaptLog::LogPriority> namedLogLevel;
    std::string programName = argv[0];
    char *configName = NULL;
    std::vector<std::string> outputNames;
    std::vector<TRootOutput*> outputFiles;
    bool preventSavedGeometry = false;
    std::string geometryFile = "";
    int targetRun = -1;
    int targetEvent = -1;
    int exitStatus = 0;
    TMemoryUsage memoryUsage;

    // If this is not zero, then only accept triggers matched in this mask.
    signal(SIGSEGV, SIG_DFL);

    std::string fileType = "root";

    // Process the options.
    for (;;) {
        int c = getopt(argc, argv, "ac:dD:f:G:gHn:o:O:qr:R:s:t:uvV:");
        if (c<0) break;
        switch (c) {
        case 'a':
        {
            // readcount of zero means read the entire file.
            readCount = 0;
            break;
        }
        case 'c':
        {
            configName = strdup(optarg);
            break;
        }
        case 'd':
        {
            // increase the debugging level.
            ++debugLevel;
            break;
        }
        case 'D':
        {
            // Set the debug level for a named trace.
            std::string arg(optarg);
            std::size_t sep = arg.find("=");
            if (sep != std::string::npos) {
                std::string name = arg.substr(0,sep);
                std::string levelName = arg.substr(sep+1);
                switch (levelName[0]) {
                case 'e': case 'E':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::ErrorLevel;
                    break;
                case 's': case 'S':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::SevereLevel;
                    break;
                case 'w': case 'W':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::WarnLevel;
                    break;
                case 'd': case 'D':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::DebugLevel;
                    break;
                case 't': case 'T':
                    namedDebugLevel[name.c_str()] = CP::TCaptLog::TraceLevel;
                    break;
                default:
                    eventLoopUsage(programName,userCode,defaultReadCount);
                }
            }
            break;
        }
        case 'f':
        {
            // Look for a particular run and event number in the file.
            std::string arg(optarg);
            if (arg.find(',')!=std::string::npos) {
                std::istringstream input(arg);
                char c;
                input >> targetRun;
                input >> c;
                input >> targetEvent;
            }
            else {
                std::istringstream input(arg);
                input >> targetEvent;
            }
            break;
        }   
        case 'G':
        {
            // Force a particular geometry file.
            geometryFile = optarg;
            break;
        }
        case 'g':
        {
            // prevent the geometry from being saved.
            preventSavedGeometry = true;
            break;
        }
        case 'H':
        {
            // Debug the handles.
            EnableHandleRegistry(true);
            break;
        }
        case 'n':
        {
            std::istringstream tmp(optarg);
            tmp >> readCount;
            break;
        }
        case 'o':
        {
            outputNames.push_back(optarg);
            break;
        }
        case 'O':
        { 
            std::string option = optarg;
            std::string value = "";
            // If the option is "<option>=<value>", split this into two
            // strings "<option>" and "<value>"
            std::string::size_type equals = option.find_first_of("=");
            if (equals != std::string::npos) {
                value = option.substr(equals+1,std::string::npos);
                option = option.substr(0,equals);
            }
            if (!userCode.SetOption(option,value)) {
                std::cerr << "ERROR: Illegal user option: " << option;
                if (value != "") std::cerr << "   Value: " << value;
                std::cerr << std::endl;
                eventLoopUsage(programName,userCode,defaultReadCount);
            }
            break;
        }
        case 'q':
        {
            // decrease the verbosity level.
            if (logLevel>0) --logLevel;
            else logLevel = 0;
            break;
        }
        case 'r':
        {
            std::string param(optarg);
            // If the param is "<param>:<value>", split this into two
            // strings "<param>" and "<value>"
            std::string::size_type sep = param.find_first_of(":");
            if (sep == std::string::npos) {
                std::cerr << "ERROR: Invalid override parameter: "
                          << "\"" << param << "\""
                          << std::endl;
                eventLoopUsage(programName,userCode,defaultReadCount);
            }
            std::string value = param.substr(sep+1,std::string::npos);
            param = param.substr(0,sep);
            CP::TRuntimeParameters::Get().SetOverrideParameter(param,value);
            break;
        }
        case 'R':
        {
            CP::TRuntimeParameters::Get().ReadParamOverrideFile(optarg);
            break;
        }
        case 's':
        {
            std::istringstream tmp(optarg);
            tmp >> skipCount;
            break;
        }
        case 't':
        {
            std::cout << "INFO:: Set file type to " << optarg << std::endl;
            fileType = optarg;
            break;
        }
        case 'u':
        {
          // Enable logging of memory usage
          memoryUsage.Enable();
          break;
        }
        case 'v':
        {
            // increase the verbosity level.
            if (logLevel>0) ++logLevel;
            else logLevel = 2;
            break;
        }
        case 'V':
        {
            // Set the debug level for a named trace.
            std::string arg(optarg);
            std::size_t comma = arg.find(",");
            if (comma != std::string::npos) {
                std::cerr << "ERROR: Only one verbosity setting per option: "
                          << "\"" << arg << "\""
                          << std::endl;
                eventLoopUsage(programName,userCode,defaultReadCount);
            }
            std::size_t sep = arg.find("=");
            if (sep != std::string::npos) {
                std::string name = arg.substr(0,sep);
                std::string levelName = arg.substr(sep+1);
                switch (levelName[0]) {
                case 'q': case 'Q':
                    namedLogLevel[name.c_str()] = CP::TCaptLog::QuietLevel;
                    break;
                case 'l': case 'L':
                    namedLogLevel[name.c_str()] = CP::TCaptLog::LogLevel;
                    break;
                case 'i': case 'I':
                    namedLogLevel[name.c_str()] = CP::TCaptLog::InfoLevel;
                    break;
                case 'v': case 'V':
                    namedLogLevel[name.c_str()] = CP::TCaptLog::VerboseLevel;
                    break;
                default:
                    eventLoopUsage(programName,userCode,defaultReadCount);
                }
            }
            break;
        }
        default:
            eventLoopUsage(programName,userCode,defaultReadCount);
        }
    }
    
    // Set up the logging code.
    TCaptLog::Configure(configName);
    
    if (logLevel == 0) {
        TCaptLog::SetLogLevel(TCaptLog::QuietLevel);
    }
    else if (logLevel == 1) {
        TCaptLog::SetLogLevel(TCaptLog::LogLevel);
        CaptLog("Set log level to LogLevel");
    }
    else if (logLevel == 2) {
        TCaptLog::SetLogLevel(TCaptLog::InfoLevel);
        CaptInfo("Set log level to InfoLevel");
    }
    else if (logLevel >= 3) {
        TCaptLog::SetLogLevel(TCaptLog::VerboseLevel);
        CaptVerbose("Set log level to VerboseLevel");
    }
    
    for (std::map<std::string,CP::TCaptLog::LogPriority>::iterator i 
             = namedLogLevel.begin();
         i != namedLogLevel.end();
         ++i) {
        CP::TCaptLog::SetLogLevel(i->first.c_str(), i->second);
    }
         
    if (debugLevel == 1) {
        TCaptLog::SetDebugLevel(TCaptLog::WarnLevel);
        CaptWarn("Set debug level to WarnLevel");
    }
    else if (debugLevel == 2) {
        TCaptLog::SetDebugLevel(TCaptLog::DebugLevel);
        CaptDebug("Set debug level to DebugLevel");
    }
    else if (debugLevel >= 2) {
        TCaptLog::SetDebugLevel(TCaptLog::TraceLevel);
        CaptTrace("Set debug level to TraceLevel");
    }

    for (std::map<std::string,CP::TCaptLog::ErrorPriority>::iterator i 
             = namedDebugLevel.begin();
         i != namedDebugLevel.end();
         ++i) {
        CP::TCaptLog::SetDebugLevel(i->first.c_str(), i->second);
    }
         
    if (!outputNames.empty()) {
        for (std::vector<std::string>::iterator n = outputNames.begin();
             n != outputNames.end();
             ++n) {
            TRootOutput* output = new TRootOutput((*n).c_str(),"NEW");
            if (!output->IsOpen()) {
                std::cerr << "ERROR: Output file not open" << std::endl;
                exit(2);
            }
            // Save the command line to the TFile.
            std::string command;
            for (int i=0; i<argc; ++i) {
                if (i > 0) command += " ";
                command += argv[i];
            }
            TObjString commandLine(command.c_str());
            output->WriteObject(&commandLine,"commandLine");
            outputFiles.push_back(output);
        }
    }
    
    // Check that there is an input file to open.
    if (argc<=optind) {
        std::cerr << "ERROR: No input file" << std::endl << std::endl;
        eventLoopUsage(programName,userCode,defaultReadCount);
    }
    
    if (!outputFiles.empty()) outputFiles.front()->cd();
    userCode.Initialize();
    
    // If readcount is zero, that means read the entire file.
    if (readCount<1) readCount = std::numeric_limits<int>::max();
    
    if (geometryFile == "CAPTAIN") {
        const char* geomFile = gSystem->Getenv("CAPTAINGEOMETRY");
        if (geomFile) geometryFile = geomFile;
    }

    if (geometryFile == "miniCAPTAIN"
        || geometryFile == "mCAPTAIN") {
        const char* geomFile = gSystem->Getenv("MINICAPTAINGEOMETRY");
        if (geomFile) geometryFile = geomFile;
    }

    if (geometryFile != "") {
        TManager::Get().SetGeometryOverride(geometryFile);
    }

    int totalRead = 0;
    int totalWritten = 0;
    double nextOutput = 10;
    while (optind<argc) {
        // Check to see if we need to read more events.
        if (readCount<=totalRead) break;
        
        // Open input the file
        std::string fileName = argv[optind++];
        CP::TEventContext firstContext;
        CP::TEventContext lastContext;
        int fileRead = 0;
        int fileWritten = 0;
        try {
            std::unique_ptr<CP::TVInputFile> input;
            try {
                input.reset(
                    CP::TManager::Get().Input().Builder(fileType.c_str()).Open(
                        fileName.c_str()));
            }
            catch (std::exception& ex) {
                CaptError("ERROR: Caught exception: " 
                          << ex.what());
                exit(1);
            }
            catch (...) {
                CaptError("ERROR: Unknown exception for \"" 
                          << fileType << "\"");
                exit(1);
            }
            
            if (!input->IsOpen()) {
                CaptError("ERROR: File not found " << fileName);
                continue;
            }
            
            if (!outputFiles.empty()) {
                // Save the input file name input the output file.
                for (std::vector<CP::TRootOutput*>::iterator f
                         = outputFiles.begin();
                     f != outputFiles.end(); ++f) {
                    std::unique_ptr<char>
                        resolvedPath(realpath(fileName.c_str(),NULL));
                    TObjString inputNameString(resolvedPath.get());
                    (*f)->WriteObject(&inputNameString,"inputFile");
                }
                // Make sure we are on the first output file so that any
                // created histograms go to a predictable place.
                outputFiles.front()->cd();
            }

            userCode.BeginFile(input.get());
            
            std::unique_ptr<TEvent> event(input->FirstEvent());

            // Position to the first event in the file to read.  This might
            // leave the file positioned at the end of file.
            while (skipCount > 0) {
                int startPosition = input->GetPosition();
                event.reset(input->NextEvent(skipCount-1));
                int changedPosition = input->GetPosition() - startPosition;
                if (changedPosition>0) skipCount -= changedPosition;
                else --skipCount;
            }

            // Get the first event context that will be processed.
            firstContext = event->GetContext();
            
            // Process the events in the file.
            for (;!input->EndOfFile();event.reset(input->NextEvent())) {

                if (targetRun>=0 
                    && event->GetRunId()< (UInt_t) targetRun) continue;
                else targetRun = -1;

                if (targetEvent>=0 
                    && event->GetEventId()<(UInt_t) targetEvent) continue;
                else targetEvent = -1;

                ++totalRead;
                ++fileRead;
                
                // Save the last event context that was read.
                lastContext = event->GetContext();
                memoryUsage.LogMemory();
                
                int saveEvent = -1;
                try {
                    if (!outputFiles.empty()) outputFiles.front()->cd();
                    saveEvent = userCode.Process(*event,outputFiles.size());
                }
                catch (ENextEventLoopFile& ex) {
                    break;
                }

                if (!preventSavedGeometry) {
                    int geomFile = std::max(0,saveEvent);
                    // Check if the geometry should be saved.
                    if (0 <= saveEvent 
                        && saveEvent < (int) outputFiles.size()) {
                        try {
                            outputFiles[geomFile]
                                ->WriteGeometry(TManager::Get().Geometry());
                        }
                        catch (ENoGeometry) {
                            CaptSevere("Geometry not saved in output");
                        }
                        catch (EManager) {
                            CaptSevere("Geometry not saved in output");
                        }
                    }
                }
                
                if (0 <= saveEvent && saveEvent < (int) outputFiles.size()) {
                    outputFiles[saveEvent]->WriteEvent(*event);
                    ++totalWritten;
                    ++fileWritten;
                }
                
                event.reset(NULL);
                if (!CleanHandleRegistry()) {
                    DumpHandleRegistry();
                    CaptError("WARNING: Memory Leak in "
                              << " File(event,run): " << fileName 
                              << ":(" 
                              << lastContext << ")");
                }
                
                if (totalRead>(nextOutput-0.5)) {
                    nextOutput *= std::sqrt(10);
                    if (nextOutput - totalRead > 1000) {
                        nextOutput = totalRead + 1000;
                    }
                    CaptLog("Events Processed: " << totalRead);
                }

                // Check to see if we have read enough events.
                if (readCount<=totalRead) break;
            }
            
            if (!CleanHandleRegistry()) {
                DumpHandleRegistry();
                CaptError("WARNING: Memory Leak after finishing "
                          << fileName);
            }
            
            if (!outputFiles.empty()) outputFiles.front()->cd();
            userCode.EndFile(input.get());
            input->CloseFile();
        }
        catch (std::exception& except) {
            CaptError("ERROR: Uncaught exception in " 
                      << fileName);
            CaptError("    What: " << except.what());
            if (!CleanHandleRegistry()) {
                DumpHandleRegistry();
                CaptError("WARNING: Memory Leak after finishing "
                          << fileName);
            }
            exitStatus = 3;
        }
        catch (...) {
            // Don't crash on an error, but try to go to the next file.
            CaptError("ERROR: Uncaught exception in " 
                      << fileName);
            if (!CleanHandleRegistry()) {
                DumpHandleRegistry();
                CaptError("WARNING: Memory Leak after finishing "
                          << fileName);
            }
            exitStatus = 4;
        }
        std::cout << "Event_File_Summary: "
                  << "\"" << fileName << "\""
                  << "," << firstContext.GetRun()
                  << "," << firstContext.GetSubRun()
                  << "," << firstContext.GetEvent()
                  << "," << firstContext.GetTimeStampString()
                  << "," << fileRead
                  << "," << fileWritten
                  << std::endl;
        if (exitStatus != 0) break;
    }
    
    if (!outputFiles.empty()) {
        for (std::vector<CP::TRootOutput*>::iterator file 
                 = outputFiles.begin();
             file != outputFiles.end(); 
             ++ file) {
            userCode.Finalize(*file);
            memoryUsage.Write(*file);
        }
    }
    else {
        userCode.Finalize((CP::TRootOutput*) NULL);
        memoryUsage.Write((CP::TRootOutput*) NULL);
    }
    
    std::cout << "Total Events Read: " << totalRead << std::endl;
    if (!outputFiles.empty()) {
        std::cout << "Total Events Written: " << totalWritten << std::endl;
        for (std::vector<TRootOutput*>::iterator file = outputFiles.begin();
             file != outputFiles.end();
             ++file) {
            (*file)->Close();
        }
    }

    if (exitStatus > 0) {
        CaptError("Terminating with non-zero exit status.");
        std::exit(exitStatus);
    }

    return 0;
}

