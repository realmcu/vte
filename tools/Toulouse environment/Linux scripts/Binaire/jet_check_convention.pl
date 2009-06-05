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

# ==================================================================
# Main
# ==================================================================

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

    print "---" . $file . "\n";

    if ($file =~ /\.(c|cpp)$/) {
        &checkSourceFile($file, 1);
        &checkUnixFormat($file);
        &checkSpelling($file) if (!$nospell);
    } elsif ($file =~ /\.(h|java)$/) {
        &checkSourceFile($file, 0);
        &checkUnixFormat($file);
        &checkSpelling($file) if (!$nospell);
    } elsif ($file =~ /\.(gmk|pl)$/ || $file =~ /^Makefile/ || $file =~ /^makefile/) {
        &checkMakeFile($file);
        &checkUnixFormat($file);
    } else {
        &checkUnixFormat($file);
    }
}

exit $status;

#-------- End of main -----------

# ==================================================================
# Check the spelling
# ==================================================================
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

# ==================================================================
# check the source file:
#    indentation, tab/space, end of lines, ...
# ==================================================================
sub checkSourceFile
{
    local ($inC) = pop;
    local ($fname) = pop;
    local ($sawReturn);
    local ($indentDepth) = -1;
    local ($checkIndentOfNextLine) = 0;

    open(INP, $fname) || die("Cannot open $fname\n");


    undef %errors;

    
    for ($lineNumber = 1; $line = <INP>; $lineNumber++)
    {
        chomp($line);

	# Lines must not start with tabs (except for Java files)
        # ------------------------------------------------------
        if ($fname !~ /\.java/)
        {
            &recordError("TAB", $lineNumber) if ($line =~ /^\t/);
        }

        # Open function parenthesis must not be on same line as function
        # prototype (C only)
        # ---------------------------------------------------------------
        &recordError("OPENPAR", $lineNumber) 
                if ($inC && ($line =~ /^[A-z0-9_].*\(.*\).*{/));

        # Functions that don't take any parameters should use void,
        # as in "int foo(void)"
        # ---------------------------------------------------------
        &recordError("MISSINGVOID", $lineNumber) 
                if ($inC && ($line =~ /^[A-z0-9_].*\(\)/));


        next if ($indentDepth != -1);

        # Check indentation level 
        # ------------------------
        if ($checkIndentOfNextLine && ($line !~ /\t/) && ($line !~ /^\s*$/))
        {
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

# ==================================================================
# Check the Makefiles
# ==================================================================
sub checkMakeFile
{
    local ($fname) = pop;

    open(INP, $fname) || die("Cannot open $fname\n");

    $lineNumber = 0;
    while ($line = <INP>)
    {
        chomp($line);
        $lineNumber++;
        next if ($line !~ /#/);
        $line =~ s/.*#//;
#        &recordError("LEN", $lineNumber) if (length($line) > 80);
        &recordError("DOS", $lineNumber) if ($line =~ /\r\n$/);
    }
    &dumpErrors($fname);
    close(INP);
}


# ==================================================================
# check the file format. It must be Unix not to raise an error.
# ==================================================================
sub checkUnixFormat
{
    local ($fname) = pop;

    open(INP, $fname) || die("Cannot open $fname\n");


    for ($lineNumber = 1; $line = <INP>; $lineNumber++)
    {
        chomp($line);

	# DOS line ends (^M or \r) are prohibited.
        # ----------------------------------------
        &recordError("DOS", $lineNumber) if ($line =~ /\r/);
    }
}


# ==================================================================
# Accumulate errors for the current file. For each error type record
# the line numbers for the offending lines.
# ==================================================================
sub recordError
{
    $ln = pop;
    $type = pop;
    $errors{$type} .= "$ln, ";

    $status = 1;
}

# ==================================================================
# Dump the errors
# ==================================================================
sub dumpErrors
{
    $fname = pop;

    if ($msg = $errors{"INDENT"}) {
        print $fname .
            ": lines on which code is not properly indented - " . $msg . "\n"; 
    } 
    if ($msg = $errors{"PAR"}) {
        print $fname .
            ": lines on which a '{' is missing - " . $msg . "\n"; 
    } 
    if ($msg = $errors{"LEN"}) {
        print $fname .
            ": lines longer than 80 characters - " . $msg . "\n";
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
    if ($msg = $errors{"DOS"}) {
        print $fname .
            ": lines containing a ^M (i.e. \\r) - " . $msg . "\n";
    } 
}

# ==================================================================
# Setup
# ==================================================================
sub dosetup
{
# Create regexp for catching lines that need to end with a '{'

    $list1 = "if|for|while";
    $list2 = "do|try|catch|synchronized";
    $condcond = '^\s+(((' . $list1 . ')\s+\(.*\)\s*)|(' . $list2 .  ')\s+)[^{]*$';
    $condOkEnd = '[=|&,<>(]\s*$';
    #print $condcond . "\n"; 
}

