CompileSource ()
{
  TString currentDir(gSystem->pwd());

  TString incDir("include");
  TString srcDir("src");
  TString srcSuffix("cxx");
  TString libPrefix("lib");
  TString buildDir("lib");
  TString buildOptions("Okf");
  TString includePathString;
  TString srcPathString;
  TString buildPathString;
  TObjArray sourceList;
  TList *files;
  Int_t result = 0;

  srcPathString = srcDir;
  srcPathString = gSystem->PrependPathName(currentDir.Data(), srcPathString);
  includePathString = incDir;
  includePathString = gSystem->PrependPathName(currentDir.Data(), includePathString);
  includePathString.Prepend("-I");
  buildPathString = buildDir;
  buildPathString = gSystem->PrependPathName(currentDir.Data(), buildPathString);

  gSystem->AddIncludePath(includePathString.Data());
  gSystem->SetBuildDir(buildPathString.Data(), kTRUE);

  // Get the list of source files (files in the src dir and affixed with srcSufix)
  TSystemDirectory dir(srcPathString.Data(), srcPathString.Data());
  files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(srcSuffix.Data()))
        sourceList.Add(new TObjString(fname.Remove(fname.Last('.'), fname.Length()).Data()));
    }
  }

  // Loop through source files and compile in the directory given in buildPathString
  for (Int_t i = 0; i <= sourceList.GetEntries(); ++i) {
    TString libName;
    if (i < sourceList.GetEntries())  {
      libName = ((TObjString*) sourceList[i])->GetString();
      if (libName.CompareTo("Analysis") == 0) continue;
    }
    else
      libName = "Analysis";
    TString fullSrcName;
    TString fullLibName;

    fullSrcName = libName;
    fullSrcName.Append(".").Append(srcSuffix.Data());
    fullSrcName = gSystem->PrependPathName(srcDir.Data(), fullSrcName);
    fullLibName = libPrefix;
    fullLibName.Append(libName.Data());

    result += gSystem->CompileMacro(fullSrcName.Data(), buildOptions.Data(), fullLibName.Data());
  }

  if (result == sourceList.GetEntries()) {
    std::cout << "All source files compiled successfully! Link to " << 
      buildPathString.Data() << " to access the libraries."  << std::endl;
  }
  else {
    std::cout << "Unable to compile all the source files." << std::endl;
  }
}
