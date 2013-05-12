# script to split file into parts of roughly 32kb
#!/bin/perl
my $file = shift;
my $target = shift;
my $count = 1;
my $len = 0;
$target=~/p(\d+).js/;
my $part = $1;
open(F,"<$file") || die ("cannot open file for reading: $!");
open(G,">$target") || die("cannot open file for writing: $!");
while (<F>)
{
  if ($part==$count)
  {
    print G "$_";
  }
  $len+=length($_);
  if ($len>32768) 
  {
    $len=0;
    $count++;
  }
}
close(F);
close(G);
