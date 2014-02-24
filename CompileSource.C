CompileSource ()
{
  TString currentDir(gSystem->pwd());
  TString incDir("include");
  TString srcDir("src");
  TString buildDir("lib");
  TString incSuffix("h");
  TString srcSuffix("cxx");
  TString objSuffix("o");
  TString includePathString;
  TString srcPathString;
  TString buildPathString;
  TString includePathFlag;
  TString includeListString;
  TString linkedLibFlag;
  TString sourceListString;
  TString objectListString;
  TString libraryName("libHAL.so");
  TString linkdefFile("HAL_LinkDef.h");
  TString buildOS(gSystem->GetBuildArch());
  TString linkingInstruction(" -Wl,");
  TString runCintCommand("rootcint -f $BuildDir/HAL_dict.cxx -c -p $IncludePath ");
  TString makeLibCommands(gSystem->GetMakeSharedLib());
  TString runCintResult;
  TString makeLibResult;
  TList *files;
  Bool_t hasPython = (gSystem->GetFromPipe("root-config --has-python") == "yes") ? kTRUE: KFALSE;

  // Create full paths
  srcPathString = srcDir;
  srcPathString = gSystem->PrependPathName(currentDir.Data(), srcPathString);
  includePathString = incDir;
  includePathString = gSystem->PrependPathName(currentDir.Data(), includePathString);
  includePathFlag = includePathString;
  includePathFlag.Prepend("-I");
  linkedLibFlag = gSystem->GetFromPipe("root-config --glibs");
  if (hasPython) {
    linkedLibFlag.Append(" ");
    linkedLibFlag.Append(gSystem->GetFromPipe("python-config --ldflags"));
  }
  buildPathString = buildDir;
  buildPathString = gSystem->PrependPathName(currentDir.Data(), buildPathString);

  // Determine the linking instructions
  if (buildOS.Contains("macosx")) {
    linkingInstruction.Append("-install_name,@rpath/");
    linkingInstruction.Append(libraryName.Data());
  }
  else {
    linkingInstruction.Append("-soname,");
    linkingInstruction.Append(libraryName.Data());
  }

  // Set up environment for compiling and linking libraries
  if (hasPython) {
    includePathFlag.Append(" ");
    includePathFlag.Append(gSystem->GetFromPipe("python-config --includes"));
  }
  gSystem->AddIncludePath(includePathFlag.Data());
  gSystem->Setenv("IncludePath", gSystem->ExpandPathName(gSystem->GetIncludePath()));
  gSystem->Setenv("BuildDir", srcPathString.Data());
  gSystem->Setenv("LinkedLibs", linkedLibFlag.Data());
  gSystem->Setenv("SharedLib", libraryName.Data());
  gSystem->Setenv("Opt", "");

  // Create list of include files for cint
  includePathString = gSystem->PrependPathName(includePathString.Data(), "HAL");
  TSystemDirectory dir(includePathString.Data(), includePathString.Data());
  files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(incSuffix.Data()) && 
          !fname.Contains("LinkDef")) {
        if (!hasPython && fname.Contains("Python")) continue;
        else {
          includeListString = includeListString.Append(gSystem->PrependPathName(includePathString.Data(), fname));
          includeListString = includeListString.Append(" ");
        }
      }
    }
  }
  includeListString = includeListString.Append(gSystem->PrependPathName(includePathString.Data(), linkdefFile));
  runCintCommand = runCintCommand.Append(includeListString.Data());

  // Make dictionary
  std::cout << "Building HAL's CINT dictionary..." << std::endl;
  runCintResult = gSystem->GetFromPipe(gSystem->ExpandPathName(runCintCommand.Data()));
  if (runCintResult.CompareTo("")) {
    std::cout << "Something prevented rootcint from making dictionary smoothly." <<
      " I will try to make the shared library regardless." << std::endl;
    std::cout << runCintResult << std::endl;
  }
  else
    std::cout << "Success!" << std::endl;
  
  // Get the list of source (and object) files (files in the src dir and affixed with srcSufix)
  dir.SetDirectory(srcPathString.Data());
  files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(srcSuffix.Data())) {
        if (!hasPython && fname.Contains("Python")) continue;
        else {
          sourceListString = sourceListString.Append(gSystem->PrependPathName(srcPathString.Data(), fname));
          sourceListString = sourceListString.Append(" ");
          fname = file->GetName();
          objectListString = objectListString.Append(gSystem->PrependPathName(buildPathString.Data(), fname));
          objectListString = objectListString.Append(" ");
        }
      }
    }
  }
  // Change all cxx affixes to o
  objectListString = objectListString.ReplaceAll(srcSuffix.Data(), objSuffix.Data());

  // Finish setting up environment variables
  gSystem->Setenv("BuildDir", buildPathString.Data());
  gSystem->Setenv("SourceFiles", sourceListString.Data());
  gSystem->Setenv("ObjectFiles", objectListString.Data());
  objSuffix.Prepend(".");
  gSystem->SetObjExt(objSuffix.Data());
  // Make the library
  std::cout << "Creating the HAL shared library..." << std::endl;
  makeLibResult = gSystem->GetFromPipe(gSystem->ExpandPathName(makeLibCommands.Append(linkingInstruction.Data()).Data()));
  if (makeLibResult.CompareTo("")) {
    std::cout << "Compilation or linking may have ran into problems." << std::endl;
    std::cout << makeLibResult << std::endl;
  }
  else
    std::cout << "Success!" << std::endl;

  if (!runCintResult.CompareTo("") && !makeLibResult.CompareTo("")) {
    std::cout << "All source files compiled successfully!" << std::endl <<"Link to " << 
      buildPathString.Data() << " to access the library."  << std::endl;
  }
  else {
    std::cout << "There may have been a problem making the library." << std::endl;
  }
}
