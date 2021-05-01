import glob
import re
import sys

def strip_comment(line):
    escaped = False
    in_string = False
    for i in range(len(line)):
        ch = line[i]

        if ch == '\\' and not escaped:
            escaped = True
        else:
            escaped = False

        if ch == '"' and not escaped:
            in_string = not in_string
        elif ch == '/' and not in_string:
            if i > 0 and line[i-1] == '/':
                # comment
                return line[0:i-1]
    # no comment - return while line
    return line

def is_debug_if(line):
    return re.match(r'\s*#\s*ifdef .*DEBUG', line) or \
       re.match(r'\s*#\s*ifdef .*VERBOSE', line) or \
       re.match(r'\s*#\s*if +defined *\(DEBUG', line)

# strip out stuff that is excluded by #ifdef's, etc.
def strip_uncompiled(lines):
    skip = False
    result = []
    ifs = []

    for line in lines:

        if re.match('^\s*#', line):
            if re.match('^\s*#\s*if', line):
                ifs.append(line)
                # skip parts that are only included in DEBUG build
                if is_debug_if(line):
                    skip = True
                    continue
            elif re.match(r'\s*#\s*else', line):
                if is_debug_if(ifs[-1]):
                    skip = False
                    continue
            elif re.match(r'\s*#\s*endif', line):
                if_line = ifs.pop()
                if is_debug_if(if_line):
                    skip = False
                    continue

        if not skip:
            result.append(line)

    return result

SKIP_FILES = [ 
    # these just contain a bunch of compile flags, etc.
    'AppHdr.h', 'AppHdr.cc',
    'build.h', 'compflag.h',
    'version.h', 'version.cc',
    # json tags should not be translated
    'branch-data-json.h', 'branch-data-json.cc',
    'json.h', 'json.cc', 'json-wrapper.h',
    'tileweb.h', 'tileweb.cc', 'tileweb-text.h', 'tileweb-text.cc',
    # nor other tags
    'colour.h', 'colour.cc',
    # keys/properties
    'defines.h', 'dgn-layouts.h', 'dgn-layouts.cc',
    # debug/test stuff
    'debug.h', 'ctest.h', 'ctest.cc', 'fake-main.cc', 'coord-def.h',
    'crash.h', 'crash.cc', 'errors.h', 'errors.cc',
    # English grammar
    'english.h', 'english.cc',
    # files related to the translation process itself
    'xlate.h', 'xlate.cc', 
    'localise.h', 'localise.cc',
    'database.h', 'database.cc',
    # stuff related to morgue file is not translated
    # (because if we run this on a server, we want all the morgues in English)
    'chardump.h', 'chardump.cc', 'notes.h', 'notes.cc',
    # lua scripting stuff
    'clua.h', 'clua.cc', 'cluautil.h', 'cluautil.cc', 'dlua.h', 'dlua.cc',
    # internal logic
    'domino.h', 'domino.cc', 'dungeon.h', 'dungeon.cc',
    # nonsense
    'lang-fake.h', 'lang-fake.cc'
]

