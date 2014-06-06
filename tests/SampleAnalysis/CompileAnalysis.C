/*
 * This macro is designed to aid in compiling and linking the
 * user's algorithms and analysis code with the HAL Analysis
 * framework. If the file structure of the user's code follows
 * the standard format (explained below), then all the user
 * needs to provide is:
 *  - the directory of the HAL framework
 *  - the name of the analysis source file
 * 
 * The standard file structure:
 *  UserRoot - CompileAnalysis.C
 *           - bin
 *           - analyses
 *           - algorithms - include
 *                        - src
 *
 * Logically the user's code should be separated into one or
 * more algorithm files which may or may not have separate
 * declarations (header files) and definitions
 * (source files). By default this macro looks in 
 * UserRoot/algorithms/include and UserRoot/algoirthms/src
 * for these files. Additionally the user must specify a
 * "driver" or "analysis" source file located in 
 * UserRoot/analyses by default. This file should have a 
 * "main" function and should act to set all the analysis
 * specific variables (TTree name, branch mappings, etc...).
 *
 * The C++ source files in UserRoot/analyses and 
 * UserRoot/algorithms/src should end in '.cxx' and the header
 * files in UserRoot/algorithms/include should end in '.h' by
 * default.
 *
 * The resulting executable should be found in UserRoot/bin if
 * everything was successful. The default name for the
 * executable is the "analysis file" minus the extention.
 *
 * Most of the parameters are customizable and listed in the 
 * arguement list below. If you need the default value for a
 * parameter, just input a "" as the arguement.
 *
 * */

