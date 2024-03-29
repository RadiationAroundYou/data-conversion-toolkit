/// @file ListHandler.cc
/// @brief Implementation of the ListHandler class.

#include "ListHandler.h"

//
// The constructor.
//
ListHandler::ListHandler (Char_t * datasetpath) : m_dirPath(datasetpath) {
  cout << "* The dataset location specified is '" << m_dirPath << "'" << endl;
}

//
// OrderFilesPairMatch
//
std::vector<std::string> ListHandler::OrderFilesPairMatch(
  std::vector<std::string> listFrames,
  std::vector<std::string> listDSC)
{

  std::vector<std::string> orderedString;

  std::vector<std::string>::iterator itFrames = listFrames.begin();
  std::vector<std::string>::iterator itDSD    = listDSC.begin();

  TString tempName = "";
  TString tempName2 = "";

  for( ; itFrames != listFrames.end() ; itFrames++ ) {
    // first possible name
    tempName = ((TString)(*itFrames));
    tempName+=".dsc";

    // second possible name
    tempName2 = ((TString)(*itFrames));
    int size = tempName2.Sizeof();
    bool marktobreak = false;
    for (int i = size-1 ; i >= 0 ; i--) {
      if (tempName2[i] == '.') {
        marktobreak = true;
      }//end of check for the full stop.
      // remove first
      tempName2.Remove(i);
      // then break if necessary
      if (marktobreak) break;
    }//end of loop 
    
    tempName2+=".dsc";

    // Look for the corresponding .dsc file
    // that hopefully is not so far down in the list.
    // No need to optimize this now: this is fast enough.
    for (itDSD = listDSC.begin() ; itDSD != listDSC.end() ; itDSD++ ) {
      if ((TString)(*itDSD) == tempName || (TString)(*itDSD) == tempName2) {
        /* get the good filename in the list and erase the entry
           in  listDSC.  So it gets faster next time. */
        orderedString.push_back((*itDSD));
        listDSC.erase(itDSD);
        //std::cout << " !!! new size = " << listDSC.size() << std::endl;
        //std::cout << " !!! found --> " << (*itDSD) << std::endl;
        break;
      }//end of check
    }//end of loop over the DSC files(???).
  }//end of loop over the list of files(???).

  return orderedString;

}//end of OrderFilesPairMatch method.

//
// list_order
//
void ListHandler::list_order(vector<string> & f) {
  sort (f.begin(), f.end());
}

