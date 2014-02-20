CompileAnalysis ()
{
  TString HAL_Dir("/Users/jhetherly/src/root_HAL");
  TString HAL_LibDir(gSystem->PrependPathName(HAL_Dir.Data(), "lib"));
  TString HAL_IncDir(gSystem->PrependPathName(HAL_Dir.Data(), "include"));
  TString currentDir(gSystem->pwd());
  TString incDir("include");
  TString srcDir("src");
  TString buildDir("bin");
  TString incSuffix("h");
  TString srcSuffix("cxx");
  TString objSuffix("o");
  TString exeName("MyAnalysis");
  TString linkPathFlag;
  TString includePathString;
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
  TList *files;

  // Create full paths
  srcPathString = srcDir;
  srcPathString = gSystem->PrependPathName(currentDir.Data(), srcPathString);
  includePathString = incDir;
  includePathString = gSystem->PrependPathName(currentDir.Data(), includePathString);
  includePathFlag = includePathString;
  includePathFlag.Prepend("-I");
  buildPathString = buildDir;
  buildPathString = gSystem->PrependPathName(currentDir.Data(), buildPathString);
  linkPathFlag = " -L";
  linkPathFlag.Append(HAL_LibDir.Data());
  linkPathFlag.Append(" -lHAL");

  // Set up environment for compiling and linking libraries
  gSystem->AddIncludePath(includePathFlag.Data());
  gSystem->Setenv("IncludePath", gSystem->ExpandPathName(gSystem->GetIncludePath()));
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
  if (makeExeResult.CompareTo("")) {
    std::cout << "Compilation or linking may have ran into problems." << std::endl;
    std::cout << makeExeResult << std::endl;
  }
  else
    std::cout << "Success!" << std::endl;

}
