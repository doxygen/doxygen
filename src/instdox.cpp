/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2000 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>

#include "qtbc.h"
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>

#include "instdox.h"
#include "doxygen.h"
#include "config.h"
#include "message.h"

void writeInstallScript()
{
  QCString fileName=Config::htmlOutputDir+"/installdox";
  QFile f(fileName);
  if (f.open(IO_WriteOnly))
  {
    QTextStream t(&f);
    t << "#!" << Config::perlPath << endl << endl << "%subst = ( ";
    
    char *s=Config::tagFileList.first();
    while (s)
    {
      QCString tagLine=s;
      QCString fileName;
      int eqPos = tagLine.find('=');
      if (eqPos!=-1) // strip destination part
      {
        fileName = tagLine.left(eqPos).stripWhiteSpace();
      }
      else
      {
        fileName = tagLine;
      }
      QFileInfo fi(fileName);
      t << "\"" << fi.fileName() << "\", \"\"";
      s=Config::tagFileList.next();
      if (s) t << ", ";
    }
    
    t << ");\n";
    t << "$quiet   = 0;\n";
    t << "\n";
    t << "if (open(F,\"search.cfg\"))\n";
    t << "{\n";
    t << "  $_=<F> ; s/[ \\t\\n]*$//g ; $subst{\"_doc\"} = $_;\n";
    t << "  $_=<F> ; s/[ \\t\\n]*$//g ; $subst{\"_cgi\"} = $_;\n";
    t << "}\n";
    t << "\n";
    t << "while ( @ARGV ) {\n";
    t << "  $_ = shift @ARGV;\n";
    t << "  if ( s/^-// ) {\n";
    t << "    if ( /^l(.*)/ ) {\n";
    t << "      $v = ($1 eq \"\") ? shift @ARGV : $1;\n";
    t << "      ($v =~ /\\/$/) || ($v .= \"/\");\n";
    t << "      $_ = $v;\n";
    t << "      if ( /(.+)\\@(.+)/ ) {\n";
    t << "        if ( exists $subst{$1} ) {\n";
    t << "          $subst{$1} = $2;\n";
    t << "        } else {\n";
    t << "          print STDERR \"Unknown tag file $1 given with option -l\\n\";\n";
    t << "          &usage();\n";
    t << "        }\n";
    t << "      } else {\n";
    t << "        print STDERR \"Argument $_ is invalid for option -l\\n\";\n";
    t << "        &usage();\n";
    t << "      }\n";
    t << "    }\n";
    t << "    elsif ( /^q/ ) {\n";
    t << "      $quiet = 1;\n";
    t << "    }\n";
    t << "    elsif ( /^\\?|^h/ ) {\n";
    t << "      &usage();\n";
    t << "    }\n";
    t << "    else {\n";
    t << "      print STDERR \"Illegal option -$_\\n\";\n";
    t << "      &usage();\n";
    t << "    }\n";
    t << "  }\n";
    t << "  else {\n";
    t << "    push (@files, $_ );\n";
    t << "  }\n";
    t << "}\n";
    t << "\n";
    t << "foreach $sub (keys %subst)\n";
    t << "{\n";
    t << "  if ( $subst{$sub} eq \"\" ) \n";
    t << "  {\n";
    t << "    print STDERR \"No substitute given for tag file `$sub'\\n\";\n";
    t << "    &usage();\n";
    t << "  }\n";
    t << "  elsif ( ! $quiet && $sub!=\"_doc\" && $sub!=\"_cgi\" )\n";
    t << "  {\n";
    t << "    print \"Substituting $subst{$sub} for each occurence of tag file $sub\\n\"; \n";
    t << "  }\n";
    t << "}\n";
    t << "\n";
    t << "if ( ! @files ) {\n";
    t << "  if (opendir(D,\".\")) {\n";
    t << "    foreach $file ( readdir(D) ) {\n";
    t << "      $match = \".html\";\n";
    t << "      next if ( $file =~ /^\\.\\.?$/ );\n";
    t << "      ($file =~ /$match/) && (push @files, $file);\n";
    t << "      ($file =~ \"tree.js\") && (push @files, $file);\n";
    t << "    }\n";
    t << "    closedir(D);\n";
    t << "  }\n";
    t << "}\n";
    t << "\n";
    t << "if ( ! @files ) {\n";
    t << "  print STDERR \"Warning: No input files given and non found!\\n\";\n";
    t << "}\n";
    t << "\n";
    t << "foreach $f (@files)\n";
    t << "{\n";
    t << "  if ( ! $quiet ) {\n";
    t << "    print \"Editing: $f...\\n\";\n";
    t << "  }\n";
    t << "  $oldf = $f;\n";
    t << "  $f   .= \".bak\";\n";
    t << "  unless (rename $oldf,$f) {\n";
    t << "    print STDERR \"Error: cannot rename file $oldf\\n\";\n";
    t << "    exit 1;\n";
    t << "  }\n";
    t << "  if (open(F,\"<$f\")) {\n";
    t << "    unless (open(G,\">$oldf\")) {\n";
    t << "      print STDERR \"Error: opening file $oldf for writing\\n\";\n";
    t << "      exit 1;\n";
    t << "    }\n";
    t << "    if ($oldf!=\"tree.js\") {\n";
    t << "      while (<F>) {\n";
    t << "        s/doxygen\\=\\\"([^ \\\"\\:\\t\\>\\<]*)\\:([^ \\\"\\t\\>\\<]*)\\\" (href|src)=\\\"\\2/doxygen\\=\\\"$1:$subst{$1}\\\" \\3=\\\"$subst{$1}/g;\n";
    t << "        print G \"$_\";\n";
    t << "      }\n";
    t << "    }\n";
    t << "    else {\n";
    t << "      while (<F>) {\n";
    t << "        s/\\\"([^ \\\"\\:\\t\\>\\<]*)\\:([^ \\\"\\t\\>\\<]*)\\\", \\\"\\2/\\\"$1:$subst{$1}\\\" ,\\\"$subst{$1}/g;\n";
    t << "        print G \"$_\";\n";
    t << "      }\n";
    t << "    }\n";
    t << "  } \n";
    t << "  else {\n";
    t << "    print STDERR \"Warning file $f does not exist\\n\";\n";
    t << "  }\n";
    t << "  unlink $f;\n";
    t << "}\n";
    t << "\n";
    t << "sub usage {\n";
    t << "  print STDERR \"Usage: installdox [options] [html-file [html-file ...]]\\n\";\n";
    t << "  print STDERR \"Options:\\n\";\n";
    t << "  print STDERR \"     -l tagfile\\@linkName   tag file + URL or directory \\n\";\n";
    t << "  print STDERR \"     -q                    Quiet mode\\n\\n\";\n";
    t << "  exit 1;\n";
    t << "}\n";
  }
  else
  {
    err("Error: Cannot open file %s for writing\n",fileName.data());
  }
  f.close();
  struct stat stat_struct;
  stat(fileName,&stat_struct);
#if !defined(_WIN32)
  chmod(fileName,stat_struct.st_mode|S_IXUSR|S_IXGRP|S_IXOTH);
#endif
}
