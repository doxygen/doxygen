use DoxyModel;

sub template($) {
    my $model = $_[0];
    my ($type, $name) = @$model[0, 1];
    if ($type eq "field") {
	print TEXTEMPLATE "\\def\\" . $name . "#1{#1}%\n";
    } elsif ($type eq "doc") {
	print TEXTEMPLATE "\\def\\" . $name . "#1{#1}%\n";
    } elsif ($type eq "hash") {
	my ($key, $value);
	while (($key, $submodel) = each %{$$model[2]}) {
	    my $subname = $$submodel[1];
	    print TEXTEMPLATE "\\def\\" . $subname . "Empty{}%\n";
	    print TEXTEMPLATE "\\def\\f" . $subname . "Empty{\\" . $subname . "Empty}%\n";
	    template($submodel);
	}
	print TEXTEMPLATE "\\def\\" . $name . "{}%\n";
    } elsif ($type eq "list") {
	template($$model[2]);
	print TEXTEMPLATE "\\def\\" . $name . "#1{#1}%\n";
	print TEXTEMPLATE "\\def\\" . $name . "Sep{}%\n";
    }
}

open TEXTEMPLATE, ">doxytemplate.tex";
template($doxymodel);
close TEXTEMPLATE;
