#!/usr/local/bin/perl
#
# Copyright & Disclaimer.
#	This set of routines may be freely distributed, modified and
#	used, provided this copyright & disclaimer remains intact.
#	This package is used at your own risk, if it does what you
#	want, good; if it doesn't, modify it or use something else--but
#	don't blame me. Support level = negligable (i.e. mail bugs but
#	not requests for extensions)
#
# Module: dcmwlm (WWW Component)
#
# Author:
# 	James Tappin: sjt@xun8.sr.bham.ac.uk
#	School of Physics & Space Research University of Birmingham
#	Feb 1993.		
# 
# Purpose:
#   The CGI_HANDLERS deal with basic CGI POST or GET method request
#   elements such as those delivered by an HTTPD form, i.e. a url
#   encoded line of "=" separated key=value pairs separated by &'s
#
# Renamed from "cgi_handlers.pl" to "urldecod.ph" to conform to
# ISO High Sierra file naming conventions.
#
# Last Update:      $Author: braindead $
# Update Date:      $Date: 2007/04/24 09:53:39 $
# Source File:      $Source: /sources/aeskulap/aeskulap/dcmtk/dcmwlm/perl/urldecod.ph,v $
# CVS/RCS Revision: $Revision: 1.2 $
# Status:           $State: Exp $
#
# CVS/RCS Log
#   $Log: urldecod.ph,v $
#   Revision 1.2  2007/04/24 09:53:39  braindead
#   - updated DCMTK to version 3.5.4
#   - merged Gianluca's WIN32 changes
#
#   Revision 1.1.1.1  2006/07/19 09:16:47  pipelka
#   - imported dcmtk354 sources
#
#
#   Revision 1.1  2002/12/03 12:16:18  wilkens
#   Added files und functionality from the dcmtk/wlisctn folder to dcmtk/dcmwlm
#   so that dcmwlm can now completely replace wlistctn in the public domain part
#   of dcmtk. Pertaining to this replacement requirement, another optional return
#   key attribute was integrated into the wlm utilities.
#
#
#


# Routines:
# get_request:	reads the request and returns both the raw and
#               processed version.
# url_decode:	URL decodes a string or array of strings
# html_header:	Transmits a HTML header back to the caller
# html_trailer: Transmits a HTML trailer back to the caller

# Usage:
#	needs a 'require "urldecod.ph";' line in the main script
#
#	&get_request;    will get the request and decode it into an
#			 indexed array %rqpairs, the raw request is in
#			 $request
#
#	... = &url_decode(LIST); will return a URL decoded version of
#			         the contents of LIST
#
#	&html_header(TITLE); 	will write to standard output an HTML
#				header (including the content-type
#				field) giving the document the title
#				specified by TITLE.
#
#	&html_trailer;		Writes a trailer to the html document
#				with the name of the script generating
#				it and the date (in UT).

sub get_request {

    # Subroutine get_request reads the POST or GET form request from STDIN
    # into the variable  $request, and then splits it into its
    # name=value pairs in the associative array %rqpairs.
    # The number of bytes is given in the environment variable
    # CONTENT_LENGTH which is automatically set by the request generator.

    # Encoded HEX values and spaces are decoded in the values at this
    # stage.

    # $request will contain the RAW request. N.B. spaces and other
    # special characters are not handler in the name field.

    if ($ENV{'REQUEST_METHOD'} eq "POST") {
	read(STDIN, $request, $ENV{'CONTENT_LENGTH'});
    } elsif ($ENV{'REQUEST_METHOD'} eq "GET" ) {
	$request = $ENV{'QUERY_STRING'};
    }

    %rqpairs = &url_decode(split(/[&=]/, $request));
}

sub url_decode {

#	Decode a URL encoded string or array of strings 
#		+ -> space
#		%xx -> character xx

    foreach (@_) {
	tr/+/ /;
	s/%(..)/pack("c",hex($1))/ge;
    }
    @_;
}

sub html_header {

    # Subroutine html_header sends to Standard Output the necessary
    # material to form an HHTML header for the document to be
    # returned, the single argument is the TITLE field.

    local($title) = @_;

    print "Content-type: text/html\n\n";
    print "<html><head>\n";
    print "<title>$title</title>\n";
    print "</head>\n<body>\n";
}

sub html_trailer {

    # subroutine html_trailer sends the trailing material to the HTML
    # on STDOUT.

    local($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst)
	= gmtime;

    local($mname) = ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
		     "Aug", "Sep", "Oct", "Nov", "Dec")[$mon];
    local($dname) = ("Sun", "Mon", "Tue", "Wed", "Thu", "Fri",
		     "Sat")[$wday]; 

    print "<p>\nGenerated by: <var>$0</var><br>\n";
    print "Date: $hour:$min:$sec UT on $dname $mday $mname $year.<p>\n";
    print "</body></html>\n";
}

1;

