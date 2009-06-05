#!/usr/bin/perl
#
# (c) Copyright 2002, 2003 Motorola, Inc.  All Rights Reserved.
#
# checkConventions.pl
#
# Checks that certain coding conventions are adhered to be the input file(s),
# and runs a spell check on C-style comments.
# Problems found are reported to standard output.
#

# ispell regards _ as a word terminator, so eliminate these
$spellCmd = "sed 's/_/A/g' |sed 's/0x/xA/g' | aspell -l --mode=none --ignore-case|sort -u";

$usage = <<USAGE;

[36m[40mUsage:
perl checkConventions.pl [-nospell] file1.c [file2.java file3.h ...]
[m
Checks for certain WSAS coding conventions.
Returns 0 if conventions are met, non-zero otherwise.
USAGE

die($usage) if ($#ARGV == -1);

if ($ARGV[0] eq "-nospell") {
    shift;
    $nospell = 1;
}

$status = 0;

&dosetup;

for ($i = 0; $i <= $#ARGV; $i++) {
    $file = $ARGV[$i];
    die("$file does not exist or is not a file\n") if (!-f $file);
    if ($file =~ /\.(c|cpp)$/) {
        &checkSourceFile($file, 1);
        &checkSpelling($file) if (!$nospell);
    } elsif ($file =~ /\.(h|java)$/) {
        &checkSourceFile($file, 0);
        &checkSpelling($file) if (!$nospell);
    } elsif ($file =~ /\.(gmk|pl)$/ || $file =~ /^Makefile/) {
        &checkMakeFile($file);
    } elsif ($file =~ /\.o$/) {
        &checkObjectFile($file);
    }
}

exit $status;

#-------- End of main -----------
sub checkSpelling
{
    local ($fname) = pop;
    local ($tmp1) = "/tmp/$$.1.words";
    local ($tmp2) = "/tmp/$$.2.words";

    local (%keyword);

    %keyword = (
        "Freescale", 1,
        "MIDlet", 1,
        "Microsystems", 1,
        "Utf", 1,
        "code", 1,
        "const", 1,
        "endcode", 1,
        "endif", 1,
        "enum", 1,
        "ifdef", 1,
        "ifndef", 1,
        "inline", 1,
        "javax", 1,
        "kvm", 1,
        "malloc", 1,
        "memcpy", 1,
        "microedition", 1,
        "midlet", 1,
        "param", 1,
        "romized", 1,
        "romjava", 1,
        "sizeof", 1,
        "struct", 1,
        "typedef", 1,
        "utf", 1,

        "Last", 1
    );

    # Filter out include directives, then run cpp and filter out its markers.
    system("cat $fname|egrep -v '^#(include|if|else|endif)'> $tmp2");
    system("cpp $tmp2 |grep -v '^#' > $tmp1");

    open(PARAM_IN, $fname) || die ("Cannot find params");
    while ($line = <PARAM_IN>) {
        next if ($line !~ /\@param/);
        chomp($line);
        $line =~ s/.*\@param\s+([^\s]+).*/$1/;
        $params{$line} = 1;
    }
    close(PARAM_IN);

    # Collect the identifiers in the C code, so that we don't complain
    # about variable names.

    open(CCODE, $tmp1) || die("Cannot read $tmp1\n");
    @ccode = <CCODE>;
    close(CCODE);
    $ccode = join(' ', @ccode);
    @cwords = split(/\W+/, $ccode);
    foreach $word (@cwords) {
        $cwords{$word} = 1;
    }

    open(SPELL, "diff $tmp1 $tmp2|$spellCmd|") || die ("Cannot run spell\n");
    while ($line = <SPELL>) {
        chomp($line);
        next if ($cwords{$line});
        next if ($line =~ /^[A-Z][A-Z]/);
        next if ($line =~ /[a-z][A-Z]/);
        next if ($params{$line});
        next if ($keyword{$line});
        next if ($line =~ /^(mvm|md|ci|kvm|k_)[A-Za-z]/);
        print "Misspelled word? " . $line . "\n";
    }
    unlink($tmp1);
    unlink($tmp2);
    close(SPELL);
}

sub checkSourceFile
{
    local ($inC) = pop;
    local ($fname) = pop;
    local ($sawReturn);
    local ($indentDepth) = -1;
    local ($checkIndentOfNextLine) = 0;

    open(INP, $fname) || die("Cannot open $fname\n");


    undef %errors;

    
    for ($lineNumber = 1; $line = <INP>; $lineNumber++) {
        chomp($line);

        if ($indentDepth != -1) {
            # We are inside a multi-line comment 

            # Is this line properly indented?
            $expectedPrefix = (" " x $indentDepth);
            &recordError("CMNTCONT", $lineNumber)
                if (($line !~ /\t/) && ($line !~ /^\s$expectedPrefix\*/));
                    # Don't flag lines that contain tabs, that's separate
            
            $indentDepth = -1 if ($line =~ /\*\//); # Found closing of comment
            next;
        }
        if ($line =~ /^\s*\/\*/) {
            if ($line =~ /\*\//) {
                # Single line comments are allowed only if follow statements
                &recordError("SLC", $lineNumber);
            } else {
                # We are entering a multi-line comment

                if ($line =~ /\w/) {
                    # Prohibit
                    # /* text found here
                    #  */
                    &recordError("CMNT", $lineNumber);
                }
                $indentDepth = index $line,"/*"
                    if ($line !~ /\t/);         # TABS will confuse the count

                # Comment must be indented by multiples of 4 spaces
                &recordError("CMNTCONT", $lineNumber)
                    if (($indentDepth != -1) && (($indentDepth % 4) != 0));
            }
        }

	# Java/C++ style comments (i.e. //) are prohibited
        &recordError("JCO", $lineNumber)
            if ($line =~ /^\s*\/\// && ($line !~ /\/\/(ifdef|else|endif)/));

	# Line length must not exceed 80 chars
        &recordError("LEN", $lineNumber) if (length($line) > 80);

	# Lines must not start with tabs
        &recordError("TAB", $lineNumber) if ($line =~ /^\t/);

        # Commas separating arguments (e.g. in function parameters) must be
        # followed by a space.
	# Find commas missing the space, but not if they are inside
        # a char token (e.g. ',') or string (e.g. "abc,") or comment or 
        # cpp definition.
        &recordError("COM", $lineNumber) 
            if ($line =~ /,([^\s])/ && ($1 ne "'") && ($1 ne "\"") &&
                ($line !~ /^#/) && ($line !~ /^\s+\*/));

	# DOS line ends (^M or \r) are prohibited.
        &recordError("DOS", $lineNumber) if ($line =~ /\r/);

        # Open function parenthesis must not be on same line as function
        # prototype (C only)
        &recordError("OPENPAR", $lineNumber) 
                if ($inC && ($line =~ /^[A-z0-9_].*\(.*\).*{/));

        # Functions that don't take any parameters should use void,
        # as in "int foo(void)"
        &recordError("MISSINGVOID", $lineNumber) 
                if ($inC && ($line =~ /^[A-z0-9_].*\(\)/));

        # Goto labels must not have preceding white space
        &recordError("GOTOLABEL", $lineNumber) 
                if ($inC && ($line =~ /^\s+(\w+)\:/) && ($1 ne "default"));

        #&recordError("PAR", $lineNumber) 
        #    if (($line =~ /$condcond/o) && ($line !~ /$condOkEnd/o));

        &recordError("SPACEAFTERITERATOR", $lineNumber) 
                if ($line =~ /^[^\*]*(for|while)\(/);

        &recordError("EMPTY_LINE", $lineNumber) 
                if (($line eq "") && $sawEmptyLine);
        $sawEmptyLine = ($line eq "");

        
        &recordError("REDUNDANTRETURN", $lineNumber - 1) 
                if (($line =~ /^}$/) && $sawReturn);
        $sawReturn = ($line =~ /^\s+return;/) ne "";

        next if ($indentDepth != -1);

        # Check indentation level 
        if ($checkIndentOfNextLine && ($line !~ /\t/) && ($line !~ /^\s*$/)) {
            ($prefix) = ($line =~ /^([ ]*)/); # leave just leading w.space
            $depth = length($prefix);
            &recordError("INDENT", $lineNumber)
                if (($depth % 4) != 0);
        }
        $checkIndentOfNextLine = 
            # Don't check after ; because of for loop continuation lines 
            (($line =~ /^\s*$/) || ($line =~ /[{}]\s*$/) || 
                ($line =~ /^\s*\w+:\s*$/));
    }
    &dumpErrors($fname);
    close(INP);
}

sub checkMakeFile
{
    local ($fname) = pop;

    open(INP, $fname) || die("Cannot open $fname\n");

    $lineNumber = 0;
    while ($line = <INP>) {
        chomp($line);
        $lineNumber++;
        next if ($line !~ /#/);
        $line =~ s/.*#//;
        &recordError("LEN", $lineNumber) if (length($line) > 80);
        &recordError("DOS", $lineNumber) if ($line =~ /\r\n$/);
    }
    &dumpErrors($fname);
    close(INP);
}

sub checkObjectFile
{
    local ($fname) = pop;

    open(INP, "nm -g $fname|") || die("Cannot open $fname\n");

    while ($line = <INP>) {
        chomp($line);
        ($addr, $type, $symbol) = ($line =~ /(\w+)\s+([A-z])\s+(.*)/);
        if ($type eq "T") {
            print "$symbol, function, $fname,\n";
        } elsif ($type eq "D") {
            print "$symbol, variable, $fname,\n";
        }

    }
    close(INP);
}

# Accumulate errors for the current file. For each error type record
# the line numbers for the offending lines.

sub recordError
{
    $ln = pop;
    $type = pop;
    $errors{$type} .= "$ln, ";

    $status = 1;
}

sub dumpErrors
{
    $fname = pop;

    if ($msg = $errors{"INDENT"}) {
        print $fname .
            ": lines on which code is not properly indented - " . $msg . "\n"; 
    } 
#    if ($msg = $errors{"CMNTCONT"}) {
#        print $fname .
#            ": lines on which a comment is not properly indented (or missing *) - " . $msg . "\n"; 
#    } 
#    if ($msg = $errors{"CMNT"}) {
#        print $fname .
#            ": lines on which a comment start is followed by text - " . $msg . "\n"; 
#    } 
#    if ($msg = $errors{"COM"}) {
#        print $fname .
#            ": lines on which a ',' is missing the following space - " . $msg . "\n"; 
#    } 
    if ($msg = $errors{"PAR"}) {
        print $fname .
            ": lines on which a '{' is missing - " . $msg . "\n"; 
    } 
    if ($msg = $errors{"LEN"}) {
        print $fname .
            ": lines longer than 80 characters - " . $msg . "\n";
    } 
    if ($msg = $errors{"GOTOLABEL"}) {
        print $fname .
            ": lines containing white space before a goto label - " . 
            $msg . "\n";
    } 
    if ($msg = $errors{"OPENPAR"}) {
        print $fname .
            ": opening parenthesis on same line as function declaration - " . 
                $msg . "\n";
    } 
    if ($msg = $errors{"MISSINGVOID"}) {
        print $fname .
            ": functions not taking any parameters should use 'void' - " . 
                $msg . "\n";
    } 
    if ($msg = $errors{"TAB"}) {
        print $fname .
            ": lines starting with tab - " . $msg . "\n";
    } 
#    if ($msg = $errors{"SLC"}) {
#        print $fname .
#            ": lines containing only a single line comment - " . $msg . "\n";
#    } 
#    if ($msg = $errors{"JCO"}) {
#        print $fname .
#            ": lines containing a Java style single line comment - " . $msg . "\n";
#    } 
    if ($msg = $errors{"DOS"}) {
        print $fname .
            ": lines containing a ^M (i.e. \\r) - " . $msg . "\n";
    } 
    if ($msg = $errors{"REDUNDANTRETURN"}) {
        print $fname .
            ": lines containing a redundant return statement - " . $msg . "\n";
    } 
    if ($msg = $errors{"SPACEAFTERITERATOR"}) {
        print $fname .
            ": lines missing a space after 'for' or 'while' - " . $msg . "\n";
    } 
    if ($msg = $errors{"EMPTY_LINE"}) {
        print $fname .  ": redundant empty line - " . $msg . "\n";
    } 
}

sub dosetup
{
# Create regexp for catching lines that need to end with a '{'

    $list1 = "if|for|while";
    $list2 = "do|try|catch|synchronized";
    $condcond = '^\s+(((' . $list1 . ')\s+\(.*\)\s*)|(' . $list2 .  ')\s+)[^{]*$';
    $condOkEnd = '[=|&,<>(]\s*$';
    #print $condcond . "\n"; 
}

