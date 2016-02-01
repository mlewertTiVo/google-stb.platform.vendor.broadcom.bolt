################################################################################
# Copyright (c) 2013-2014 Broadcom Corporation
# All Rights Reserved
# Confidential Property of Broadcom Corporation
# 
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
#
# This package implements the parsing, rendering, and modification of
# Device Trees.
#
# TODO: ( likely) interrupt-parent = < &{/soc/interrupt-controller@40000} >;
# TODO: (!likely) reg = reglabel: <0 sizelabel: 0x1000000>; 
# TODO: (!likely) prop = [ab cd ef byte4: 00 ff fe]; 
# TODO: (!likely) str = start: "string value" end: ;
################################################################################
package DevTree;
use strict;
use warnings FATAL=>q(all);
use File::Spec;
use File::Basename;
use Data::Dumper;
use lib dirname(File::Spec->rel2abs(__FILE__));
use Parse::RecDescent;
use Carp;

my $P = basename $::0;


$Parse::RecDescent::skip = 
qr{
	(
	\s+			# whitespace
	|			# or
	/[*] .*? [*]/ \s*	# a multiline comment
	|			# or
	//.*?$			# a single line comment
	)*			# zero or more
}mxs;


my $dt_grammar = q{
<autotree:DevTree>
	dts : '/dts-v1/' ';' memres(s?) '/' '{'  prop(s?) node(s?) '}' <commit> ';' | <error?>

        node : label(s?) name '{' prop(s?) node(s?) '}' <commit>  ';' | <error?>

        prop : label(s?) name '=' <commit> prop_val(s /,/)  ';' | label(s?) name ';' | <error?>

        memres : '/memreserve/' <commit> cint(2) ';' | <error?>

	label : var ':' | <error>

	prop_val : qstring | u32_seq | byte_seq | u32 | <error>

	byte_seq : '[' <commit> byte(s) ']' | <error?>

	u32_seq : '<' <commit> u32(s) '>' | <error?>

	u32 : /0x[0-9a-f]+/i | /[0-9]+/ | '&' var | <error>

	name : /[-a-z0-9,._+*\#@?]+/i <commit> .../[\{\=\;]/ | <error?:Unexpected text after name>

        var : /[a-z_][a-z0-9_]*/i

	byte : /[0-9a-f]{2}/i

	cint : /(0[xX][0-9a-fA-F]+|[0-9]+)(U|L|UL|LL|ULL)?/

	qstring : /"((?:\\"|[^"])*?)"/
};

$::RD_ERRORS = 1;
our $parser = Parse::RecDescent->new( $dt_grammar );

sub to_str($$) { (shift)->{"__VALUE__"}; }
sub tabs($$) { my ($s,$lvl) = @_; return "\t" x $lvl; }
sub new { croak "$P: error: DevTree is a pure base class!"; }
sub match_name($$)
{
	my ($s, $pattern) = @_;
	return undef if (!$s->{name});
	return undef if ('DevTree::name' ne ref($s->{name}));
	return $s if ('Regexp' ne ref($pattern) && $s->{name}->to_str eq $pattern);
	return $s if ('Regexp' eq ref($pattern) && $s->{name}->to_str =~ /$pattern/);
	return undef;
}

##############################################
package DevTree::node;
use base 'DevTree';
use Carp;

sub new($$) {
	my (undef, $str) = @_;
	my $arg = ref($str) ? $str : \ $str;
	my $s = $DevTree::parser->node( $arg );
	croak "$P: failed to parse DT node string" if (!$s);
	return $s;
}

sub to_str($$)
{
	my ($s,$lvl) = @_;
	$lvl ||= 0;
	my $t = "\n" . $s->tabs($lvl);
	map { $t .= $_->to_str($lvl); } @{$s->{'label(s?)'}};
	$t .= $s->{name}->to_str($lvl) . " {\n";
	map { $t .= $_->to_str($lvl+1); } @{$s->{'prop(s?)'}};
	map { $t .= $_->to_str($lvl+1); } @{$s->{'node(s?)'}};
	$t .= $s->tabs($lvl) . "};\n";
	return $t;
}

sub find_node($$)
{
	my ($s, $pattern) = @_;
	my @a;
	push (@a, $s) if ($s->match_name($pattern));
	push(@a, map { $_->find_node($pattern) } @{$s->{'node(s?)'}})
		if ($s->{'node(s?)'});
	return @a;
}

sub del_node($$)
{
	my ($s, $pattern) = @_;
	return 0 if (! $s->{'node(s?)'});
	my $n = scalar( @{$s->{'node(s?)'}} );
	my @a;
	my $count = 0;
	for (my $i=0; $i<$n; $i++) {
		my $x = $s->{'node(s?)'}->[$i];
		$count += $x->del_node($pattern);
		push(@a, $i) if ($x->match_name($pattern));
	}
	# Now delete the elems of the array by splicing.
	map { splice(@{$s->{'node(s?)'}}, $_, 1) } reverse @a;
	return $count + scalar(@a);
}

sub add_node($$)
{
	my ($s, $new) = @_;
	croak "$P: argument is not a DevTree::node"
		if ('DevTree::node' ne ref($new));
	push @{$s->{'node(s?)'}}, $new;
	$new->{'parent'} = $s;
	return $new;
}

sub find_prop($$)
{
	my ($s, $pattern) = @_;
	my @a;

	map { push(@a,$_) if ($_->match_name($pattern)); } @{$s->{'prop(s?)'}}
		if ($s->{'prop(s?)'});
	push(@a, map { $_->find_prop($pattern) } @{$s->{'node(s?)'}})
		if ($s->{'node(s?)'});
	return @a;
}

sub del_prop($$)
{
	my ($s, $pattern) = @_;
	return 0 if (! $s->{'prop(s?)'});
	my $n = scalar( @{$s->{'prop(s?)'}} );
	my @a;
	for (my $i=0; $i<$n; $i++) {
		my $x = $s->{'prop(s?)'}->[$i];
		push(@a, $i) if ($x->match_name($pattern));
	}
	# Now delete the elems of the array by splicing.
	map { splice(@{$s->{'prop(s?)'}}, $_, 1) } reverse @a;
	my $count = scalar(@a);

	# Recurse to children nodes.
	map { $count += $_->del_prop($pattern); } @{$s->{'node(s?)'}};
	return $count;
}

sub add_prop($$)
{
	my ($s, $p) = @_;
	croak "argument is not a DevTree::prop"
		if ('DevTree::prop' ne ref($p));
	push @{$s->{'prop(s?)'}}, $p;
	return $s;
}

sub get_root($)
{
	my ($n) = @_;
	while (exists($n->{'parent'})) {
		$n = $n->{'parent'};
	}
	return $n;
}

#
# Get a path string for a non-root node, with no trailing slash, e.g.
#
#   /rdb/serial@f040ab00
#
# This will give you an empty string:
#
#   $node->get_root()->get_path()
#
sub get_path($)
{
	my ($n) = @_;
	my $path = "";

	while (exists($n->{'parent'})) {
		$path = "/" . $n->{'name'}->to_str(0) . $path;
		$n = $n->{'parent'};
	}
	return $path;
}

##############################################
package DevTree::dts;
use base 'DevTree';
use base 'DevTree::node';
use Carp;

sub new_from_str($$) {
	my (undef, $str) = @_;
	my $s = $DevTree::parser->dts( \$str );
	croak "\n$P: failed to parse DT tree from string.\n"
		if (!$s);
	$str =~ /$Parse::RecDescent::skip/s;
	croak "\n$P: text leftover in string: <$str>.\n"
		if (length($str) && $str !~ /\A\s*\Z/s);
	return $s;
}


sub new_from_file($$) {
	my (undef, $file) = @_;
	croak "$P: error: file '$file' does not exist!\n"
		if (!-f $file);
	open(my $fh, '<', $file) 
		or croak "$P: error: could not open '$file' for reading.\n";
	my $str = join('', <$fh>);
	close($fh);
	my $s = $DevTree::parser->dts( \$str );
	die "\n$P: failed to parse DT tree from file '$file'.\n"
		if (!$s);
	$str =~ /$Parse::RecDescent::skip/s;
	die "\n$P: text leftover in '$file': <$str>.\n"
		if (length($str) && $str !~ /\A\s*\Z/s);
	return $s;
}

sub to_str($$)
{
	my ($s,$lvl) = @_;
	$lvl ||= 0;
	my $t = "/dts-v1/;\n\n";
	map { $t .= $_->to_str($lvl+0) } @{$s->{'memres(s?)'}};
	$t .= "\n/ {\n";
	map { $t .= $_->to_str($lvl+1) } @{$s->{'prop(s?)'}};
	map { $t .= $_->to_str($lvl+1) } @{$s->{'node(s?)'}};
	$t .= "};\n";
	return $t;
}


##############################################
package DevTree::prop;
use Data::Dumper;
use base 'DevTree';
use Carp;

sub new($$) {
	my (undef, $str) = @_;
	my $arg = ref($str) ? $str : \ $str;
	my $s = $DevTree::parser->prop( $arg );
	croak "failed to parse DT prop string" if (!$s);
	return $s;
}

sub to_str($$)
{
	my ($s,$lvl) = @_;
	$lvl ||= 0;
	my $t = $s->tabs($lvl);
	map { $t .= $_->to_str($lvl); } @{$s->{'label(s?)'}};
	return $t .= $s->{name}->to_str($lvl) . ";\n" 
		if (! $s->{'prop_val(s)'});
	$t .= $s->{name}->to_str($lvl) . " = ";
	my $n = @{$s->{'prop_val(s)'}};

	for (my $i=0; $i<$n; $i++) {
		my $pv = $s->{'prop_val(s)'}->[$i];
		$t .= $s->tabs($lvl+1) if ($i != 0);
		$t .= $pv->to_str($lvl);
		if ($i == $n-1) {
			$t .= ";\n";
		} else {
			$t .= ",\n";
		}
	}
	return $t;
}


##############################################
package DevTree::memres;
use Data::Dumper;
use base 'DevTree';
use Carp;

sub to_str($$)
{
	my ($s,$lvl) = @_;
	$lvl ||= 0;
	my $t = '/memreserve/ ';
	$t .= $s->{'cint(2)'}->[0]->to_str . ' ';
	$t .= $s->{'cint(2)'}->[1]->to_str . ";\n";
	return $t;
}


##############################################
package DevTree::label;
use base 'DevTree';

sub to_str($$)
{
	my ($s,$lvl) = @_;
	$lvl ||= 0;
	return $s->{var}->to_str($lvl) . ": ";
}


##############################################
package DevTree::prop_val;
use base 'DevTree';

sub to_str($$) 
{
	my ($s, $lvl) = @_;
	$lvl ||= 0;
	return $s->{u32_seq}->to_str($lvl)
		if ($s->{u32_seq});
	return $s->{byte_seq}->to_str($lvl)
		if ($s->{byte_seq});
	return $s->{qstring}->to_str($lvl)
		if ($s->{qstring});
	return $s->{u32}->to_str($lvl)
		if ($s->{u32});
	die "$P: internal error";
}


##############################################
package DevTree::u32_seq;
use base 'DevTree';

sub to_str($$) 
{
	my ($s, $lvl) = @_;
	$lvl ||= 0;
	my $n = scalar( @{$s->{'u32(s)'}} );
	my $t = '<';
	for (my $i=0; $i<$n; $i++) {
		if ($i>0 && 0==$i%4) {
			$t .= "\n" . $s->tabs($lvl+1);
		} elsif ($i>0) {
			$t .= " ";
		}
		$t .= $s->{'u32(s)'}->[$i]->to_str($lvl);
	}
	$t .= '>';
	return $t;
}


##############################################
package DevTree::byte_seq;
use base 'DevTree';

sub to_str($$) 
{
	my ($s, $lvl) = @_;
	$lvl ||= 0;
	my $t = '[';
	$t .= join(' ', map { $_->to_str($lvl) } @{$s->{'byte(s)'}});
	$t .= ']';
	return $t;
}


##############################################
package DevTree::u32;
use base 'DevTree';

sub to_str($$) 
{
	my ($s, $lvl) = @_;
	$lvl ||= 0;
	return '&' . $s->{var}->to_str($lvl)
		if ($s->{var});
	return "" . $s->{__VALUE__};
}


##############################################
package DevTree::name;
use base 'DevTree';
sub to_str($$) { (shift)->{"__PATTERN1__"}; }


##############################################
package DevTree::var;
use base 'DevTree';


##############################################
package DevTree::byte;
use base 'DevTree';


##############################################
package DevTree::qstring;
use base 'DevTree';

##############################################
package DevTree::cint;
use base 'DevTree';


##############################################
package DevTree;
use Data::Dumper;

sub _test_case()
{
	my $d_scripts = dirname(File::Spec->rel2abs(__FILE__));
	chdir("$d_scripts/..") or die;
	my $f = 'config/family-7445d0.dts';
	my $dt = DevTree::dts->new_from_file($f);
	my $n0 = $dt->del_node('serial@f040ab00');
	my $n1 = $dt->del_node(qr{serial});
	print $dt->to_str;
	print "$n0 nodes were deleted by name\n";
	print "$n1 nodes were deleted by regexp\n";
}

exit _test_case() unless caller();

1;
