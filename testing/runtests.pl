#!/usr/bin/perl

# perl script to execute doxygen's regression test suite.
#
# Copyright (C) 1997-2013 by Dimitri van Heesch.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation under the terms of the GNU General Public License is hereby 
# granted. No representations are made about the suitability of this software 
# for any purpose. It is provided "as is" without express or implied warranty.
# See the GNU General Public License for more details.
#
# Documents produced by Doxygen are derivative works derived from the
# input used in their production; they are not affected by this license.

use strict;
use warnings;

use Getopt::Long;
use Test::More;
use File::Path qw(make_path remove_tree);
use File::Copy qw(copy);

my $Test = Test::Builder->new;
my $opt_doxygen_exe = 'doxygen';
my $opt_xmllint_exe = 'xmllint';
my $opt_updateref = '';
my @opt_test_ids;
my $opt_all = '';

GetOptions( 'updateref'  => \$opt_updateref,
            'doxygen=s'  => \$opt_doxygen_exe,
            'xmllint=s'  => \$opt_xmllint_exe,
            'id=i'       => \@opt_test_ids,
            'all'        => \$opt_all
          );

sub read_two_files {
  my $first   = shift;
  my $second  = shift;
  my $filter  = shift;
  my $success = 1;
  my @errors;

  unless (open FIRST, "$first") {
    $success = 0;
    push @errors, "$first absent";
  }
  unless (open SECOND, "$second") {
    $success = 0;
    push @errors, "$second absent";
  }
  return ($success, @errors) unless $success;

  my $first_lines  = join "",<FIRST>;
  my $second_lines = join "",<SECOND>;
  
  close FIRST;
  close SECOND;

  return ($success, $first_lines, $second_lines);
}

sub compare_ok {
  my $got_file      = shift;
  my $expected_file = shift;
  my $name          = shift;
  my @read_result   = read_two_files($got_file, $expected_file);
  my $files_exist   = shift @read_result;

  if ($files_exist) {
    my ($got, $expected) = @read_result;
    my $diff = `diff -u $got_file $expected_file`;
    my $failed = length $diff;
    return ($failed,"Difference between generated output and reference:\n$diff");
  }
  else {
    return (1,join "\n", @read_result);
  }
}

sub chop_volatile {
  my $line = shift;
  $line =~ s/version="\d\.\d.\d+"/version=""/g; # strip version
  $line =~ s/file=".*\/(.*)"/file="$1"/g; # strip location
  return $line;
}

sub get_config {
  my $file = shift;
  my %config;
  open F,"<$file";
  while (<F>) {
    if (/\/\/\s*(\S+):\s*(.*)$/) {
      my $key = $1;
      my $val = $2;
      chomp $val;
      $config{$key} = [] unless defined $config{$key};
      push @{$config{$key}},$val;
    }
  }
  return %config;
}

sub perform_test {
  my $test_file = shift;
  my %config = get_config($test_file);
  my $test_name = "[$test_file]: $config{'objective'}[0]";
  my $test_id = $test_file;
  $test_id =~ s/^(\d+).*$/$1/;
  my $test_out = "test_output_${test_id}";

  if (scalar($config{'check'})==0) {
    $Test->ok(0, $test_name);
    $Test->diag("Test doesn't specify any files to check");
    return;
  }

  # prepare test environment
  remove_tree("$test_out");
  make_path("$test_out");
  copy("Doxyfile","$test_out");
  open(F,">>$test_out/Doxyfile");
  print F "INPUT = $test_file\n";
  print F "XML_OUTPUT = $test_out/out\n";
  foreach my $cfg (@{$config{'config'}}) {
    print F "$cfg\n";
  }
  close(F);

  # run doxygen
  if (system("$opt_doxygen_exe $test_out/Doxyfile")!=0) {
    $Test->ok(0, $test_name);
    $Test->diag("Failed to run doxygen");
    return;
  } 

  # look for files to check against the reference
  foreach my $fn (@{$config{'check'}}) {
    if (!-f "$test_out/out/$fn") {
      $Test->ok(0, $test_name);
      $Test->diag("Non-existing file $test_out/out/$fn after 'check:' statement");
      return;
    }
    # run xmllint on the output file
    my @lines = `$opt_xmllint_exe --format --noblanks --nowarning $test_out/out/$fn`;
    if (scalar(@lines)>0 && open(F,">$test_out/$fn")) {
      foreach my $line (@lines) {
        print F chop_volatile($line);
      }
      close(F);
    } else {
      $Test->ok(0, $test_name);
      $Test->diag("Failed to run xmllint on the doxygen output file $test_out/out/$fn");
    }
    my ($failed,$msg) = compare_ok("$test_out/$fn","$test_id/$fn",$test_name);
    if ($failed) {
      $Test->ok(0, $test_name);
      $Test->diag($msg);
      return;
    }
  }

  # test passed
  remove_tree("$test_out");
  $Test->ok(1, $test_name);
}

sub update_test {
  my $test_file = shift;
  my %config = get_config($test_file);
  my $test_name = "[$test_file]: $config{'objective'}[0]";
  my $test_id = $test_file;
  $test_id =~ s/^(\d+).*$/$1/;
  my $test_out = $test_id;

  # prepare reference environment
  remove_tree("$test_out");
  make_path("$test_out");
  copy("Doxyfile","$test_out");
  open(F,">>$test_out/Doxyfile");
  print F "INPUT = $test_file\n";
  print F "XML_OUTPUT = $test_out/out\n";
  foreach my $cfg (@{$config{'config'}}) {
    print F "$cfg\n";
  }
  close(F);

  print "Updating reference for $test_name\n";

  # run doxygen
  if (system("$opt_doxygen_exe $test_out/Doxyfile")!=0) {
    print("Error: failed to run doxygen");
    return;
  } 
  my $err=0;

  # look for files to prepare as reference
  foreach my $fn (@{$config{'check'}}) {
    if (!-f "$test_out/out/$fn") {
      printf("Error: Non-existing file $test_out/out/$fn after 'check:' statement\n");
      $err=1;
    }
    # run xmllint on the output file
    if (!$err) {
      my @lines = `$opt_xmllint_exe --format --noblanks --nowarning $test_out/out/$fn`;
      if (scalar(@lines)>0 && open(F,">$test_out/$fn")) {
        foreach my $line (@lines) {
          print F chop_volatile($line);
        }
        close(F);
      } else {
        printf("Error: Failed to run xmllint on the doxygen output file $test_out/out/$fn\n");
        $err=1;
      }
    }
  }

  if (!$err) {
    # clean-up
    remove_tree("$test_out/out");
    unlink("$test_out/Doxyfile");
  }
}

# get the tests
my @tests;
if (scalar(@opt_test_ids)==0 && $opt_updateref && !$opt_all) {
  printf("Error: updateref option requires -id to update a test or -all to update all\n");
  exit(1);
}
if (scalar(@opt_test_ids)>0) {
  foreach my $t (@opt_test_ids) {
    push @tests, glob("${t}_* 0${t}_* 00${t}_*");
  }
} else {
  @tests = glob('[0-9][0-9][0-9]_*');
}

if ($opt_updateref) {
  # update reference
  foreach my $test (@tests) {
    update_test($test);
  }
} else {
  # run tests
  plan tests => scalar(@tests);
  foreach my $test (@tests) {
    perform_test($test);
  }
}

