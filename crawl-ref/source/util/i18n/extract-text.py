import glob
import re
import sys

SKIP_FILES = [ 
    # these just contain a bunch of compile flags, etc.
    'AppHdr.h', 'AppHdr.cc',
    'build.h', 'compflag.h',
    'version.h', 'version.cc',
    # json tags should not be translated
    'branch-data-json.h', 'branch-data-json.cc',
    'json.h', 'json.cc', 'json-wrapper.h',
    'tileweb.h', 'tileweb.cc', 'tileweb-text.h', 'tileweb-text.cc',
    # debug/test stuff
    'debug.h', 'ctest.h', 'ctest.cc', 'fake-main.cc',
    # files related to the translation process itself
    'xlate.h', 'xlate.cc', 
    'localize.h', 'localize.cc', 
    'database.h', 'database.cc',
    # stuff related to morgue file is not translated
    # (because if we run this on a server, we want all the morgues in English)
    'chardump.h', 'chardump.cc', 'notes.h', 'notes.cc',
    # nonsense
    'lang-fake.h', 'lang-fake.cc'
]

files = []
if len(sys.argv) > 1:
    # use list of files specified on command line
    files = sys.argv[1:]
else:
    # build my own list of files

    source_files = glob.glob("*.h")
    source_files.extend(glob.glob("*.cc"))

    # sort source files with .h files before matching .cc files
    for i in range(len(source_files)):
        source_files[i] = source_files[i].replace('.h', '.a')
    source_files.sort()
    for i in range(len(source_files)):
        source_files[i] = source_files[i].replace('.a', '.h')

    # put some important files first
    # (because if there are duplicate strings, we want them put under these files)
    files = [ 'stringutil.h', 'mon-data.h',
              'spl-data.h', 'zap-data.h',
              'item-prop.cc', 'item-name.cc', 'art-data.h', 
              'species-data.h', 'job-data.h', 'variant-msg.cc' ]

    # add wanted source files to list to be processed
    for fname in source_files:
        if fname not in files and \
           fname not in SKIP_FILES and \
           not re.match('dbg-', fname):
            files.append(fname)
    
output = []