//
// getListToLoop (with idx files)
//
int ListHandler::getListToLoop (
  TString tempScratchDir,
  vector<string> & files,
  vector<string> & dscfiles,
  vector<string> & idxfiles)
{

  string tempfilename("lf_temp.txt");

  // If the user called the data files .dsc then the
  // dsc files will have extension .dsc.dsc
  // see if this happened
  DIR * dp;
  struct dirent *dirp;
  if ((dp  = opendir(m_dirPath.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << m_dirPath << endl;
    return errno;
  }

  string tempfn;
  bool doubledsc = false;
  while ((dirp = readdir(dp)) != NULL) {
    tempfn = string(dirp->d_name);
    if( tempfn.find(".dsc.dsc") != string::npos ) { // found .dsc.dsc
      doubledsc = true;
      break;
    }//end of check for a double dsc file ending(?!).
  }//end of while loop
  
  // rewind
  rewinddir(dp);

  // See if there are .idx files (only present in multiframe)
  bool idxpresent = false;
  while ((dirp = readdir(dp)) != NULL) {
    tempfn = string(dirp->d_name);
    if ( tempfn.find(".idx") != string::npos ) { // found .dsc.dsc
      idxpresent = true;
      break;
    }
  }

  // rewind
  rewinddir(dp);

  // Prepare to retreive all files
  string dsc_string = ".dsc";
  if(doubledsc) {
    cout << "[WARNING] the dsc files have extension .dsc.dsc" << endl;
    dsc_string = ".dsc.dsc";
  }

  // Find an get dsc files now
  while ((dirp = readdir(dp)) != NULL) {
    tempfn = string(dirp->d_name);

    if( tempfn.find( dsc_string.c_str() ) != string::npos ) { // found .dsc.dsc
      dscfiles.push_back( m_dirPath + "/" + tempfn );
    }
  }

  // These files are not in order. Attempt to order them.
  list_order(dscfiles);

  // Using the dsc list build the rest
  vector<string>::iterator i = dscfiles.begin();
  string tempstr;
  size_t pos;
  for ( ; i != dscfiles.end() ; i++) {
    tempstr = *i;
    // data file
    pos = tempstr.find_last_not_of( dsc_string.c_str() );
    //cout << tempstr << " --> " << pos << endl;
    if (doubledsc) { // particular case
      tempstr = tempstr.substr(0, pos+2);
      tempstr += ".dsc";
    } else {
      tempstr = tempstr.substr(0, pos+1);
    }
    files.push_back( tempstr );

    // idx file
    if(idxpresent) {
      tempstr = *i;
      // data file
      pos = tempstr.find_last_not_of( dsc_string.c_str() );
      
      if(doubledsc) {
        tempstr = tempstr.substr(0, pos+2);
        tempstr += ".dsc"; // particular case
      } else {
        tempstr = tempstr.substr(0, pos+1);
      }
      tempstr += ".idx";
      idxfiles.push_back( tempstr );
    }//enf of check for an idx file.
  }//end of loop over the dsc files.

  // Close the directory.
  closedir(dp);

  // Return the number of files found.
  return (int) files.size();

}//end of getListToLoop method.

//
// getListToLoop
//
std::vector<std::string> ListHandler::getListToLoop(
  TString tempScratchDir,
  Int_t typeF)
{

  TString filename = "";

  if (tempScratchDir.Length() > 0) {
    filename += tempScratchDir;
    filename += "/";
  }

  if (typeF==FRAME_FILES) filename += "listOfFiles.txt";
  if (typeF==DSC_FILES)   filename += "listOfFiles.dsc.txt";
  if (typeF==IDX_FILES)   filename += "listOfFiles.idx.txt";

  // Command to erase old listing files
  std::string command0 = "/bin/bash -c 'rm -f "; // listOfFiles.txt"   "'";
  command0 += filename.Data();
  command0 += "'";

  std::string command02 = "/bin/bash -c 'rm -f ";
  command02 += filename.Data();
  command02 += "'";

  std::string command03 = "/bin/bash -c 'rm -f ";
  command03 += filename.Data();
  command03 += "'";

  // Commands to find the number of related files
  std::string commandn = "/bin/bash -c \"ls ";
  std::string commandn2 = "/bin/bash -c 'ls ";
  std::string commandn3 = "/bin/bash -c 'ls ";

  // Commands to find the list of related files
  std::string command1 = "/bin/bash -c 'for a in ";
  std::string command2 = "/bin/bash -c 'for a in ";
  std::string command3 = "/bin/bash -c 'for a in ";

  // Number of frame files, i.e. not dsc or idx
  commandn += "'" + m_dirPath + "'" + " | awk '{ print !(/.dsc/||/.idx/) }' | grep 1 | wc -l >> "; //listOfFiles.txt'";
  commandn += filename.Data();
  commandn += " \"";

  commandn2 += "\"" + m_dirPath + "\"" + " | grep \".dsc\" | wc -l >> "; //listOfFiles.dsc.txt'";
  commandn2 += filename.Data();
  commandn2 += "'";

  commandn3 += "\"" + m_dirPath + "\"" + " | grep \".idx\" | wc -l >> "; //listOfFiles.idx.txt'";
  commandn3 += filename.Data();
  commandn3 += "'";

  // Get the files that are not dsc or idx, sometimes frame data comes
  // Without extension (.txt), or can be any other
  command1 += "\"" + m_dirPath + "\"" + "/* ; do [[ $a == *.dsc || $a == *.idx ]] || echo $a >> "; // listOfFiles.txt ; done'";
  command1 += filename.Data();
  command1 += " ; done'";

  // Get the files that are dsc
  command2 += "\"" + m_dirPath + "\"" + "/* ; do [[ $a == *.dsc ]] && echo $a >> "; // listOfFiles.dsc.txt ; done'";
  command2 += filename.Data();
  command2 += " ; done'";

  // Get the files that are idx
  command3 += "\"" + m_dirPath + "\"" + "/* ; do [[ $a == *.idx ]] && echo $a >> "; // listOfFiles.dsc.txt ; done'";
  command3 += filename.Data();
  command3 += " ; done'";


  // Erase the old files.
  if (typeF==FRAME_FILES) system(command0.c_str());
  if (typeF==DSC_FILES)   system(command02.c_str());
  if (typeF==IDX_FILES)   system(command03.c_str());

  if (typeF==FRAME_FILES) system(commandn.c_str());
  if (typeF==DSC_FILES)   system(commandn2.c_str());
  if (typeF==IDX_FILES)   system(commandn3.c_str());

  int rcm = 0;
  //rcm2 = 0, rcm3 = 0;
  if (typeF==FRAME_FILES)  rcm = system(command1.c_str());
  //if (typeF==DSC_FILES) rcm2 = system(command2.c_str());
  //if (typeF==IDX_FILES) rcm3 = system(command3.c_str());

  bool throwerror = false;
  std::vector<std::string> listOfFiles; // list to return

  if (typeF==FRAME_FILES && rcm != 0) {
    throwerror = true;
  }
  //else if (typeF==DSC_FILES && rcm2 != 0){
  //throwerror = true;
  //}
  //std::cout << command2 << " : " << rcm << "," << rcm2 << std::endl;

  if (throwerror) {
    cout
      << "ERROR: * Error when listing files in " << m_dirPath << "'"   << endl
      << "ERROR: * Please READ all the bash errors above."             << endl
      << "ERROR: * Other possible reasons: "                           << endl
      << "ERROR: *--> Temporary files need to be written."             << endl
      << "ERROR: *    Do you have 'w' permissions in this directory? " << endl
      << "ERROR: *    Use the third parameter to specify a scratch"    << endl
      << "ERROR: *    directory to write these to."                    << endl;
    return listOfFiles; // empty
  }

  fstream filestrList;
  filestrList.open(filename, fstream::in);

  std::string lineTemp;
  Int_t cntrInFile = 0;
  Int_t cntrInActualFiles = 0;
  Int_t nFiles = 0;

  while (filestrList.good()) {

    filestrList >> lineTemp;

    if (cntrInFile == 0) {
      nFiles = atoi(lineTemp.c_str());
    }
    if (cntrInFile > 0 && cntrInFile <= nFiles) {
      //std::cout << lineTemp << std::endl;
      listOfFiles.push_back(lineTemp);
      cntrInActualFiles++;
    }
    cntrInFile++;
  }

  // If there is a mismatch, exit the program.
  if (nFiles != cntrInActualFiles) {
    cout 
      << "ERROR: * There should be " << nFiles << " frame (and .dsc) files "
      << "in this directory. "                                   << endl
      << "ERROR: Only " << cntrInFile << " were found. Exiting." << endl;
    exit(1);
  }

  return listOfFiles;

}//end of getListToLoop method.
