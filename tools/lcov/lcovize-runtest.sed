#!/bin/sed -f

s/^\([^#[:blank:]]\+\)\([[:blank:]]\+\)\([^#]*\>[^#;]*\)\([#;]*.*\)/\
# Original line: \1\2\3\4\
# Instrumented line:\
\1 lcov-reset.sh; \3; lcov-capture.sh \1;/g
