/*=========================================================================

  Program:   ALFABIS fast medical image registration programs
  Language:  C++
  Website:   github.com/pyushkevich/greedy
  Copyright (c) Paul Yushkevich, University of Pennsylvania. All rights reserved.

  This program is part of ALFABIS: Adaptive Large-Scale Framework for
  Automatic Biomedical Image Segmentation.

  ALFABIS development is funded by the NIH grant R01 EB017255.

  ALFABIS is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ALFABIS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ALFABIS.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#ifndef COMMANDLINEHELPER_H
#define COMMANDLINEHELPER_H

#include <cerrno>
#include <iostream>
#include <sstream>
#include "vtksys/SystemTools.hxx"


/**
 * A command line parsing exception
 */
class CommandLineException : public std::exception
{
public:

  CommandLineException(const char *format, ...)
    {
    buffer = new char[4096];
    va_list args;
    va_start (args, format);
    vsprintf (buffer,format, args);
    va_end (args);
    }

  virtual const char* what() const throw() { return buffer; }

  virtual ~CommandLineException() throw() { delete buffer; }

private:

  char *buffer;

};

/**
 * @brief A class to facilitate processing of command-line arguments
 */
class CommandLineHelper
{
public:
  CommandLineHelper(int argc, char *argv[])
  {
    this->argc = argc;
    this->argv = argv;
    i = 1;
  }

  bool is_at_end()
  {
    return i >= argc;
  }

  /**
   * Just read the next arg (used internally)
   */
  const char *read_arg()
  {
    if(i >= argc)
      throw CommandLineException("Unexpected end of command line arguments.");

    return argv[i++];
  }

  /**
   * Peak at the next argument
   */
  const char *peek_arg()
  {
    if(i >= argc)
      throw CommandLineException("Unexpected end of command line arguments.");

    return argv[i];
  }

  /**
   * Read a command (something that starts with a '-')
   */
  std::string read_command()
  {
    current_command = read_arg();
    if(current_command[0] != '-')
      throw CommandLineException("Expected a command at position %d, instead got '%s'.", i, current_command.c_str());
    return current_command;
  }

  /**
   * Read a command if it matches the provided command
   */
  bool try_command(const char *command)
  {
    if(i >= argc)
      throw CommandLineException("Unexpected end of command line arguments.");

    if(strcmp(command, argv[i]) == 0)
      {
      i++;
      return true;
      }
    else return false;
  }

  bool try_command(const char *command1, const char *command2)
  {
    return try_command(command1) || try_command(command2);
  }

  bool try_command(const char *command1, const char *command2, const char *command3)
  {
    return try_command(command1) || try_command(command2) || try_command(command3);
  }

  /**
   * Read a string that is not a command (may not start with a -)
   */
  std::string read_string()
  {
    std::string arg = read_arg();
    if(arg[0] == '-')
      throw CommandLineException("Expected a string argument as parameter to '%s', instead got '%s'.", current_command.c_str(), arg.c_str());

    return arg;
  }


  /**
   * Get the number of free arguments to the current command. Use only for commands with
   * a priori unknown number of arguments. Otherwise, just use the get_ commands
   */
  int command_arg_count(int min_required = 0)
  {
    // Count the number of arguments
    int n_args = 0;
    for(int j = i; j < argc; j++, n_args++)
      if(argv[j][0] == '-')
        break;

    // Test for minimum required
    if(n_args < min_required)
      throw CommandLineException(
          "Expected at least %d arguments to '%s', instead got '%d'",
          min_required, current_command.c_str(), n_args);

    return n_args;
  }

  /**
   * Read an existing filename
   */
  std::string read_existing_filename()
  {
    std::string file = read_arg();
    if(!vtksys::SystemTools::FileExists(file.c_str()))
      throw CommandLineException("File '%s' does not exist", file.c_str());

    return file;
  }

  /**
   * Read a transform specification, format file,number
   */
  /*
  TransformSpec read_transform_spec()
  {
    std::string spec = read_arg();
    size_t pos = spec.find_first_of(',');

    TransformSpec ts;
    ts.filename = spec.substr(0, pos);
    ts.exponent = 1.0;

    if(!vtksys::SystemTools::FileExists(ts.filename.c_str()))
      throw CommandLineException("File '%s' does not exist", ts.filename.c_str());

    if(pos != std::string::npos)
      {
      errno = 0; char *pend;
      std::string expstr = spec.substr(pos+1);
      ts.exponent = std::strtod(expstr.c_str(), &pend);

      if(errno || *pend)
        throw CommandLineException("Expected a floating point number after comma in transform specification '%s', instead got '%s'",
                              current_command.c_str(), spec.substr(pos).c_str());

      }

    return ts;
  }
  */

