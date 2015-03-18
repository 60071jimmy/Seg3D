/*
 For more information, please see: http://software.sci.utah.edu

 The MIT License

 Copyright (c) 2013 Scientific Computing and Imaging Institute,
 University of Utah.


 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */

#ifndef CORE_UTILS_FILEUTIL_H
#define CORE_UTILS_FILEUTIL_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif 

// Boost includes
#include <boost/filesystem/path.hpp>

namespace Core
{

class FileUtil{

public:
  /// FINDFILESERIES
  /// Find a file series based on the file of one file
  static bool FindFileSeries( const boost::filesystem::path& first_file, std::vector<boost::filesystem::path >& filenames, std::string& error );

  /// OrderFILESERIES
  /// Order a file series
  static bool OrderFileSeries( const std::vector<boost::filesystem::path>& files, std::vector<boost::filesystem::path >& filenames, std::string& error );

  /// CheckFileExtension
  // Check whether file has one of given extensions
  static bool CheckExtension ( const boost::filesystem::path& file, const std::string& extensions );
};

} // end namespace Core

#endif