for filename in files:

    strings = []

    with open(filename) as infile:
        data = infile.read()

        # remove (potentially) multi-line comments
        # NOTE: *? is a non-greedy match, and DOTALL allows dot to match newlines
        data = re.sub(r'/\*.*?\*/', '', data, 0, re.DOTALL)

        # join strings distributed over several lines
        data = re.sub(r'"\s*\n\s*"', '', data)

        # join function calls distributed over several lines
        # (because we are going to filter out certain function calls)
        data = re.sub(r',\s*\n\s*', ', ', data)
        data = re.sub(r':\s*\n\s*', ': ', data)
        
        skip = False
        for line in data.splitlines():

            # skip parts that are only included in DEBUG build
            if not skip:
                if re.match(r'\s*#\s*ifdef .*DEBUG', line) or \
                   re.match(r'\s*#\s*ifdef .*VERBOSE', line) or \
                   re.match(r'\s*#\s*if +defined *\(DEBUG', line):
                    skip = True
                    continue
            else:
                if re.match(r'\s*#\s*endif', line) or re.match(r'\s*#\s*else', line):
                    skip = False
                continue

            if '"' not in line:
                continue

            # ignore precompiler directives, except #define
            if re.match(r'^\s*#', line) and not re.match(r'^\s#\s*define', line):
                continue

            if re.search('extern +"C"', line):
                continue

            # ignore strings explicitly marked as not to be extracted
            if "noextract" in line:
                continue

            # ignore debug messages
            if re.search(r'\bdie *\(', line) or \
               re.search(r'dprf? *\(', line) or \
               re.search(r'debug_dump_item *\(', line) or \
               re.search(r'ASSERTM? *\(', line) or \
               'log_print' in line or \
               'MSGCH_DIAGNOSTICS' in line or \
               re.search(r'fprintf *\(', line):
                continue

            # ignore axed stuff
            if 'AXED' in line:
                continue

            # ignore file open (will have file name and mode as strings)
            if re.search(r'\bfopen(_u)? *\(', line):
                continue

            # ignore lua function calls
            if re.search(r'callfn *\(', line):
                continue

            # Leave notes/milsones in English
            if re.search('take_note', line) or re.search('mark_milestone', line):
                continue
            if re.search(r'\bmalmutate\s*\(', line):
                continue

            # ouch and hurt strings just go to score file
            if re.search(r'\bouch\s*\(', line) or re.search(r'\bhurt\s*\(', line):
                continue

            # skip tags/keys
            if re.search(r'^[^"]*_tag\(', line) and not re.search('text_tag', line):
                continue
            if re.search(r'strip_tag_prefix *\(', line):
                continue
            if re.search(r'json_write', line):
                continue
            if re.search(r'\bgetRandNameString *\(', line):
                continue
            if re.search(r'\bgetQuoteString *\(', line):
                continue;
            if re.search(r'\bgetSpeakString *\(', line):
                continue;
            if re.search(r'\bprops\.erase *\(', line):
                continue
            if re.search(r'_print_converted_orc_speech *\(', line):
                continue
            if re.search('^[^"]*property[A-Za-z_]* *\(', line):
                continue
            if re.match(r'^\s*key[A-Za-z_]*\.[A-Za-z_]*\(', line):
                continue

            # tokenize line into string and non-string
            tokens = []
            token = ""
            escaped = False
            in_string = False
            for i in range(len(line)):
                ch = line[i]
                if ch == '"' and not escaped:
                    if in_string:
                        token += ch
                        tokens.append(token)
                        token = ""
                        in_string = False
                    else:
                        if token != "":
                            tokens.append(token)
                        token = ch
                        in_string = True
                    continue
                
                if ch == '\\' and not escaped:
                    escaped = True
                else:
                    escaped = False
                
                if ch == '/' and i != 0 and line[i-1] == '/' and not in_string:
                    # comment
                    token = token[0:-1]
                    break;
    
                token += ch

            if token != "":
                tokens.append(token)
                        
            for i in range(len(tokens)):
                token = tokens[i]
                if len(token) < 3 or token[0] != '"' or token[-1] != '"':
                    continue;

                string = token[1:-1]
                if string in strings:
                    continue

                if i != 0:
                    last = tokens[i-1]

                    # skip (in)equality tests (assume string is defined elsewhere)
                    if re.search(r'[=!]=\s*$', last):
                        continue

                    # skip map keys
                    if re.search(r'\[\s*$', last):
                        continue

                    if '(' in last:
                        # another type of equality test
                        if re.search(r'\bstarts_with\s*\([^,"]+,\s*$', last):
                            continue

                strings.append(string)
            

    # filter out strings we want to ignore
    filtered_strings = []
    for string in strings:
        # ignore empty string
        if string == '':
            continue

        # ignore pronouns
        if string.lower() == 'you':
            continue

        # ignore strings that are just whitespace
        if re.match(r'^(\\n|\s)*$', string):
            continue

        # ignore opengl functions
        if re.match(r'^gl[A-Z]', string):
            continue
        
        # ignore HTML and formatted text tags
        if re.match(r'^(\\n|\s)*</?[^<>/]+>(\\n|\s)*$', string):
            continue

        # ignore variable names
        if re.match(r'^(\\n|\s)*@[A-Za-z0-9_]+@(\\n|\s)*$', string):
            continue

        # ignore identifiers
        if '_' in string and re.match(r'^[A-Za-z_]+$', string):
            continue
        if 'Gozag bribe' in string or 'Gozag permabribe' in string:
            continue

        # ignore filenames
        if re.match(r'^[A-Za-z]+\.[A-Za-z]+$', string):
            continue

        # ignore format strings without any actual text
        if re.match(r'^([^a-zA-Z]*%[0-9\.]*l{0,2}[a-zA-Z][^a-zA-Z]*)*$', string):
            continue

        filtered_strings.append(string)

    if len(filtered_strings) > 0:
        output.append("")
        output.append("##################")
        output.append("# " + filename)
        output.append("##################")
        for string in filtered_strings:
            # in some cases, string needs to be quoted
            #   - if it has leading or trailing whitespace
            #   - if it starts with # (because otherwise it looks like a comment)
            #   - if it starts and ends with double-quotes
            if re.search('^(\s|#)', string) or  re.search('\s$', string) \
               or (string.startswith(r'\"') and string.endswith('"')):
                string = '"' + string + '"'
            else:
                string = string.replace(r'\"', '"')
            string = string.replace(r'\\', '\\')

            if string in output:
                output.append('# duplicate: ' + string)
            else:
                output.append(string)

for line in output:
    print(line)

