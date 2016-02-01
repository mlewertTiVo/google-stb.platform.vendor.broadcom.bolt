#!/usr/bin/perl -w
###########################################################################
#     Copyright (c) 2015, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
###########################################################################

#  Example for BOLT v1.01, with all severity levels (i.e. <= 5) and a
# template message under each Jira issue link:
#
# [user@linux]$ ./scripts/relnotes.pl -p <corp_passwd> -v1.01 -s5 \
# -i -m "Please take this release if..." && make doc/relnotes.html

use strict;
use warnings;

#  IMPORTANT: You may have to do a local Perl (module) install
# if your build server does not support:
use File::Basename;
use Data::Dumper;
use REST::Client;
use JSON;
use MIME::Base64;
use CGI qw(:standard);
use URI::Escape;
use Getopt::Std;

my $P = basename $0;

# consts -------------------------------------------------------------------
# We should not need to update these unless the bug dbase gets changed.

use constant JIRA_URL => "http://jira.broadcom.com";

# Jira REST API
use constant REST_VER => "/rest/api/2/";

# Issue list JQL web query
use constant JIRA_WEBQUERY => JIRA_URL . "/issues/?jql=";

use constant SEVERITY => "2";


# Fixed vars ---------------------------------------------------------------
# ...for now.

my $fname = "doc/relnotes.txt";

my %sevhash = (
	1 => 'blocker',
	2 => 'major',
	3 => 'normal',
	4 => 'minor',
	5 => 'very low',
);

my %colorhash = (
	1 => 'red',
	2 => 'yellow',
	3 => 'lime',
	4 => 'aqua',
	5 => 'blue',
);


# Args ---------------------------------------------------------------------

my $env_user = $ENV{USER};

my $arg_user = $env_user;
my $arg_password = undef;
my $arg_version = undef;
my $arg_severity = SEVERITY;
my $arg_insert = 0;
my $arg_proforma = undef;


# Helpers ------------------------------------------------------------------

sub client_errors($$) {
	my ($rc, $content) = @_;

	select STDERR;

	foreach my $msg (@{$content->{'errorMessages'}}) {
		print "Error: $msg\n";
	}
}

sub rest_request($$$)
{
	my ($client, $auth, $req) = @_;

	$client->GET(REST_VER . $req, $auth);

	my $rc = $client->responseCode();
	die client_errors($rc, from_json($client->responseContent()))
		if $rc ne '200';

	return from_json($client->responseContent());
}

sub rest_field_id_by_name($$$)
{
	my ($client, $auth, $name) = @_;

	my $fields = rest_request($client, $auth, "field");

	foreach my $f (@$fields) {
		return $f->{'id'}
			if ($f->{'name'} eq $name);
	}
	die("Cannot find field '$name'");
}

sub usage()
{
	print " help: $P [options]\n";
	print "  -h ................This help message\n";
	print "  -u <name> ........ Jira username, or default: '$env_user'\n";
	print "  -p <password> .... Jira password\n";
	print "  -v ............... Jira verson tag (in N.NN format)\n";
	print "  -s ............... Jira 'Severity' level, default: "
		. SEVERITY . "\n";
	print "  -i ............... Insert into release notes\n";
	print "  -m ............... Proforma Message under each Jira link\n";
}

sub usage_exit($)
{
	my $x = shift @_;
	usage();
	exit $x;
}

sub failed($)
{
	my $x = shift @_;
	print " *** fail: " . $x . "\n";
	usage_exit(-1);
}


# Getopts ------------------------------------------------------------------

my %cmdflags=();
getopts("hu:p:v:s:im:", \%cmdflags) or usage_exit(-1);

usage_exit(0)
	if (defined $cmdflags{h});

$arg_user = $cmdflags{u}
	if (defined $cmdflags{u});

$arg_password = $cmdflags{p}
	if (defined $cmdflags{p});

$arg_version = "v" . $cmdflags{v}
	if (defined $cmdflags{v});

$arg_severity = scalar($cmdflags{s})
	if (defined $cmdflags{s});

$arg_insert = 1
	if (defined $cmdflags{i});

$arg_proforma = $cmdflags{m}
	if (defined $cmdflags{m});


# Cmdflags checks ----------------------------------------------------------

failed("no username, password or version given.")
	if (!defined $arg_user || !defined $arg_password
		|| !defined $arg_version);

# Configure ----------------------------------------------------------------

my $relnotes = "\n";

my $jql_query = uri_escape("project = SWBOLT and fixVersion = \"" .
	$arg_version . "\" ORDER BY Severity");

my $auth_header = {
	Accept => 'application/json',
	Authorization => 'Basic ' .
	encode_base64($arg_user . ':' . $arg_password)
};

my $client = REST::Client->new();
$client->setHost(JIRA_URL);


# Load ---------------------------------------------------------------------

my $id = rest_field_id_by_name($client, $auth_header, "Severity");

my $response = rest_request($client, $auth_header, "search?jql=" . $jql_query);
my $issues = $response->{'issues'};

foreach my $issue (@$issues) {
	my $key     = $issue->{'key'};
	my $summary = $issue->{'fields'}->{'summary'};
	my $sev = scalar(substr($issue->{'fields'}->{$id}->{'value'}, 0, 1));
	die("Unknown Severity level" . $sev)
		if (($sev < 1) || ($sev > 5));
	next
		if ($sev > $arg_severity);

	my $link = JIRA_URL . "/browse/" . $key;
	$relnotes .= "link:" .  $link  . "[" . $key . "] " . $summary . " ";
	$relnotes .= "^[" . $colorhash{$sev} . " silver-background]#";
	$relnotes .= $sevhash{$sev} . "#^\n\n";

	$relnotes .= "  " . $arg_proforma . "\n\n"
		if (defined  $arg_proforma);
}

if ($arg_insert < 1) {
	print $relnotes;
	exit(0);
}


# Insert -------------------------------------------------------------------
# Requires a fixed relnotes section title format.

my $fh;
my $mark = 0;
my $jiralist = JIRA_WEBQUERY .  $jql_query . "[Here]\n";

open($fh, '<', $fname)
		or die("could not open file '$fname'");
my @lines = <$fh>;
close($fh);

open($fh, '>', $fname)
		or die("could not open file '$fname'");

for (my $i = 0; $i < (@lines); $i++) {

	if ($lines[$i] =~ m/Upgrade to v\d\.\d\d criteria/) {
		print $fh " Upgrade to " . $arg_version. " criteria\n";
		print $fh $lines[$i+1];
		print $fh $relnotes;
		$mark = 1;
		next;
	}

	if ($lines[$i] =~ m/Broadcom FAE access/) {
		print $fh $lines[$i];
		print $fh $lines[$i+1];
		print $fh $jiralist;
		$i += 2;
		$mark = 0;
		next;
	}

	print $fh $lines[$i]
		if ($mark == 0);
}

close($fh);

