# make script used to create a config file for windows

use Cwd;

# get current working directory
$pwd=cwd();

# create config file
open(FILE,">makeconfig") || die "Cannot create file makeconfig!";

print FILE "DOXYGEN   = $pwd\n";
print FILE "TMAKEPATH = $pwd\\tmake\\lib\\win32-msvc\n";
print FILE "TMAKE     = $pwd\\tmake\\bin\\tmake\n";
print FILE "MAKE      = nmake\n";
print FILE "PERL      = perl\n";
print FILE "RM        = del /s /q\n";
print FILE "VERSION   = ";

# copy contents of VERSION file to FILE
open(VERFILE,"<VERSION") || die "Cannot open file VERSION for reading!";
while (<VERFILE>) {
  print FILE $_;
}
close VERFILE;
close FILE;
