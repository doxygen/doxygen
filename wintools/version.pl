# Extract the version information from the configure script and write
# it in the version of the VERSION file to stdout.
# Should be called from the root of the distribution.

open(FILE,"<configure") || die "Could not open configure file!";

while (<FILE>) {
  if (/doxygen_version_major=(\d+)/) {
    $major=$1
  } elsif (/doxygen_version_minor=(\d+)/) {
    $minor=$1
  } elsif (/doxygen_version_revision=(\d+)/) {
    $revision=$1
  } elsif (/doxygen_version_mmn=(\w+)/) {
    $mmn=$1
  }
}

if ($mmn) {
  print "$major.$minor.$revision-$mmn\n";
} else {
  print "$major.$minor.$revision\n";
}

close(FILE);