  /**
   * Read an output filename
   */
  std::string read_output_filename()
  {
    std::string file = read_arg();
    return file;
  }

  /**
   * Read a floating point value
   */
  double read_double()
  {
    std::string arg = read_arg();

    errno = 0; char *pend;
    double val = std::strtod(arg.c_str(), &pend);

    if(errno || *pend)
      throw CommandLineException("Expected a floating point number as parameter to '%s', instead got '%s'",
                            current_command.c_str(), arg.c_str());

    return val;
  }

  /**
   * Check if a string ends with another string and return the
   * substring without the suffix
   */
  bool check_suffix(const std::string &source, const std::string &suffix, std::string &out_prefix)
  {
    int n = source.length(), m = suffix.length();
    if(n < m)
      return false;

    if(source.substr(n-m, m) != suffix)
      return false;

    out_prefix = source.substr(0, n-m);
    return true;
  }

  /**
   * Read a floating point value with units (mm or vox)
   */
  double read_scalar_with_units(bool &physical_units)
  {
    std::string arg = read_arg();
    std::string scalar;

    if(check_suffix(arg, "vox", scalar))
      physical_units = false;
    else if(check_suffix(arg, "mm", scalar))
      physical_units = true;
    else
      throw CommandLineException("Parameter to '%s' should include units, e.g. '3vox' or '3mm', instead got '%s'",
                            current_command.c_str(), arg.c_str());

    errno = 0; char *pend;
    double val = std::strtod(scalar.c_str(), &pend);

    if(errno || *pend)
      throw CommandLineException("Expected a floating point number as parameter to '%s', instead got '%s'",
                            current_command.c_str(), scalar.c_str());

    return val;
  }

  /**
   * Read an integer value
   */
  long read_integer()
  {
    std::string arg = read_arg();

    errno = 0; char *pend;
    long val = std::strtol(arg.c_str(), &pend, 10);

    if(errno || *pend)
      throw CommandLineException("Expected an integer as parameter to '%s', instead got '%s'",
                            current_command.c_str(), arg.c_str());

    return val;
  }

  /**
   * Read one of a list of strings. The optional parameters to this are in the form
   * int, string, int, string, int, string. Each string may in turn contain a list
   * of words (separated by space) that are acceptable. So for example. NULL string
   * is used to refer to the default option.
   *
   * enum Mode { NORMAL, BAD, SILLY }
   * Mode m = X.read_option(NORMAL, "NORMAL normal", BAD, "bad BAD", SILLY, NULL);
   */
  /*
  template <class TOption>
  TOption read_option(TOption opt1, const char *str1, ...)
  {
    not implemented yet
  }
  */

  /**
   * Read a vector in the format 1.0x0.2x0.6
   */
  std::vector<double> read_double_vector()
  {
    std::string arg = read_arg();
    std::istringstream f(arg);
    std::string s;
    std::vector<double> vector;
    while (getline(f, s, 'x'))
      {
      errno = 0; char *pend;
      double val = std::strtod(s.c_str(), &pend);

      if(errno || *pend)
        throw CommandLineException("Expected a floating point vector as parameter to '%s', instead got '%s'",
                              current_command.c_str(), arg.c_str());
      vector.push_back(val);
      }

    if(!vector.size())
      throw CommandLineException("Expected a floating point vector as parameter to '%s', instead got '%s'",
                            current_command.c_str(), arg.c_str());

    return vector;
  }

  std::vector<int> read_int_vector()
  {
    std::string arg = read_arg();
    std::istringstream f(arg);
    std::string s;
    std::vector<int> vector;
    while (getline(f, s, 'x'))
      {
      errno = 0; char *pend;
      long val = std::strtol(s.c_str(), &pend, 10);

      if(errno || *pend)
        throw CommandLineException("Expected an integer vector as parameter to '%s', instead got '%s'",
                              current_command.c_str(), arg.c_str());
      vector.push_back((int) val);
      }

    if(!vector.size())
      throw CommandLineException("Expected an integer vector as parameter to '%s', instead got '%s'",
                            current_command.c_str(), arg.c_str());

    return vector;
  }





private:
  int argc, i;
  char **argv;
  std::string current_command;
};

#endif // COMMANDLINEHELPER_H