# These files are evaluated differently. We ignore all strings unless we have a reason to extract them,
# as opposed to extracting all strings unless we have a reason to ignore them.
LAZY_FILES = [
    'dgn-overview.cc', 'files.cc','fineff.cc'
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
              'spl-data.h', 'zap-data.h', 'feature-data.h',
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

    lazy = (filename in LAZY_FILES)

    strings = []

    with open(filename) as infile:
        data = infile.read()

        # remove (potentially) multi-line comments
        # NOTE: *? is a non-greedy match, and DOTALL allows dot to match newlines
        data = re.sub(r'/\*.*?\*/', '', data, 0, re.DOTALL)

        lines_raw = strip_uncompiled(data.splitlines())
        lines = []

        # join split lines and remove comments
        for line in lines_raw:

            # ignore strings explicitly marked as not to be extracted
            if 'noextract' in line or 'noloc' in line:
                continue

            # remove comment
            if '//' in line and not re.search('// *localise', line):
                line = strip_comment(line)

            line = line.strip()
            if line == '':
                continue

            if len(lines) > 0:        
                last = lines[-1]

                # join strings distributed over several lines
                if line[0] == '"' and last[-1] == '"':
                    lines[-1] = last[0:-1] + line[1:]
                    continue

                # join function calls split over multiple lines (because we want to filter out some function calls)
                if last[-1] == ',' or last[-1] == ':' or last[-1] == '?' or last[-1] == '(' or \
                     line[0] == ',' or line[0] == ':' or line[0] == '?':
                    lines[-1] = last + line
                    continue

            lines.append(line)

        
        for line in lines:

            if '"' not in line:
                continue

            extract = False

            if 'localise' in line:
                extract = True
            elif 'simple_monster_message' in line or 'simple_god_message' in line:
                extract = True
            elif re.search(r'mpr[a-zA-Z_]* *\(', line):
                # extract mpr, mprf, etc. messages, unless they are diagnostics
                if 'MSGCH_DIAGNOSTICS' in line:
                    continue
                else:
                    extract = True
            elif re.search(r'prompt[a-zA-Z_]* *\(', line) or 'yesno' in line:
                # extract prompts
                extract = True
            elif re.match(r'\s*end *\(', line):
                extract = True
            elif re.search(r'\bsave_game *\(', line):
                extract = True
                
            if lazy:
                # ignore strings unless we have a specific reason to extract them
                if not extract:
                    continue 

            # we don't want to extract the db key used with getSpeakString(), etc.,
            # but we don't necessarily want to ignore the whole line because
            # sometimes there are other strings present that we do want to extract
            if re.search(r'\bget[a-zA-Z]*String', line):
                line = re.sub(r'\b(get[a-zA-Z]*String) *\(.*\)', r'\1()', line)

                if '"' not in line:
                    continue


            if not extract:
                # if we get here then we are not in lazy mode
                # extract strings unless we have reason to ignore them

                # ignore precompiler directives, except #define
                if line[0] == '#' and not re.match(r'^#\s*define', line):
                    continue

                if re.search('extern +"C"', line):
                    continue

                # ignore debug messages
                if re.search(r'\bdie(_noline)? *\(', line) or \
                   re.search(r'dprf? *\(', line) or \
                   re.search(r'dprintf *\(', line) or \
                   re.search(r'debug_dump_item *\(', line) or \
                   re.search(r'ASSERTM? *\(', line) or \
                   'log_print' in line or \
                   re.search(r'fprintf *\(', line):
                    continue

                # ignore axed stuff
                if 'AXED' in line:
                    continue

                # ignore file open (will have file name and mode as strings)
                if re.search(r'\bfopen(_u)? *\(', line):
                    continue

                # ignore lua function calls
                if re.search(r'call[a-zA-Z]*fn *\(', line):
                    continue

                # Leave notes/milsones in English
                if re.search('take_note', line) or re.search('mark_milestone', line):
                    continue
                if re.search(r'mutate\s*\(', line):
                    continue

                # ouch and hurt strings just go to score file
                if re.search(r'\bouch\s*\(', line) or re.search(r'\bhurt\s*\(', line):
                    continue

                # skip tags/keys
                if re.search(r'^[^"]*_tag\(', line) and not re.search('text_tag', line):
                    continue
                if re.search(r'strip_tag_prefix *\(', line):
                    continue
                if 'json_' in line:
                    continue
                if 'push_ui_layout' in line:
                    continue
                if re.search(r'\bprops\.erase *\(', line):
                    continue
                if '_print_converted_orc_speech' in line:
                    continue
                if 'show_specific_help' in line:
                    continue
                if re.search('^[^"]*property[A-Za-z_]* *\(', line):
                    continue
                if re.match(r'^\s*key[A-Za-z_]*\.[A-Za-z_]*\(', line):
                    continue

                # just a find
                if re.match(r'\bstrstr\s*\(', line):
                    continue
                if 'search_stashes' in line:
                    continue

            extract = True

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
                        if re.search(r'\bfind\s*\(\s*(string\()?$', last):
                            continue
                        if re.search(r'\bexists\s*\(\s*$', last):
                            continue

                        if re.search(r'\bstrip_suffix\s*\(', last):
                            continue
                        if re.search(r'\bsend_exit_reason\s*\(', last):
                            continue
                        if re.search(r'\bsend_dump_info\s*\(', last):
                            continue
                        if re.search(r'\bmons_add_blame\s*\(', last):
                            continue

                # simple_god/monster_message may contain an implied %s
                if 'simple_god_message' in line or 'simple_monster_message' in line:
                    if string != "" and (string[0] == " " or string[0] == "'"):
                        string = '%s' + string

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
        if '_' in string and re.match(r'^[A-Za-z_-]+$', string):
            continue
        if 'Gozag bribe' in string or 'Gozag permabribe' in string:
            continue

        # ignore filenames
        if re.match(r'^[A-Za-z_]+\.[A-Za-z]+$', string):
            continue

        # ignore format strings without any actual text
        if re.match(r'^(\\n)*([^a-zA-Z]*%[0-9\.\*]*l{0,2}[a-zA-Z][^a-zA-Z]*)*(\\n)*$', string):
            continue

        # ignore punctuation
        if re.match(r'^[!\.\?]+$', string):
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