void CompileAnalysis (TString HAL_dir,             // Directory of HAL framework
                      TString analysis_src,        // Analysis source file (include extension)
                      TString exe_name = "",       // Executable name
                      TString user_inc_flag = "",  // Additional user include flag(s)
                      TString user_lib_flag = "",  // Additional user library flag(s)
                      TString header_suffix = "",  // Header files suffix
                      TString source_suffix = "",  // Source files suffix
                      TString include_dir = "",    // Header files directory
                      TString source_dir = "",     // Source files directory
                      TString analyses_dir = "",   // Directory of analyses sources
                      TString output_dir = "",     // Directory to build executable
                      Bool_t debug = kFALSE)       // Add debug flags
{
  // HAL framework information
  TString HAL_Dir(HAL_dir);
  TString lib("lib");
  TString src("src");
  TString include("include");
  TString algo("algorithms");
  TString HAL_LibDir(gSystem->PrependPathName(HAL_Dir.Data(), lib));
  TString HAL_IncDir(gSystem->PrependPathName(HAL_Dir.Data(), include));
  lib = "lib";
  include = "include";
  // User specific information
  TString currentDir(gSystem->pwd());
  TString algoDir(gSystem->PrependPathName(currentDir, algo));
  TString incDir;
  if (include_dir.EqualTo(""))
    incDir = gSystem->PrependPathName(algoDir, include);
  else
    incDir = include_dir;
  TString srcDir;
  if (source_dir.EqualTo(""))
    srcDir = gSystem->PrependPathName(algoDir, src);
  else
    srcDir = source_dir;
  include = "include";
  src = "src";
  TString buildDir;
  if (output_dir.EqualTo(""))
    buildDir = "bin";
  else
    buildDir = output_dir;
  TString analysesDir;
  if (analyses_dir.EqualTo(""))
    analysesDir = "analyses";
  else 
    analysesDir = analyses_dir;
  TString incSuffix;
  if (header_suffix.EqualTo(""))
    incSuffix = "h";
  else
    incSuffix = header_suffix;
  TString srcSuffix;
  if (source_suffix.EqualTo(""))
    srcSuffix = "cxx";
  else
    srcSuffix = source_suffix;
  TString objSuffix("o");
  TString temp(analysis_src);
  TString exeName;
  if (exe_name.EqualTo(""))
    exeName = temp.Remove(temp.Last('.'), temp.Length());
  else
    exeName = exe_name;
  TString linkPathFlag;
  TString includePathString;
  TString analysesPathString;
  TString srcPathString;
  TString buildPathString;
  TString includePathFlag;
  TString includeListString;
  TString sourceListString;
  TString objectListString;
  TString buildOS(gSystem->GetBuildArch());
  TString linkedLibs(gSystem->GetFromPipe("root-config --glibs"));
  TString linkingInstruction(" -Wl,");
  TString makeExeCommands(gSystem->GetMakeExe());
  TString makeExeResult;
  Int_t versionNumber = gROOT->GetVersionInt();
  TList *files;

  // Create full paths
  analysesPathString = analysesDir;
  analysesPathString = gSystem->PrependPathName(currentDir.Data(), analysesPathString);
  srcPathString = srcDir;
  srcPathString = gSystem->PrependPathName(currentDir.Data(), srcPathString);
  includePathString = incDir;
  includePathFlag = includePathString;
  includePathFlag.Prepend("-I");
  HAL_IncDir.Prepend(" -I");
  includePathFlag.Append(HAL_IncDir.Data());
  if (user_inc_flag.CompareTo("")) // evaluates to false is user_inc_flag != ""
    includePathFlag.Prepend(" ").Prepend(user_inc_flag);
  buildPathString = buildDir;
  buildPathString = gSystem->PrependPathName(currentDir.Data(), buildPathString);
  linkPathFlag = " -L";
  linkPathFlag.Append(HAL_LibDir.Data());
  linkPathFlag.Append(" -lHAL");
  if (user_lib_flag.CompareTo(""))
    linkPathFlag.Append(" ").Append(user_lib_flag);

  // Set up environment for compiling and linking libraries
  gSystem->AddIncludePath(includePathFlag.Data());
  includePathFlag = gSystem->ExpandPathName(gSystem->GetIncludePath());
  if (debug) {
    TRegexp optFlag("-O[0-9] ");
    long optPosition = makeExeCommands.Index(optFlag);

    if (optPosition >= 0) 
      makeExeCommands.Remove(optPosition, 4);
    includePathFlag.Prepend("-g ");
  }
  gSystem->Setenv("IncludePath", includePathFlag);
  gSystem->Setenv("BuildDir", buildPathString.Data());
  gSystem->Setenv("LinkedLibs", linkedLibs.Append(linkPathFlag.Data()));
  gSystem->Setenv("ExeName", exeName.Data());

  // Get the list of source (and object) files (files in the src dir and affixed with srcSufix)
  TSystemDirectory dir(srcPathString.Data(), srcPathString.Data());
  dir.SetDirectory(srcPathString.Data());
  files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(srcSuffix.Data())) {
        sourceListString = sourceListString.Append(gSystem->PrependPathName(srcPathString.Data(), fname));
        sourceListString = sourceListString.Append(" ");
        fname = file->GetName();
        objectListString = objectListString.Append(gSystem->PrependPathName(buildPathString.Data(), fname));
        objectListString = objectListString.Append(" ");
      }
    }
  }
  // Add the analysis source file
  temp = analysis_src;
  sourceListString = sourceListString.Append(gSystem->PrependPathName(analysesPathString.Data(), temp));
  sourceListString = sourceListString.Append(" ");
  objectListString = objectListString.Append(gSystem->PrependPathName(buildPathString.Data(), analysis_src));
  objectListString = objectListString.Append(" ");

  // Change all cxx affixes to o
  objectListString = objectListString.ReplaceAll(srcSuffix.Data(), objSuffix.Data());

  // Determine the linking instructions
  linkingInstruction.Append("-rpath,");
  linkingInstruction.Append(HAL_LibDir.Data());

  // Finish setting up environment variables
  gSystem->Setenv("SourceFiles", sourceListString.Data());
  gSystem->Setenv("ObjectFiles", objectListString.Data());
  objSuffix.Prepend(".");
  gSystem->SetObjExt(objSuffix.Data());
  // Make the executable
  std::cout << "Creating your analysis executable..." << std::endl;
  makeExeResult = gSystem->GetFromPipe(gSystem->ExpandPathName(makeExeCommands.Append(linkingInstruction.Data()).Data()));
  //if (makeExeResult.CompareTo("")) {
  //  std::cout << "Compilation or linking may have ran into problems." << std::endl;
  //  std::cout << makeExeResult << std::endl;
  //}
  //else
  //  std::cout << "Success!" << std::endl;

}

