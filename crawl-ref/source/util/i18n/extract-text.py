################################################################################
#
# This script extracts strings from C++ (and Lua and .des) source code
#
# By default, all strings are extracted, unless there's a reason to ignore them.
# However for files named in LAZY_FILES, strings are only extracted if
# there's an explicit reason to do so.
#
# It understands certain directives placed in single-line comments:
#   @noloc = do not extract strings on this line
#   @noloc section start = stop extracting strings from this line onwards
#   @noloc section end = resume extracting strings
#   @localise = DO extract strings on this line, even if in a noloc section or lazy
#               file (not required if there's a call to the localise() function)
#
# These strings are always ignored:
#   - map keys (but not values)
#   - anything that looks like a key or identifier
#   - tags of the form "<foo>" (on their own)
#   - file names
#
################################################################################

import glob
import re
import sys

# pattern for recognising strings
# handles escaped double-quotes
STRING_PATTERN = r'"(\\\\|\\"|[^"])*"'

PRAY_SENTENCE = "You %s the altar of %s."
RU_SACRIFICE_PREFIX = "Ru asks you to "

# strings to ignore
IGNORE_STRINGS = [
    # partial strings
    'The ', 'the', 'the ', ' the ', 'la ', 'the %s ',
    'a', 'a ', 'an', 'an ', 'a %s ', 'no ',
    'Your ', 'your', 'your ', 'its ',
    'You ', 'you ', ' you',
    ' of ', ' of', 'of ', 's', 'in ', ' by ',
    ' ghost', ' illusion', '-headed ', ' beast',
    '%sand %s', "on level %d of ", "between levels %d and %d of ",
    # debug/error stuff
    'debugging ray', 'debug', 'bugger',
    'bug', 'null', 'invalid', ' (holding nobody)', 'Yak', 'Un',
    'DEAD MONSTER', 'STAIR BEAM', 'Dummy Monster', 'John Doe', 'Unemployed',
    'You hear the sound of one hand!', "Failed to create item '",
    'Missing', 'missing status', 'Missing status description.',
    '(error object is not a string)',
    'very very heavily contaminated', 'impossibly contaminated',
    # suffixes for walking verb
    'ing', 'er',
    # property keys
    'Brand', 'BAcc', 'BDam', 'nupgr', 'cap-',
    'true', 'false', 'veto',
    # other keys
    'known-menu', 'freeform', 'highlighter', 'pick-up',
    'majin-bo cast weak', 'majin-bo cast',
    # text colour tags
    'lightgrey', 'darkgrey', 'lightgray', 'darkgray', 'lightgreen', 'darkgreen',
    'lightcyan', 'darkcyan', 'lightred', 'darkred', 'lightmagenta', 'darkmagenta',
    'lightyellow', 'darkyellow', 'w',
    # stuff that is used to build expanded strings
    RU_SACRIFICE_PREFIX,
    PRAY_SENTENCE,
    # notes
    'god gift: %s', 'HP: %d/%d MP: %d/%d', 'something (%d)', ' (+ monsters)',
    '%d fountains blood', '%d doors open', '%d doors close',
    'Cast into level %d of the Abyss', 'smitten by ',
    # No need for explicit translation for these. Handled as if
    # they were random artefacts.
    r'quick blade \"Gimble\"', r'quick blade \"Gyre\"',
    # other
    '<w>a:</w> ', '<w>A:</w> ', '[<w>XXX</w>]</lightgrey>',
    'top', 'bot',
]

# These files need special handling because they define data structures
# containing strings (normally names of things)
# (stringutil.h is an exception - it contains strings related to list building)
SPECIAL_FILES = [
    'stringutil.h', 'mon-data.h',
    'spl-data.h', 'zap-data.h', 'feature-data.h',
    'item-prop.cc', 'item-name.cc',
    'art-data.txt', # art-data.h generated from art-data.txt
    'job-data.h', 'form-data.h'
]

# These files are evaluated differently. We ignore all strings unless we have a reason to extract them,
# as opposed to extracting all strings unless we have a reason to ignore them.
LAZY_FILES = [
    'dgn-overview.cc', 'delay.h', 'end.cc', 'files.cc','fineff.cc',
    'god-passive.cc', 'god-prayer.cc', 'macro.cc', 'main.cc'
]

SKIP_FILES = [
    # art-data.h generated from art-data.txt
    'art-data.h',
    # generated from yaml files
    'species-data.h',
    # these just contain a bunch of compile flags, etc.
    'AppHdr.h', 'AppHdr.cc',
    'build.h', 'compflag.h',
    'version.h', 'version.cc',
    # json tags should not be translated
    'branch-data-json.h', 'branch-data-json.cc',
    'json.h', 'json.cc', 'json-wrapper.h',
    'tileweb-text.h', 'tileweb-text.cc',
    # nor other tags
    'colour.h', 'colour.cc', 'format.h', 'format.cc',
    # keys/properties
    'defines.h', 'dgn-layouts.h', 'dgn-layouts.cc', 'god-abil.h', 'initfile.cc',
    'libunix.h', 'libunix.cc', 'libutil.h', 'libutil.cc', 'mgen-data.h',
    'mi-enum.h', 'mon-abil.h', 'mon-clone.h', 'mon-speak.cc', 'monster.h',
    'religion-enum.h',
    # debug/test stuff
    'debug.h', 'ctest.h', 'ctest.cc', 'fake-main.cc', 'coord-def.h',
    'crash.h', 'crash.cc', 'errors.h', 'errors.cc',
    # English grammar
    'english.h', 'english.cc',
    # files related to the translation process itself
    'xlate.h', 'xlate.cc',
    'localise.h', 'localise.cc',
    'database.h', 'database.cc', 'sqldbm.cc',
    # stuff related to morgue file is not translated
    # (because if we run this on a server, we want all the morgues in English)
    'chardump.h', 'chardump.cc', 'kills.h', 'kills.cc', 'notes.h', 'notes.cc',
    # lua scripting stuff
    'clua.h', 'clua.cc', 'cluautil.h', 'cluautil.cc', 'dlua.h', 'dlua.cc',
    # internal logic
    'domino.h', 'domino.cc', 'dungeon.h', 'dungeon.cc', 'mapdef.h', 'mapdef.cc',
    'mapmark.h', 'mapmark.cc', 'maps.h', 'maps.cc', 'mon-gear.h', 'mon-gear.cc',
    'mon-pathfind.cc', 'ng-init.cc', 'ng-input.cc', 'precision-menu.h',
    'precision-menu.cc', 'randbook.h', 'randbook.cc', 'sound.h', 'sound.cc',
    'tilepick.cc', 'viewchar.h', 'viewchar.cc',
    # error messages that probably aren't worth translating
    'fontwrapper-ft.cc', 'game-options.h', 'game-options.cc', 'glwrapper-ogl.cc',
    'libw32c.h', 'libw32c.cc', 'package.cc', 'windowmanager-sdl.cc',
    # utils
    'stringutil.cc', 'syscalls.h', 'syscalls.cc', 'ui.h', 'ui.cc',
    'unicode.h', 'unicode.cc',
    # nonsense
    'lang-fake.h', 'lang-fake.cc',
    # dump file stuff
    'dat/clua/kills.lua',
    # simple messaging - I don't think this is actually used, even though it's built into webtiles
    'dgl-message.h', 'dgl-message.cc',
]

form_attack_verbs = []
medium_attack_verbs = []


############################
# General utility functions
############################

def replace_last(s, old, new):
    return new.join(s.rsplit(old, 1))

def remove_duplicates(strings):
    result = []
    for string in strings:
        if string.startswith('#') or string not in result:
            result.append(string)
        elif len(result) > 0 and result[-1].startswith("# note:"):
            # remove note associated with removed string
            result.pop()
    return result

# remove html-style tags
def remove_tags(string):
    return re.sub('<[^>]*>', '', string)

def dump_lines(filename, lines):
    print("------------------------------------------", file=sys.stderr)
    print(filename, file=sys.stderr)
    print("------------------------------------------", file=sys.stderr)
    for line in lines:
        print(line, file=sys.stderr)

################################
# Grammatical utility functions
################################

def has_article_the(string):
    return re.match("^the ", string, flags=re.IGNORECASE)

def has_article_a(string):
    return re.match("^(a|an) ", string, flags=re.IGNORECASE)

def remove_article(string):
    return re.sub("^(a|an|the) ", "", string, flags=re.IGNORECASE)

def article_a(string):
    string = remove_article(string)
    if re.search('^[aeiouAEIOU]', string) and not string.startswith('one-'):
        return "an " + string
    else:
        return "a " + string

def article_the(string):
    return "the " + remove_article(string)

def possessive(string):
    return string + "'s"

# pluralise a string - replicates the logic of the equivalent function in english.cc
def pluralise(string):
    # if it's something like "potion of healing" then we want "potions of healing", not "potion of healings"
    # so separate the suffix, pluralise the main noun, then put the suffix back
    pos = string.find(" of ")
    if pos < 0:
        pos = string.find(" from ")
    if pos < 0:
        pos = string.find(" labelled ")
    if pos >= 0:
        prefix = string[0:pos]
        suffix = string[pos:]
        return pluralise(prefix) + suffix

    if string.endswith("us"):
        if string.endswith("lotus") or string.endswith("status"):
            return string + "es"
        else:
            return string[:-2] + "i";
    elif string.endswith("ex"):
        return string[:-2] + "ices"
    elif string.endswith("mosquito") or string.endswith("ss"):
        return string + "es"
    elif string.endswith("cyclops"):
        return string[:-1] + "es"
    elif string.endswith("catoblepas"):
        return string[:-1] + "e"
    elif string.endswith("s"):
        return string
    elif string.endswith("y") and not string.endswith("ey"):
        return string[:-1] + "ies"
    elif string.endswith("staff"):
        return string[:-2] + "ves"
    elif string.endswith("f") and not string.endswith("ff"):
        return string[:-1] + "ves"
    elif string.endswith("mage") and not string.endswith("damage"):
        return string[:-1] + "i"
    elif re.search('(gold|fish|folk|spawn|tengu|sheep|swine|efreet|jiangshi|raiju|meliai)$', string):
        return string
    elif re.search('(ch|sh|x)$', string):
        return string + "es"
    elif re.search('(simulacrum|eidolon)$', string):
        return string[:-2] + "a"
    elif string.endswith("djinni"):
        return string[:-1]
    elif string.endswith("foot"):
        return string[:-4] + "feet"
    elif re.search('(ophan|cherub|seraph)$', string):
        return string + "im"
    elif string.endswith("arachi"):
        return string + "m"
    elif string.endswith("ushabti"):
        return string + "u"
    elif string.endswith("mitl"):
        return string[:-2] + "meh"
    else:
        return string + "s"

# conjugate verb for 3rd person singular (he/she/it)
def conjugate_verb(verb_phrase):
    parts = verb_phrase.split(' ', 1)
    verb = parts[0]
    suffix = ''
    i = 1
    while verb.endswith('ly') and i < len(parts):
        verb += ' ' + parts[i]
        i += 1
    while i < len(parts):
        suffix += ' ' + parts[i]
        i += 1

    if verb == "be" or verb == "are":
        return "is" + suffix
    elif verb.endswith('ss') or verb.endswith('sh') or verb.endswith('ch'):
        return verb + 'es' + suffix
    else:
        return verb + 's' + suffix

def do_any_2_actors_message(verb, suffix):
    verb3p = conjugate_verb(verb)
    strings = []
    strings.append("You " + verb + " %s" + suffix)
    strings.append("%s " + verb3p + " you" + suffix)
    strings.append("%s " + verb3p + " %s" + suffix)
    strings.append("%s " + verb3p + " itself" + suffix)
    return strings

def do_any_2_actors_messages(verbs, suffixes):
    temp = []
    for idx, verb in enumerate(verbs):
        suffix = suffixes[idx] if len(suffixes) > idx else ''
        temp.append(do_any_2_actors_message(verb, suffix))

    # group by person
    results = []
    for i in range(len(temp[0])):
        for j in range(len(temp)):
            results.append(temp[j][i])

    return results


################################
# String extraction functions
################################

# should string be ignored?
def ignore_string(string):

    # check explicit ignore list
    if string in IGNORE_STRINGS:
        return True

    without_tags = remove_tags(string)

    # ignore strings without alpha-numeric characters
    if not re.search(r'[A-Za-z0-9]', without_tags):
        return True

    # This can't be changed, and we don't want trasnlators to think it can
    if without_tags.strip() == "[a-z]":
        return True

    # the name of the game
    if string.startswith('Crawl'):
        return True

    # ignore opengl functions
    if re.match(r'^gl[A-Z]', string):
        return True

    # ignore variable names like @foo@
    if re.match(r'^\s*@[A-Za-z0-9_]+@?\s*$', string):
        return True

    # ignore identifiers
    if '_' in string and re.match(r"^[A-Za-z0-9_\- ']+$", string):
        return True
    if 'Gozag bribe' in string or 'Gozag permabribe' in string:
        return True
    if string == 'passage of golubria': # display name has uppercase G
        return True
    if string.startswith('fake') or string.startswith('FAKE'):
        return True

    # ignore bug-catching stuff
    if 'INVALID' in string or 'DUMMY' in string or 'eggplant' in string:
        return True
    if re.search('bug', string, re.I) and 'bug-like' not in string \
       and 'bug report' not in string and 'program bug' not in string \
       and not re.search('debug', string, re.I):
        return True

    # ignore debug stuff
    if 'gdb' in string or 'Git' in string:
        return True

    # ignore filenames and file extensions
    if re.match(r'^[A-Za-z0-9_\-\/]*\.[A-Za-z]{1,4}$', string):
        return True

    # ignore format strings without any actual text
    temp = re.sub(r'%[\-\+ #0]?[\*0-9]*(\.[\*0-9]*)?(hh|h|l|ll|j|z|t|L)?[diuoxXfFeEgGaAcspn]', '', without_tags)
    temp = re.sub('0x', '', temp); # Hexadecimal number indicator
    if not re.search(r'(?<!\\)[a-zA-Z]', temp):
        return True

    return False


IGNORE_SECTIONS = {
    'abyss.cc':         ['_abyss_grid'],        # dungeon generation
    'acquire.cc':       [
        '_hyphenated_letters',                  # hyphenate aquirement option letters
        '_why_reject'                           # debug messages
    ],
    'arena.cc':         ['arena_tee'],          # arena dumpfile stuff
    'art-func.h':       ['_SINGING_SWORD_melee_effects'], # internal keys
    'artefact.cc':      ['replace_name_parts'],
    'attack.cc':        ['chaos_effects'],      # chaos effect names are only used for debugging
    'beam.cc':          ['enchant_monster_invisible'], # we expand strings at the point of calling
    'command.cc':       [
        # diagnostic stuff
        'features', '_get_version_information', '_get_version_features', '_get_version_changes',
        '_add_movement_diagram',                # same for all languages
        '_get_help_section',                    # section header text that is never used
    ],
    # internal identifiers
    'delay.cc':         ['activity_interrupt_names'],
    'describe.cc':      [
        'get_command_description',              # lookup key
        'trap_name',                            # for interpreting vault spec
    ],
    'dgn-shoals.cc':    ['dgn_build_shoals_level'], # dungeon builder stuff
    'god-abil.cc':      [
        '_god_blessing_description',            # milestones
        '_gozag_shop_spec',                     # internal identifiers
    ],
    'god-conduct.cc':   ['conducts'],           # debug
    'hints.cc':         [
        '_replace_static_tags',                 # error messages for translator, not end user
        '_hints_target_mode',                   # literal commands (and % placeholder)
    ],
    'hiscores.cc':      [
        'kill_method_names',                    # scorefile stuff (never displayed)
        '_job_name', '_job_abbrev',             # obsolete jobs (backgrounds)
        '_species_name', '_species_abbrev',     # obsolete species
        '_deconstruct_shooter_phrase',          # search strings
        'scorefile_entry::terse_missile_name',  # search strings
        'scorefile_entry::set_base_xlog_fields', # internal ids
    ],
    'jobs.cc':          ['debug_jobdata'],      # debug
    'message.cc':       [
        # this function adds a prefix to the message parameter
        # this script will add it when extracting the message at the point of call
        'wu_jian_sifu_message',
    ],
    'misc.cc':          ['maybe_to_string'],    # debug
    'mon-death.cc':     [
        '_milestone_kill_verb',                 # only used for milesone
        '_killer_type_name',                    # internal keys
    ],
    'mon-ench.cc':      [
        # debug
        'enchant_names', 'mon_enchant::operator_string',
        'mon_enchant::kill_category_desc',
    ],
    'mon-info.cc':      ['_monster_list_colour_names'],     # internal identifiers
    'mon-place.cc':     ['debug_bands'],        # debug
    'mon-util.cc':      [
        'holiness_name',                        # only used in LUA
        'mons_type_name',                       # names for "random" monsters
        '_get_species_insult', 'get_mon_shape_str',  # db keys
        'debug_mondata', 'debug_monspells',   #debug
    ],
    'monster.cc':       [
        '_invalid_monster_str',                 # debugging stuff
    ],
    'output.cc':        [
        'mpr_monster_list',                     # only used in morgue and for debugging
        '_itosym',                              # symbolic representaion of resist numbers
    ],
    'xom.cc':           [
        '_note_potion_effect',                  # milestones
        '_get_death_type_keyword', 'xom_events', # internal keys
        # debug stuff
        'validate_xom_events', '_list_exploration_estimate', 'debug_xom_effects',
    ],
}

# should section be ignored?
def ignore_section(filename, section):
    if 'milestone' in section:
        return True
    elif filename in IGNORE_SECTIONS and section in IGNORE_SECTIONS[filename]:
        return True
    elif filename == 'lookup-help.cc':
        if re.match(r'^_(get|recap)[a-z_]*keys?$', section):
            # db keys
            return True

    return False


# process art-data.txt
def process_art_data_txt():
    infile = open('art-data.txt')
    data = infile.read()
    infile.close()

    result = []
    lines = data.splitlines()
    name = ''
    desc = None
    brand_desc = None
    has_appearance = False
    for line in lines:
        if line.startswith('#'):
            continue
        elif line.startswith(' '):
            if desc is not None:
                desc += line
            elif brand_desc is not None:
                brand_desc += line
            continue

        if desc is not None:
            result.append('# note: description for ' + name)
            result.append(desc)
            desc = '' if line.startswith('+') else None

        if brand_desc is not None:
            result.append('# note: brand description for ' + name)
            result.append(brand_desc)
            brand_desc = '' if line.startswith('+') else None

        if line.startswith('+'):
            if desc is not None:
                desc += line[1:].strip()
            elif brand_desc is not None:
                brand_desc += line[1:].strip()
        elif line.startswith('NAME:'):
            has_appearance = False
            name = line.replace('NAME:', '').strip()
            if 'DUMMY' in name:
                continue
            #result.append('# note: ' + name)
            if re.search('(boots|gloves|gauntlets|quick blades)', name):
                if not 'pair of ' in name:
                    name = 'pair of ' + name
            result.append(article_the(name))
        elif 'DUMMY' in name:
            continue
        elif line.startswith('APPEAR:'):
            string = line.replace('APPEAR:', '').strip()
            result.append('# note: appearance of ' + name + " before it's identified")
            result.append(article_the(string))
        elif line.startswith('TYPE:'):
            string = line.replace('TYPE:', '').strip()
            result.append('# note: base type of ' + name)
            result.append(article_the(string))
        elif line.startswith('INSCRIP:'):
            string = line.replace('INSCRIP:', '').strip()
            if string.endswith(','):
                string = string[0:-1]
            result.append('# note: annotation for ' + name)
            result.append(string)
        elif line.startswith('DESCRIP:'):
            desc = line.replace('DESCRIP:', '').strip()
        elif line.startswith('DBRAND:'):
            brand_desc = line.replace('DBRAND:', '').strip()

    return result

def extract_strings(data):
    strings = []
    quotes = False
    for c in data:
        if c == '"':
            if quotes:
                quotes = False
            else:
                quotes = True
                strings.append('')
        elif quotes:
            strings[-1] += c
    return strings

# split on commas, but not if they're inside quotes or brackets
def safe_tokenize(string):
    if len(string) == 0:
        return []
    fields = ['']
    quotes = 0
    curlies = 0
    rounds = 0
    for c in string:
        if c == '{':
            curlies += 1
        elif c == '}':
            curlies -= 1
        elif c == '(':
            rounds += 1
        elif c == ')':
            rounds -= 1
        elif c == '"':
            quotes = 1 if quotes == 0 else 0

        if curlies < 0 or rounds < 0:
            break

        if c == ',' and quotes == 0 and curlies == 0 and rounds == 0:
            # new field
            fields.append('')
        elif len(fields) > 0:
            fields[-1] += c

    for i in range(len(fields)):
        fields[i] = fields[i].strip()

    return fields

def remove_enclosing_curlies(string):
    string = re.sub(r'^\{', '', string)
    string = re.sub(r'\};?$', '', string)
    return string

def remove_enclosing_quotes(string):
    if string.startswith('"') and string.endswith('"'):
        return string[1:-1]
    else:
        return string

# tokenize a C++ data file
# Return an array of arrays, where each element in the outer array is an entry
# and each element in the inner array is a field within that entry
def tokenize_cplusplus_data_file(filename):
    lazy = (filename in LAZY_FILES)

    # remove comments and sections that are excluded by preprocessor directives
    lines = get_cleaned_file_contents(filename)

    data = ""
    started = False

    for line in lines:
        if "static const" in line and "=" in line:
            line = re.sub('^[^=]*=', '', line)
            started = True
        if not started:
            continue
        data += line.strip()

    # remove enclosing curlies
    data = remove_enclosing_curlies(data)

    entries = safe_tokenize(data)
    results = []
    for e in entries:
        e = remove_enclosing_curlies(e)
        fields = safe_tokenize(e)
        results.append(fields)
        #print('# ' + ', '.join(fields))

    return results


def process_form_data_h(filename):
    entries = tokenize_cplusplus_data_file(filename)

    results = []
    for fields in entries:
        for i in range(len(fields)):
            strings = extract_strings(fields[i])
            if len(strings) == 0:
                if 'ANIMAL_VERBS' in fields[i]:
                    # TODO: remove hard-coding
                    strings = ["hit", "bite", "maul", "maul"]
                else:
                    continue
            if i == 5:
                # description - remove punctuation
                if strings[0].endswith(".") or strings[0].endswith("!"):
                    strings[0] = strings[0][:-1]
                results.extend(strings)
            elif i == 23:
                # attack verbs
                form_attack_verbs.extend(strings)
                if len(strings) > 1:
                    medium_attack_verbs.append(strings[1])
                else:
                    medium_attack_verbs.append(strings[0])
            elif i == 32:
                # prayer action
                if strings[0] != "":
                    string = PRAY_SENTENCE.replace("%s", strings[0], 1)
                    results.append(string)
            else:
                results.extend(strings)

    return results

def process_sacrifice_data_h(filename):
    entries = tokenize_cplusplus_data_file(filename)

    results = []
    for fields in entries:
        has_param = len(fields) > 6 and fields[6] != "nullptr"
        for i in range(len(fields)):
            strings = extract_strings(fields[i])
            if len(strings) == 0:
                continue
            if i == 2:
                # sacrifice message
                string = RU_SACRIFICE_PREFIX + strings[0]
                string += ": %s." if has_param else "."
                results.append(string)
            else:
                results.extend(strings)

    return results

def process_yaml_file(filename):
    MAIN_KEYS = ["name", "short_name", "adjective", "genus", "walking_verb", "altar_action"]


    infile = open(filename)
    data = infile.read()
    infile.close()

    species = {}
    species["mutations"] = []
    lines = data.splitlines()
    for line in lines:
        line = line.strip()
        if line.startswith('#') or ":" not in line:
            continue

        tokens = line.split(':')
        if len(tokens) != 2:
            continue

        key = tokens[0].replace('-', '').strip()
        if key in MAIN_KEYS:
            value = tokens[1].strip().replace('"', '')
            species[key] = value

            # by default short_name is first 2 chars of name
            if key == "name" and "short_name" not in species:
                species["short_name"] = value[0:2]

        elif key in ["long", "short"]:
            value = tokens[1].strip().replace('"', '')
            species["mutations"].append(value)

    if "genus" not in species:
        if species["name"] == "Demigod":
            # Short_genus hardcoded in skills.cc
            species["genus"] = "God"
        elif species["name"] == "Hill Orc":
            # Hill Orc genus is Hill Orc, but Lava Orc genus was Orc, and there's no yaml file for Lava Orc
            species["genus"] = "Orc"

    result = []
    for key in MAIN_KEYS:
        if key in species:
            if key == "walking_verb":
                result.append(species["walking_verb"] + "ing")
                result.append(species["walking_verb"] + "er")
            elif key == "altar_action":
                string = PRAY_SENTENCE.replace("%s", species[key], 1)
                result.append(string)
            elif key in ["name", "genus"]:
                result.append(article_the(species[key]))
                if key == "genus":
                    # also add lower-case version
                    string = species[key].lower()
                    result.append(article_the(string))
            elif key == "adjective" and species[key] == "Draconian":
                # same as species name
                continue
            else:
                result.append(species[key])
    result.extend(species["mutations"])

    return result

# strip (potentially) multi-line comments (i.e. /*...*/)
def strip_multiline_comments(data):
    result = ""
    escaped = False
    in_string = False
    in_char = False
    in_multiline_comment = False
    in_line_comment = False
    prev = '\0'
    length = len(data)
    for i in range(length):
        ch = data[i]

        if ch == '\\' and not escaped:
            escaped = True
        else:
            escaped = False

        if in_string:
            in_string = (ch != '"' or escaped)
        elif in_char:
            in_char = (ch != "'" or escaped)
        elif in_multiline_comment:
            in_multiline_comment = (ch != "/" or prev != '*')
            prev = ch
            continue
        elif in_line_comment:
            in_line_comment = (ch != "\n" and ch != "\r")
        elif ch == '"':
            in_string = True
        elif ch == "'":
            in_char = True
        elif ch == '/' and i+1 < length:
            if data[i+1] == "*":
                in_multiline_comment = True
            elif data[i+1] == "/":
                in_line_comment = True

        if not in_multiline_comment:
            result += ch
        prev = ch

    return result

# strip single-line comments (i.e. //...)
def strip_line_comment(line):
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
                return line[0:i-1].rstrip()
    # no comment - return whole line
    return line

# start of conditionally compiled section that can be skipped (debug or obsolete code)?
def is_skippable_if(line):
    return re.search(r'^\s*#\s*ifdef .*(DEBUG|VERBOSE)', line) or \
       re.search(r'^\s*#\s*if +defined *\(DEBUG', line) or \
       re.search(r'^\s*#\s*if\s*TAG_MAJOR_VERSION\s*==\s*34', line)

# strip out stuff that is excluded by #ifdef's, etc.
def strip_uncompiled(lines):
    skip = False
    result = []
    ifs = []
    skips = []

    for line in lines:

        if re.search(r'^\s*#', line):
            if re.search(r'^\s*#\s*if', line):
                ifs.append(line)
                # skip parts that are only included in DEBUG build
                if is_skippable_if(line):
                    skip = True
                    skips.append(skip)
                    continue
                else:
                    skips.append(skip)
            elif re.search(r'^\s*#\s*else', line):
                if skips[-1] == True:
                    if len(skips) < 2 or skips[-2] == False:
                        skip = False
                        skips[-1] = False
                elif 'DEBUG' in ifs[-1]:
                    # the if block is the non-debug part, so the else block is the debug part
                    skip = True
                    skips[-1] = True
                if is_skippable_if(ifs[-1]):
                    continue
            elif re.search(r'^\s*#\s*endif', line):
                if_line = ifs.pop()
                skips.pop()
                if len(skips) == 0:
                    skip = False
                else:
                    skip = skips[-1]
                if is_skippable_if(if_line):
                    continue

        if not skip:
            result.append(line)
            #sys.stderr.write(line + "\n")

    return result

# get file contents as list of lines
# uncompiled sections are stripped out
# comments are stripped out, apart from those containing directives for this script
def get_cleaned_file_contents(filename):
    infile = open(filename)
    data = infile.read()
    infile.close()

    data = strip_multiline_comments(data)
    lines = strip_uncompiled(data.splitlines())

    result = []
    for line in lines:

        # strip single-line comments, apart from those that have directives for this script
        if '//' in line:
            if not re.search(r'//.*noloc', line) and not re.search(r'//[ @]*localise\b', line):
                line = strip_line_comment(line)
                if line == '':
                    continue

        result.append(line)

    return result

# replace function args with a dummy value
# we do this if the arg can be a string, but we don't want to extract it
# (because it's an internal key, etc.)
def dummy_function_args(line, funcname):
    index = line.find(funcname)
    if index < 0:
        return line
    index += len(funcname)
    index = line.find("(", index)
    if index < 0:
        return line
    start = index
    depth = 1
    while depth > 0 and index < len(line) - 1:
        index += 1
        if line[index] == '(':
            depth += 1
        elif line[index] == ')':
            depth -= 1
    end = index

    args = line[start+1:end]
    if '"' not in args:
        return line

    line = line[0:start+1] + "dummy" + line[end:]
    #sys.stderr.write(line + "\n")
    return line

# replace strings that should not be extracted with dummies
def do_dummy_string_replacements(lines):
    result = []
    in_map = False
    for line in lines:
        if '//' in line and re.search('//.*noloc(?! *section)', line):
            # line marked as not to be localised - replace strings with dummies
            line = re.sub(STRING_PATTERN, '"_dummy_"', line);
            # noloc comment no longer needed, and might be in the way
            line = strip_line_comment(line)

        # ignore keys (but not values) in map initialisation
        if re.search(r'map<string,[^>]+> +[A-Za-z0-9_]+\s+=', line):
            in_map = True
        if in_map:
            # surround with @'s so it looks like a param name (later code will skip it)
            line = re.sub(r'\{\s*"([^"]+)"\s*,', r'{"@\1@",', line)
            #sys.stderr.write("NERFED MAP KEY: " + line + "\n")
        # NOTE: map end could be same line as map start
        if re.search(r'}\s*;', line):
            in_map = False

        if '"' not in line:
            result.append(line)
            continue

        # all string params to these functions are keys or identifiers
        m = re.search(r'\b(get[a-zA-Z]*String|json_[a-z_]+)\b', line)
        if m and m[0] and m[0] != "json_write_string":
            line = dummy_function_args(line, m[0])

        # both parameters are identifiers
        line = re.sub(r'(?<=json_write_string) *\("(msg|type)", [^\)]+\)', '($1, value)', line)

        # the first parameter to these functions is a key or identifier
        line = re.sub(r'(localise_contextual|json_write_string) *\([^,]+,', '$1(dummy,', line)

        result.append(line)

    return result


# convert a string containing a parameter into a list of strings for every
# possible value of the param
def expand_param(string, param, values):
    if param not in string:
        return [string]

    result = []
    for value in values:
        result.append(string.replace(param, value))

    return result


# First-stage line processing:
#   join statements that are split over multiple lines
#   join consecutive strings (in C++, "foo" "bar" is the same as "foobar")
#   strip trailing whitespace
#   strip out blank lines
def do_first_stage_line_processing(lines):
    result = []

    for line in lines:

        line = line.rstrip()

        # skip blank lines (might get in the way of statement-joining)
        if line == '':
            continue

        if len(result) > 0:
            last = result[-1]
            if not (last.endswith(';') or last.endswith('}') or last.endswith('{')):
                # check for statements split over multiple lines
                curr = line.lstrip()

                # join strings distributed over several lines
                if last.endswith('"') and curr.startswith('"'):
                    result[-1] = last[0:-1] + curr[1:]
                    continue

                join = False
                if '(' in last and last.count('(') > last.count(')'):
                    # join function calls split over multiple lines
                    join = True
                elif last.endswith('?') or curr.startswith('?') or last.endswith(':') or curr.startswith(':'):
                    # join ternary operator split over multiple lines
                    if last.endswith(':') and (re.search(r'\bcase\b', last) or re.search(r'(public|protected|private|default)\s*:$', last)):
                        # false positive
                        pass
                    else:
                        join = True
                #elif last.endswith('&&') or curr.startswith('&&') or last.endswith('||') or curr.startswith('||'):
                #    join = True
                elif last.endswith('=') and not curr.startswith('{'):
                    # assignment
                    join = True
                elif curr.startswith('='):
                    # assignment
                    join = True

                if join:
                    result[-1] = last + ' ' + curr
                    continue

        # join consecutive strings on same line (this is what the C++ compiler will do)
        if '"' in line:
            line = re.sub(r'(?!\\)"\s+"', '', line)

        result.append(line)

    return result


# insert section markers
# inserts a comment like "// @locsection: foo"
# recognised sections are classes, functions, and static array initialisations
def insert_section_markers(filename, lines):
    result = []
    section = None
    last_section = None
    for line in lines:
        stripped = line.strip()
        if '(' in line and not stripped.endswith(';') \
            and re.search('^[a-zA-Z]', line) \
            and (re.search(r'\)(\s+const)?$', stripped) or re.search(r'\)\s*:', stripped)):
            # function/method
            section = re.sub(r' *\(.*', '', line)
            section = re.sub('operator *', 'operator_', section)
            section = re.sub('^.*[ *]', '', section)
        elif line.startswith('class ') or (section == None and line.strip().startswith('class ')):
            # class
            section = re.sub('[ :].*', '', re.sub('^class *', '', line.strip()))
        elif re.search(r'^(static|const) ', line) and '=' in line and \
            (re.search('(vector|map)<', line) or re.search(r'\[.*\] *=', line)):
            # static data
            section = re.sub(' *=.*', '', line)
            section = re.sub(r'\[.*\]', '', section)
            section = re.sub('.*[^A-Za-z0-9_]', '', section.strip())

        # Ewwwwww!
        if filename == 'item-name.cc':
            if section in ['armour_ego_name', 'jewellery_effect_name'] and 'else' in line:
                section += '(terse)'
            elif section == 'item_def::name_aux' and 'potion_colours[]' in line:
                section = 'potion_colours'
            elif section == 'potion_colours' and 'COMPILE_CHECK' in line:
                section = 'item_def::name_aux'

        if section != last_section:
            result.append('// @locsection: ' + section)
            last_section = section

        result.append(line)

    return result

def is_line_relevant(line):
    # not relevant if no strings
    if not '"' in line:
        return False

    # calls to mpr_nolocalise(), etc.
    if '_nolocalise' in line and not 'you.hand_act' in line:
        return False
    elif "localise" in line:
        return True

    # ignore pre-compiler directives, apart from #define
    if line.startswith('#') and not re.match(r'#\s*define', line):
        return False

    # ignore extern "C"
    if line.startswith('extern'):
        return False

    # ignore axed stuff
    if 'AXED' in line:
        return False

    if 'MSGCH_DIAGNOSTICS' in line:
        # ignore diagnostic messages - these are for devs
        return False
    elif 'MSGCH_ERROR' in line:
        # Error messages mostly relate to programming errors, so we
        # keep the original English for the user to report to the devs.
        # The only exception is file system-related messages, which
        # relate to the user's own environment.
        if not re.search(r'(file|directory|open|writ| read |local|load|save)', line):
            return False

    # ignore file operations (any strings will be paths/filenames/modes)
    if 'fopen' in line or 'freopen' in line:
        return False
    if '_hs_open' in line or 'lk_open' in line:
        return False
    if 'catpath' in line or 'sscanf' in line:
        return False

    # ignore date formatting strings
    if 'strftime' in line:
        return False

    # ignore debug messages
    if re.search(r'\bdie(_noline)? *\(', line) or \
        re.search(r'dprf? *\(', line) or \
        re.search(r'dprintf *\(', line) or \
        re.search(r'(debuglog|debug_dump_item|dump_test_fails) *\(', line) or \
        re.search(r'bad_level_id', line) or \
        'bad item' in line or \
        'game_ended_with_error' in line or \
        re.search(r'ASSERTM? *\(', line) or \
        'DEBUG' in line or \
        'log_print' in line or \
        re.search(r'fprintf *\(', line):
        return False

    # scorefile stuff - any strings will be keys
    if 'add_field' in line or 'str_field' in line or 'int_field' in line:
        return False
    if 'death_source_flags' in line:
        return False

    # ignore lua code
    if 'execfile' in line:
        return False
    if re.search(r'^[^"]*lua[^"]*\(', line):
        return False

    # Leave notes/milstones in English
    if 'milestone' in line or 'mile_text' in line:
        return False
    if re.search('take_note', line) or re.search('note *=', line):
        return False
    if re.search('delete[a-z_]*mutation', line):
        return False
    if re.search(r'mutate\s*\(', line):
        return False
    if re.search(r'\bbanish(ed)?\s*\(', line):
        return False

    # skip tags/keys
    if re.search(r'^[^"]*_tag\(', line) and not re.search('text_tag', line):
        return False
    if re.search(r'tag\s*=\s*"', line):
        return False
    if re.search(r'strip_tag_prefix *\(', line):
        return False
    if 'annotate_string' in line:
        return False
    if 'tiles.write_message' in line:
        return False
    if 'serialize' in line:
        return False
    if '_chunk' in line:
        return False
    if '_id =' in line:
        return False
    if 'push_ui_layout' in line or 'ui_state_change' in line:
        return False
    if re.search(r'\bmenu_colour *\(', line):
        return False
    if re.search(r'\bprops\.erase *\(', line):
        return False
    if re.search(r'\bPROPS[A-Z_]*\s*=', line):
        return False
    if 'getLongDescription' in line:
        return False
    if '_print_converted_orc_speech' in line:
        return False
    if '_get_xom_speech' in line or 'XOM_SPEECH' in line:
        return False
    if '_get_species_insult' in line:
        return False
    if 'show_specific_help' in line:
        return False
    if '_translate_tentacle_ref' in line:
        return False
    if 'print_hint' in line or 'tutorial_msg' in line:
        return False
    if re.search(r'^[^"]*property[A-Za-z_]* *\(', line):
        return False
    if re.match(r'^\s*key[A-Za-z_]*\.[A-Za-z_]*\(', line):
        return False
    if re.search(r'set_sync_id\s*\(', line):
        return False
    if re.search(r'compare_item', line):
        return False
    if re.search(r'^# *define.*KEY', line):
        return False
    if 'desc_key' in line:
        return False
    if 'GetModuleHandle' in line:
        return False
    if re.search(r'\bcreate_item_named *\(', line):
        return False
    if re.search(r'\bwrite_webtiles_options *\(', line):
        return False
    if re.search(r'^\s*dbname *= ', line):
        return False

    # find or compare
    if re.search(r'\bstrstr\s*\(', line):
        return False
    if 'search_stashes' in line:
        return False
    if re.search(r'\bstrn?i?cmp\b', line):
        return False
    if '_strip_to' in line:
        return False

    if re.search(r'\bstrlen\s*\(', line):
        return False

    return True

# return only lines that have
#   a) strings that might need to be extracted
#   b) directives for this script
# any leading or trailing whitespace is removed
def get_relevant_lines(filename, lines):
    result = []
    explicit_ignore = False
    implicit_ignore = False
    section = ''
    for line in lines:
        # ignore sections explicitly marked as not to be extracted
        if 'noloc section' in line:
            if 'noloc section start' in line:
                explicit_ignore = True
            if 'noloc section end' in line:
                explicit_ignore = False
            continue

        if '@locsection' in line:
            section = re.sub('.*@locsection: *', '', line);
            implicit_ignore = ignore_section(filename, section)

        ignore = implicit_ignore or explicit_ignore

        if ignore and not re.search(r'//[ @]*localise\b', line):
            continue

        if '//' in line:
            result.append(line.strip())
            continue

        if filename == 'job-data.h':
            # special handling - only take the line with the job abbreviation and name
            if not re.search(r'"[A-Z][a-zA-Z]"', line):
                continue

        line = line.strip()

        if is_line_relevant(line):
            result.append(line)

    return result


# tokenize line into string and non-string
def tokenize_cplusplus_line(line):
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

    return tokens

# extract strings from Lua line (can be enclosed in single or double quotes)
# inclosing quotes are included in the results
def extract_lua_strings(line):

    quote = None
    start_pos = None
    results = []

    for i in range(0, len(line)):
        if quote is None:
            if line[i] in "'\"":
                quote = line[i]
                start_pos = i
        elif line[i] == quote and line[i-1] != "\\":
            # end of string
            results.append(line[start_pos:i+1])
            quote = None

    return results

def extract_strings_from_des_spellbook_line(line):
    items = line.split('/')
    strings = []
    for item in items:
        if 'randbook' not in item:
            continue
        #print('DEBUG:' + item, file=sys.stderr)
        m = re.search(r'title:([^\s]+)', item)
        if m:
            subject = m.group(1).replace('_', ' ')
            strings.append("# note: book subject")
            strings.append(subject)
        m = re.search(r'owner:([^\s]+)', item)
        if m:
            owner = m.group(1).replace('_', ' ')
            if owner != "player":
                strings.append("# note: book owner")
                strings.append(owner)
    return strings

# where a name is overriden in a .des file, extract the new name and inflections
def extract_strings_from_des_rebadge_line(line):

    strings = []

    # multiple monsters can be on the same line separated by slashes or commas
    # process them separately
    if '/' in line or ',' in line:
        lines = re.split('[/,]', line)
        for l in lines:
            strings.extend(extract_strings_from_des_rebadge_line(l))
        return strings

    # remove any existing quotes
    line = line.replace('"', '')

    if re.search(r'\bshop\b', line) or ('type:' in line and 'suffix:' in line):
        # Handle shop names
        line = re.sub(r'\s*\.\.\s*([a-zA-Z_]+)\s*\.\.', r'@\1@', line)
        line = line.replace('@smithy@', '@owner@')

        # extract owner name
        owner = "@owner@"
        m = re.search(r'(?<=\bname:)[^ \)\}]+', line)
        if  m:
            owner = m.group()

        # extract shop type
        shop_type = None
        m = re.search(r'(?<=\btype:)[^ \)\}]+', line)
        if m:
            shop_type = m.group()

        # extract shop suffix
        suffix = None
        m = re.search(r'(?<=\bsuffix:)[^ \)\}]+', line)
        if m:
            suffix = m.group()

        if not shop_type:
            return []

        name = owner + "'s " + shop_type
        if suffix:
            name += " " + suffix
        name = name.replace('_', ' ')

        return [name]

    if 'name:' not in line:
        return []

    # clean up the line
    line = re.sub(r'.*= *', '', line)
    line = re.sub(r'K?MONS: *', '', line)
    line = re.sub(r'spells:[^ ]+', '', line)
    line = re.sub(r';.*', '', line)
    line = re.sub(r'[\(\)\{\}\.]', ' ', line)
    line = re.sub('  +', ' ', line).strip()
    line = re.sub("^'", '', line)
    line = re.sub("'$", '', line)

    # extract base (original) name
    words = re.findall(r'\b[^ ]+\b', line)
    base_name = ''
    for word in words:
        if re.match(r'^[a-z]*$', word):
            base_name += ' ' + word
    base_name = base_name.strip()

    # extract override
    m = re.search(r'(?<=\bname:)[^ ]+', line)
    if not m:
        return []
    override = m.group()
    override = override.replace('_', ' ')

    string = ""
    is_adjective = False
    if 'name_adjective' in line or 'n_adj' in line:
        if override in ['sickly', 'monstrous', 'deformed', 'twisted', 'grotesque', 'hideous', 'febrile', 'skinned']:
            # just take the adjective
            string = override + " "
            is_adjective = True
        else:
            # generate the full name
            string = override + " " + base_name

    elif 'name_suffix' in line or 'n_suf' in line:
        string = base_name + " " + override
    else:
        string = override

    if string == "":
        return []

    # if adjective, just return this single string as is
    if is_adjective:
        strings.append(string)
        return strings

    if " " in string:
        for adj in ["rotten ", "ancient ", "large "]:
            if string.startswith(adj):
                strings.append(adj)
                string = string.replace(adj, '')
                break

    is_item = string.endswith('corpse')
    if is_item:
        strings.append(article_the(string))
    else:
        append_monster_permutations(strings, string)
    return strings


def process_des_or_lua_file(filename):

    is_des = filename.endswith('.des')
    is_portal = '/portals/' in filename

    infile = open(filename)
    data = infile.read()
    infile.close()

    raw_lines = data.splitlines()
    lines = []

    # remove comments and map sections
    is_map = False
    for line in raw_lines:
        line = line.strip()
        if line.startswith('--') or line.startswith('#'):
            continue
        elif line == '':
            continue
        elif line == "MAP":
            is_map = True
        elif line == "ENDMAP":
            is_map = False
        elif not is_map:
            if is_des and line.startswith(':'):
                # marker for single line of Lua
                line = line[1:].strip()
            lines.append(line)
    raw_lines = lines
    lines = []

    # a line ending in backslash means the statement continues on the next line
    for line in raw_lines:
        line = line.strip()
        if lines and lines[-1].endswith('\\'):
            lines[-1] = lines[-1][:-1].rstrip()
            if lines[-1].endswith(';'):
                lines.append(line)
            else:
                lines[-1] += " " + line
        else:
            lines.append(line)

    raw_lines = lines
    lines = []

    for line in raw_lines:
        concatenate = False
        if len(lines) > 0:
            if line.startswith('..') or lines[-1].endswith('..'):
                concatenate = True
            elif line.startswith(',') or lines[-1].endswith(','):
                concatenate = True
            elif line.startswith('and ') or lines[-1].endswith(' and'):
                concatenate = True
            elif line.startswith('or ') or lines[-1].endswith(' or'):
                concatenate = True
            elif lines[-1].endswith('{') and lines[-1] != '{{':
                concatenate = True

        if concatenate:
            lines[-1] += ' '  + line
        else:
            lines.append(line)

    wizlab_descs = []

    # for portals
    noise = None
    noisemaker = None

    #if "bailey.des" in filename:
    #    dump_lines(filename, lines)

    strings = []
    section = ''
    for line in lines:
        if line.startswith('function '):
            section = re.sub(r'^function\s*', '', line)
            section = re.sub(r'\s*\(.*', '', section)
            strings.append('# section: ' + section)
            continue
        elif line.startswith('NAME:'):
            section = line.replace("NAME:", "").strip()
            strings.append('# section: ' + section)

        # don't extract map keys
        line = re.sub(r'\["[^"]*"\]', '[dummy]', line)

        # extract wizlab descriptions
        if filename.endswith('wizlab.des') and 'wizlab_milestone' in line and '"' in line:
            m = re.search('(?<=")[^"]+(?=")', line)
            if m and m.group(0):
                wizlab_descs.append(m.group(0))

        if is_portal:
            match = re.search(r'(?<=verb)\s*=\s*[\'"][^\'"]+(?=[\'"])', line)
            if match and match.group(0):
                noise = re.sub(r'\s*=\s*[\'"]', '', match.group(0))

            match = re.search(r'(?<=noisemaker)\s*=\s*[\'"][^\'"]+(?=[\'"])', line)
            if match and match.group(0):
                noisemaker = re.sub(r'\s*=\s*[\'"]', '', match.group(0))

            line = re.sub(r'(?:entity|dstname)\s*=\s*["\'][^"\']+["\']', '', line)
            line = re.sub(r'(?:noisemaker|verb)\s*=\s*["\'][^"\']+["\']', '', line)

            if noise is not None and noisemaker is not None:
                prefix = "You hear the @adjective@"
                strings.append(prefix + noise + " of " + article_a(noisemaker) + ".")
                strings.append(prefix + noise + " of a distant " + noisemaker + ".")
                strings.append(prefix + noise + " of a very distant " + noisemaker + ".")
                strings.append(prefix + noise + " of " + article_a(noisemaker) + " nearby.")
                strings.append(prefix + noise + " of " + article_a(noisemaker) + " very nearby.")
                noise = None
                noisemaker = None

        if is_des:
            skip = True
            if 'crawl.mpr' in line or 'crawl.god_speaks' in line:
                skip = False
            elif 'lua:' in line:
                skip = False
            elif re.search(r'\bset_feature_name', line):
                # first param is a key
                line = re.sub('"[^"]",', 'dummy,', line)
                skip = False
            elif 'randbook' in line:
                strings.extend(extract_strings_from_des_spellbook_line(line))
                continue
            elif re.search(r'\bname:', line):
                strings.extend(extract_strings_from_des_rebadge_line(line))
                continue
            elif re.search(r'\bshop\b', line) or ('type:' in line and 'suffix:' in line):
                strings.extend(extract_strings_from_des_rebadge_line(line))
                continue
            elif re.search(r'(?:msg|prompt)\s*=', line):
                skip = False
            elif is_portal and re.search(r'ranges\s*=', line):
                skip = False
            if skip:
                continue

        if 'debug' in section or 'dry_run ~= nil' in line or line.startswith('assert'):
            # debug stuff
            continue

        if 'note_payed' in section:
            # note
            continue

        if filename.endswith('lm_tmsg.lua') and section == 'TimedMessaging:init':
            continue

        if section.startswith('TroveMarker:search'):
            continue

        if section == 'TroveMarker:item_name':
            # this replicates the C++ item_name (I hope)
            continue

        if section == 'TimedMessaging:range_adjective':
            # covered by the portal code above
            continue

        if 'vector_move' in line:
            # command string, not display string
            continue

        if line.startswith('error') or line.startswith('flag_order = '):
            continue

        if line.endswith(' then'):
            # if or else
            continue

        if 'CLASS =' in line or '__index =' in line:
            continue

        if 'dgn_event_type' in line:
            continue

        if 'lua:' in line:
            m = re.search('desc *= *"([^"]+)"', line)
            if m and m[1]:
                strings.append(article_the(m[1]))
            continue

        # don't extract strings that are just used for comparison/search
        line = re.sub(r'==\s*\"[^"]*\"', '== dummy', line)
        line = re.sub(r'~=\s*\"[^"]*\"', '~= dummy', line)
        line = re.sub(r'find\s*\([^\)]+\)', 'find(dummy)', line)
        line = re.sub(r'match\s*\([^\)]+\)', 'match(dummy)', line)

        line = line.replace('dgn.feature_desc_at(x, y, "The")', 'the_feature')

        # join strings that are joined at runtime
        if '..' in line:
            line = re.sub(r'"\s*\.\.\s*"', '', line)
            line = re.sub(r"'\s*\.\.\s*'", '', line)

        # replace ellipsis to avoid false matches with join operator
        line = line.replace('...', 'ELLIPSIS')

        # turn joins of variables, etc. into embedded params
        if '..' in line and 'name:' not in line:
            # embedded conditional
            line = re.sub(r'\.\.\s*\([^\)]+\)', '.. param', line)

            # dont make self a param in concatenation (e.g. msg = msg .. whatever)
            line = re.sub(r'([a-z]+)\s*=\s*\1\s*\.\.', r'\1 =', line)

            # param in middle of string
            line = re.sub(r'"\s*\.\.\s*([^"]*?)\s*\.\.\s*"', r'@\1@', line)

            # param at end of string
            line = re.sub(r'"\s*\.\.\s*(.*)', r'@\1@"', line)

            # param at start of string
            line = re.sub(r'(return\s*|.*= *|mpr\(\s*)(.*?)\s*\.\.\s*"', r'\1"@\2@', line)

            # consecutive params
            line = re.sub(r'\s*\.\.\s*', '@@', line)

            params = re.findall('@[^@]+@', line)
            for param in params:
                fixed_param = ''
                if param == '@AUTOMAGIC_SPELL_SLOT@':
                    fixed_param = '@slot@'
                elif 'spell_table' in param:
                    fixed_param = '@spell_name@'
                elif param == '@runes[name]@':
                    fixed_param = '@rune_name@'
                elif param.endswith('the_feature@'):
                    fixed_param = '@the_feature@'
                else:
                    fixed_param = param
                    fixed_param = re.sub(r'tostring\((.*)\)', r'\1', fixed_param)
                    fixed_param = re.sub(r'[\(\[].*[\)\]]', '', fixed_param)
                    fixed_param = re.sub(r'^@.*[:\.]', '@', fixed_param)
                    fixed_param = re.sub(r' [^@]*', '', fixed_param)
                if fixed_param != param:
                    line = line.replace(param, fixed_param)
                if not re.match('^@[A-Za-z_]+@$', fixed_param):
                    print("BAD PARAM: " + fixed_param, file=sys.stderr)
                    print("IN: " + line, file=sys.stderr)

            # verb is actually a noun
            line = line.replace('@chk@@verb@', '@adjective@@noise@')

        # restore ellipsis
        line = line.replace('ELLIPSIS', "...")

        if 'crawl.mpr' in line:
            # we don't want to extract the second parameter - it's the channel
            line = re.sub(r',\s*"[^"]*"\s*\);?$', ', channel)', line)
            line = re.sub(r",\s*'[^']*'\s*\);?$", ', channel)', line)

        matches = extract_lua_strings(line)
        for match in matches:
            string = match[1:-1] # remove quotes
            if len(string) < 2:
                continue
            if 'ERROR' in string or 'Error' in string or 'buggy' in string:
                continue
            if 'marker' in string or 'Marker' in string:
                continue
            if '_' in string and not '@' in string:
                # identifier
                continue
            if filename.endswith('automagic.lua') and string == " enabled,":
                continue
            if string in [" and", " the @name@", "@feat@/@dur@", r"AUTOMAGIC_SPELL_SLOT = '@slot@'\n"]:
                continue

            if filename.endswith('nemelex_the_gamble.des'):
                # expand
                if string.startswith(r'\"') and not string.startswith(r'\"Beware'):
                    string = 'Nemelex Xobeh says, ' + string
                elif string.startswith('Nemelex Xobeh says, @'):
                    continue

            # make sure double quotes are escaped
            string = re.sub(r'(?<!\\)"', r'\"', string)

            # make sure inital param is capitalised
            string = re.sub('^@the_', '@The_', string)

            if string == "no spell currently":
                strings.append("# note: @spell_name@ when no spell in chosen slot")

            # split on newlines
            substrings = string.split("\\n")
            for ss in substrings:
                if ss != "":
                    if 'set_feature_name' in line or 'desc =' in line:
                        if re.match('^([A-Z]|the |some |a |an )', ss):
                            strings.append(ss);
                        elif 'set_feature_name' in line:
                            strings.append(article_a(ss));
                        else:
                            strings.append(article_the(ss));
                    else:
                        strings.append(ss)

    # separate and clean up annotations
    if filename.endswith('stash.lua'):
        raw_strings = strings
        strings = []
        for string in raw_strings:
            if '} {' in string:
                substrings = string.split('} {')
                for ss in substrings:
                    if '@res@' in string:
                        # expanded below (cold, corrosion, etc.)
                        continue
                    ss = ss.replace('{', '').replace('}', '').strip()
                    strings.append(ss)
            elif '+' in string or ('-' in string and '-handed' not in string):
                substrings = string.split(" ")
                for ss in substrings:
                    strings.append(ss.replace('+', '').replace('-', ''))
            else:
                string = string.replace('{', '').replace('}', '').strip()
                if string in ['melee', 'ranged']:
                    string += ' weapon'
                elif string in ['cold', 'corrosion', 'electricity', 'fire', 'mutation', 'negative energy', 'poison']:
                    strings.append('resist ' + string)
                    strings.append(string + ' resistance')
                    continue
                elif string == '@subtype@ armor':
                    strings.append('body armour')
                else:
                    strings.append(string)

    # expand params
    raw_strings = strings
    strings = []
    for string in raw_strings:
        if "@" not in string and "$F" not in string:
            strings.append(string)
            continue

        # ignore if just a param and nothing else
        if re.match(r'^@[^@]*@$', string):
            continue

        alternatives = [string]
        if '@plural@' in string:
            alternatives = expand_param(string, "@plural@", ["", "s"])
        elif '@caught@' in string:
            alternatives = expand_param(string, "@caught@", ['held in a net', 'caught in a web'])
        elif filename.endswith('automagic.lua') and '@message@' in string:
            alternatives = expand_param(string, "@message@", ["", " enabled,"])
        elif filename.endswith('bailey.des') and "$F{The}" in string:
            alternatives = expand_param(string, "$F{The}", ["The portcullis"])
        elif "the wizard's @param@cell" in string:
            alternatives = expand_param(string, "@param@", ["", "empty "])
        elif '@spawn_dir@' in string:
            alternatives = expand_param(string, "@spawn_dir@", ["north", "south", "east", "west"])
        elif 'sudden vision@msg@' in string:
            alternatives = expand_param(string, "@msg@", [" of the Swamp and the Snake Pit", \
                                                          " of the Swamp and the Spider Nest", \
                                                          " of the Shoals and the Snake Pit", \
                                                          " of the Shoals and the Spider Nest" \
                                                         ])
        elif '@wizlab_desc@' in string:
            alternatives = expand_param(string, "@wizlab_desc@", wizlab_descs)
        elif filename.endswith('pan.des') and '@name@ resides here' in string:
            alternatives = expand_param(string, "@name@", ["Cerebov", "Mnoleg", "Lom Lobon", "Gloorx Vloq"])
        elif filename.endswith('pan.des') and '@rune_name@' in string:
            alternatives = expand_param(string, "@rune_name@", ["fiery", "glowing", "magical", "dark"])
        elif '@noise@ of @noisemaker@' in string:
            # will be covered under each specific portal
            alternatives = []

        strings.extend(alternatives)

    return strings


def process_cplusplus_file(filename):
    lazy = (filename in LAZY_FILES)

    strings = []

    lines = get_cleaned_file_contents(filename)
    lines = do_first_stage_line_processing(lines)
    lines = insert_section_markers(filename, lines)
    lines = do_dummy_string_replacements(lines)
    lines = get_relevant_lines(filename, lines)

    section = ''
    last_section = ''
    for line in lines:
        #sys.stderr.write(line + "\n")

        if '//' in line:
            if '@locsection' in line:
                section = re.sub(r'^.*locsection:? *', '', line)
                continue

        if '"' not in line:
            continue

        if filename == "tileweb.cc":
            if re.search(r'\b(_update_int)\b', line):
                continue
            if "errmsg =" in line:
                continue
            # these could theoretically send strings that we want,
            # but in practice, they are not, and they are hard to parse
            if re.search(r'\b(send_message|write_message)\b', line):
                continue
            # 4th param to this function is an identifier
            line = re.sub(r'\b_update_string *\(([^,]+),([^,]+),([^,]+),([^,\)]+)', \
                          '_update_string($1,$2,$3, dummy', line)

        if lazy:
            extract = False

            if 'localise' in line:
                extract = True
            elif 'simple_monster_message' in line or 'simple_god_message' in line:
                extract = True
            elif 'any_2_actors_message' in line or '3rd_person_message' in line:
                extract = True
            elif re.search(r'mpr[a-zA-Z_]* *\(', line):
                # extract mpr, mprf, etc. messages
                extract = True
            elif re.search(r'(prompt|msgwin_get_line)[a-zA-Z_]* *\(', line) or 'yesno' in line \
                or 'yes_or_no' in line:
                # extract prompts
                extract = True
            elif re.match(r'\s*end *\(', line) and not 'DEBUG' in line:
                extract = True
            elif re.search(r'\bsave_game *\(', line):
                extract = True
            elif re.search(r'\bhand_act *\(', line):
                extract = True
            elif 'cant_cmd_' in line:
                extract = True
            elif 'get_num_and_char' in line:
                extract = True

            # ignore strings unless we have a specific reason to extract them
            if not extract:
                continue

        if 'any_2_actors_message' in line:
            temp = re.sub(r'.*any_2_actors_message *\(', '', line);
            temp = re.sub(r'\).*', '', line);
            args = temp.split(',')
            verb = ''
            suffix = ''
            if len(args) >= 3:
                arg = args[2].strip()
                if arg.startswith('"'):
                    verb = remove_enclosing_quotes(arg)
            if len(args) >= 4:
                arg = args[3].strip()
                if arg.startswith('"'):
                    suffix = remove_enclosing_quotes(arg)
                    if suffix != '':
                        suffix = ' ' + suffix
            if verb != '':
                strings += do_any_2_actors_message(verb, suffix)
            continue

        tokens = tokenize_cplusplus_line(line)

        for i in range(len(tokens)):
            token = tokens[i]
            if len(token) < 3 or token[0] != '"' or token[-1] != '"':
                continue;

            string = token[1:-1]

            if i != 0:
                last = tokens[i-1]

                # skip (in)equality tests (assume string is defined elsewhere)
                if re.search(r'[=!]=\s*$', last):
                    continue
                if re.search(r'\bstr(case)?cmp\b', last):
                    continue

                # crawl environment settings
                if re.search(r'\benv\..*(=|insert *\() *$', last):
                    continue

                # skip map keys
                if re.search(r'\[\s*$', last):
                    continue

                if '(' in last:
                    # another type of equality test
                    if re.search(r'\b(starts_with|ends_with|contains)\s*\([^,"]+,\s*$', last):
                        continue
                    if re.search(r'\bfind\s*\(\s*(string\()?$', last):
                        continue
                    if re.search(r'\b(exists|matches)\s*\(\s*$', last):
                        continue

                    if re.search(r'\bsplit_string\s*\(', last):
                        continue
                    if re.search(r'\bstrip_suffix\s*\(', last):
                        continue
                    if re.search(r'\bsend_exit_reason\s*\(', last):
                        continue
                    if re.search(r'\bsend_dump_info\s*\(', last):
                        continue
                    if re.search(r'\breplace[a-zA-Z_]*\s*\(', last):
                        continue

            if section != last_section:
                strings.append('# section: ' + section)
                last_section = section

            # simple_god/monster_message may contain an implied %s
            if string.startswith(" ") or string.startswith("'"):
                if 'simple_god_message' in line or 'simple_monster_message' in line \
                  or '_spell_retribution' in line \
                  or (filename == 'beam.cc' and section in ['mass_enchantment','poison_monster']) \
                  or (filename == 'mon-abil.cc' and section == 'ugly_thing_mutate') \
                  or (filename == 'mon-cast.cc' and section == '_cast_cantrip') \
                  or (filename == 'mon-death.cc' and section == 'monster_die') \
                  or (filename == 'monster.cc' and section == 'monster::do_shaft'):
                    string = '%s' + string

            # beam hit verbs
            if re.search(r'beam\.hit_verb\s*=', line) or (filename == 'beam.cc' and re.search(r'hit_verb\s*=', line)):
                strings.append("%s " + string + " you")
                strings.append("%s " + string + " %s")
                continue

            # beam names
            if filename == "zap-data.h" \
              or (filename == "spl-damage.cc" and section == "fraggable_monsters") \
              or re.search(r'(beam|expl|effect)(\.|->)(name|aux_source)\s*=\s*"', line):
                if string in ["none", "****", "debugging ray", "rampaging", "STAIR BEAM", "explosion of "]:
                    continue
                elif string.startswith("by "):
                    string = string[3:]
                elif "by" not in string and "'s" not in string:
                    string = article_the(string)
            elif filename == "beam.cc" and string == "drain magic":
                # we want the one from describe.cc
                continue
            elif "aux_source" in line or re.search(r'(ouch|hurt|miscast_effect)\(', line):
                if string.endswith(" "):
                    string += "%s"
                if string.startswith("by "):
                    string = string[3:]
                elif "aux_source" in line and not string[0].isupper():
                    string = article_the(string)

            if 'calc_elemental_brand_damage' in line:
                # elemental damage attack verbs
                strings.extend(do_any_2_actors_message(string, ''))
                continue

            if 'enchant_monster_invisible' in line:
                strings.append('%s ' + string + '!')
                strings.append('%s ' + string + ' for a moment.')
                continue

            if 'wu_jian_sifu_message' in line:
                # this function adds a prefix to the message parameter
                string = 'Sifu %s' + string
            elif '3rd_person_message' in line:
                # also do the version where "you" is the object
                strings.append(replace_last(string, '%s', 'you'))

            if 'held_status' in line and 'while %s' in string:
                # there are only two possibilities
                strings.append(string.replace('while %s', 'while held in a net'))
                strings.append(string.replace('while %s', 'while caught in a web'))
                continue

            if 'convert_input_to_english' in line:
                # separate input chars
                for c in string:
                    strings.append(c)
                continue

            if ignore_string(string):
                continue

            if string.endswith(' by ') or string.endswith('attached to '):
                string += "%s"

            if filename == 'ability.cc':
                if string.startswith('Sacrifice '):
                    if string == 'Sacrifice ':
                        continue
                    # also used with 'Sacrifice ' removed for the cost
                    strings.append(string.replace('Sacrifice ', ''))
            elif filename == 'adjust.cc':
                if string.endswith("? "):
                    string = string[0:-1];
            elif filename == 'delay.cc':
                if string.startswith(' ') and section in ['_monster_warning', '_abyss_monster_creation_message']:
                    string = "%s" + string
            elif filename == 'describe.cc':
                if section == 'xl_rank_names':
                    # adjectives
                    string += ' '
            elif filename == 'describe-spells.cc':
                if section == "_ability_type_vulnerabilities":
                    # will be joined to strings from _abil_type_vuln_core before translation
                    if string == ", which are affected by %s":
                        continue
                elif section == "_abil_type_vuln_core":
                    # will be joined to string from _ability_type_vulnerabilities before translation
                    string = ", which are affected by " + string
            elif filename == 'god-wrath.cc':
                # god wratch names are mostly used only in notes.
                # the exception is the Wu Jian one
                if section == '_god_wrath_adjectives':
                    if string == 'rancor' or string == 'rancour':
                        string = article_the(string + ' of the Wu Jian Council')
                    else:
                        continue
                elif section == '_god_wrath_name':
                        continue
            elif filename == 'melee-attack.cc':
                if section.endswith('::set_attack_verb'):
                    # player-only attack verbs.
                    if 'verb_degree' in line or last.strip() == ",":
                        # 2nd part that follows the object
                        if len(string) != 0:
                            if string[0] not in [" ", ",", "'"]:
                                string = " " + string
                            if len(strings) != 0:
                                strings[-1] += string
                    else:
                        strings.append('You ' + string + ' %s')
                    if string == 'devastate':
                        # append form unarmed attacks
                        for verb in form_attack_verbs:
                            strings.append('You ' + verb + ' %s')
                    continue
                elif section == 'melee_attack::mons_attack_verb':
                    # monster-only attack verbs
                    verb = conjugate_verb(string)
                    strings.append('%s ' + verb + ' you')
                    strings.append('%s ' + verb + ' %s')
                    continue
                elif string == " from afar":
                    continue
                elif section.startswith('Aux'):
                    # player auxiliary attack
                    # usually same string is used as noun and verb, but there are a couple of exceptions
                    if string != 'pierce':
                        strings.append('your ' + string)
                    if string != 'tentacle spike':
                        strings.append('You ' + string + ' %s')
                    continue
            elif filename == 'mon-cast.cc':
                if section in ['_speech_keys', '_speech_message']:
                    # these are just keys for getSpeakString()
                    continue
                elif section in ['_speech_fill_target', 'mons_cast_noise']:
                    # filling params for speech strings, but we don't want to directly translate
                    # the prepositions (it's handled in monspeak.txt)
                    if 'target' not in line:
                        continue
                    if string == "nothing" or string.upper() == string:
                        # dummy placeholder
                        continue
            elif filename == 'mon-util.cc':
                if section in ['ugly_colour_names', 'drac_colour_names']:
                    # adjectives
                    string += ' '
                elif section == "_get_sound_string":
                    # we need the full thing (e.g. "says to @foe@") and also the verb alone
                    verb = string.replace(" at @foe@", "").replace(" to @foe@", "")
                    strings.append(verb)
                elif section == 'mon_attack_name':
                    if ' or ' not in string:
                        # also used in melee-attack.cc
                        verb = conjugate_verb(string)
                        strings.append('%s ' + verb + ' you')
                        strings.append('%s ' + verb + ' %s')
                        if verb in ['hits', 'stings']:
                            strings.append('%s ' + verb + ' you from afar')
                            strings.append('%s ' + verb + ' %s from afar')
            elif filename == 'output.cc':
                if section == 's_equip_slot_names' or section == 'equip_slot_to_name':
                    # equipment slots are only ever displayed in lowercase form
                    # and the specific ring slots are all just displayed as "ring"
                    if string.endswith(" Ring"):
                        string = "ring"
                    else:
                        string = string.lower()
            elif filename == 'ranged-attack.cc':
                if section.endswith('::set_attack_verb'):
                    # projectile is the subject, verb is already conjugated
                    strings.append('%s ' + string + ' you')
                    strings.append('%s ' + string + ' %s')
                    continue
            elif filename == 'spl-goditem.cc':
                if string.startswith('a scroll of '):
                    string = string.replace('a scroll', 'the scroll')
            elif filename == 'terrain.cc' and section == "feat_type_name":
                    string = article_the(string)
            elif filename == 'throw.cc' and section == '_setup_missile_beam':
                if string in ["explosion of ", " fragments"]:
                    # beam for explosive brand - now only used for Damnation artefact, and not displayed
                    continue

            # strip channel information
            string = re.sub(r'(PLAIN|SOUND|VISUAL|((VISUAL )?WARN|ENCHANT|SPELL)):', '', string)

            if string == " god" and "PRONOUN_POSSESSIVE" in line:
                strings.append("his god")
                strings.append("her god")
                strings.append("its god")
                strings.append('# note: singular "their"')
                strings.append("their god")
                continue

            if "\\n" in string:
                # split lines
                substrings = string.split("\\n")
                for ss in substrings:
                    strings.append(ss)
            else:
                if 'our @hand' in string:
                    # create strings for one and two hands (coz Ru)
                    string2 = string.replace('hands@', 'hand@')
                    string2 = string2.replace('@hand_conj@', 's')
                    string = string.replace('@hand_conj@', '')
                    strings.append(string2)
                strings.append(string)
                if filename == 'spl-miscast.cc' and "'s body" in string:
                    # string is also used with that substring for monsters that don't have a body
                    strings.append(string.replace("'s body", ""))

    return remove_duplicates(strings)

def add_strings_to_output(filename, strings, output):

    if len(strings) == 0:
        return

    output.append("")
    output.append("##################")
    output.append("# " + filename)
    output.append("##################")
    section = None
    for string in strings:
        # in some cases, string needs to be quoted
        #   - if it has leading or trailing whitespace
        #   - if it starts with # (because otherwise it looks like a comment)
        #   - if it starts and ends with double-quotes
        if string.startswith('# section:'):
            output.append(string)
            continue
        elif '# note' in string:
            output.append(string)
            continue
        elif re.search(r'^(\s|#)', string) or  re.search(r'\s$', string) \
           or (string.startswith(r'\"') and string.endswith('"')):
            string = '"' + string + '"'
        else:
            string = string.replace(r'\"', '"')
        string = string.replace(r'\\', '\\')

        if string in output:
            string = '# duplicate: ' + string
        output.append(string)

# At runtime, we derive noun forms with an indefinite article (a/an) from the
# form with the definite article (the), and the form with no article from the
# form with an indefinite article.
# Therefore, we want to discard the derived forms because they're redundant,
# and because keeping them would:
# a) add translation effort
# b) introduce opportunity for mismatches
def remove_derived_duplicates(strings):
    definites = []
    indefinites = []
    for string in strings:
        if has_article_the(string):
            definites.append(string)
        elif has_article_a(string):
            indefinites.append(string)

    result = []
    section = None
    for string in strings:
        if string.startswith('#'):
            if string.startswith('# section:'):
                section = re.sub(r'# section:\s*', '', string)
            elif string == "##################":
                # new file
                section = None
        elif has_article_a(string):
            if article_the(string) in definites:
                string = '# duplicate (derived): ' + string
        elif not has_article_the(string):
            # _flavour_base_desc contains verb phrases (not nouns) - keep distinct from beam names
            # _ashenzari_curses, learned_something_new - keep distinct from Nemelex card names
            if section not in ["_flavour_base_desc", "_ashenzari_curses", "learned_something_new"]:
                if article_the(string) in definites or article_a(string) in indefinites:
                    string = '# duplicate (derived): ' + string
        result.append(string)

    return result


###################################
# Post-processing of "raw" strings
###################################

# get rid of unnecessary section markers
def remove_unnecessary_section_markers(strings):
    section = None
    output = []
    for string in strings:
        if string.startswith('# section:'):
            section = string
        else:
            if section is not None:
                output.append(section)
                section = None
            output.append(string)
    return output

# separate adjectives from noun
# adjectives will have space appended
# noun will be last and have definite article
def separate_adjectives_and_noun(string):
    words = string.split()
    for i in range(len(words)):
        if i != len(words) - 1:
            words[i] = words[i] + " "
        else:
            words[i] = article_the(words[i])
    return words

def separate_adjectives(string):
    results = []
    words = string.split(' ')
    for word in words:
        results.append(word + ' ')
    return results

def is_unique_monster(string):
    # non-uniques with uppercase letters in them
    specials = [
        'Killer Klown', 'Orb Guardian', 'Brimstone Fiend', 'Ice Fiend',
        'Tzitzimitl', 'Hell Sentinel', 'Executioner', 'Hellbinder',
        'Cloud Mage', 'Statue of Wucad Mu', 'mad acolyte of Lugonu'
    ]

    if not re.search('[A-Z]', string):
        return False

    for special in specials:
        if special in string:
            return False;

    return True

def is_unique_noun(string, is_monster = False):
    if is_monster:
        return is_unique_monster(string)
    elif has_article_the(string):
        return True
    elif re.search("[A-Z]", string):
        return True
    else:
        return False

def get_noun_permutations(string, is_monster = False):
    list = []

    if string.startswith("The "):
        list.append(string)
        if is_monster:
            list.append(string + "'s")
    else:
        is_unique = is_unique_noun(string, is_monster)
        full = article_the(string)
        list.append(full)

        # possessive (for monsters)
        if is_monster:
            if is_unique:
                list.append(possessive(string))
            else:
                list.append(possessive(full))

    return list

def append_monster_permutations(list, string):
    list.extend(get_noun_permutations(string, True))

def is_missile(string):
    for s in ["dart", "boomerang", "javelin", "throwing net", "stone", "large rock", "bullet", "arrow", "bolt"]:
        if string.endswith(s):
            return True
    return False

def is_spellbook(string):
    if string.startswith("book of ") and not string == "book of ":
        return True

    return string in [
        "Necronomicon",
        "Grand Grimoire",
        "Everburning Encyclopedia",
        "Ozocubu's Autobiography",
        "Young Poisoner's Handbook",
        "Fen Folio",
        "Inescapable Atlas",
        "There-And-Back Book",
        "Great Wizards, Vol. II",
        "Great Wizards, Vol. VII",
        "Trismegistus Codex",
        "the Unrestrained Analects",
    ]

def add_spellbook_article(string):
    if has_article_the(string):
        # already has article
        return string
    elif string.startswith("Great Wizards") or string == "Ozocubu's Autobiography":
        # never gets an article
        return string
    else:
        # can have a/an
        return article_a(string)

def post_process_art_func_h(strings):
    result = []
    section = None
    verbs = []
    adverbs = []
    for string in strings:
        if string.startswith('# section:'):
            if len(verbs) != 0:
                if len(adverbs) < len(verbs):
                    adverbs.append("")
                result.extend(do_any_2_actors_messages(verbs, adverbs))
                verbs = []
                adverbs = []
            section = string.replace('# section:', '').strip()
            result.append(string)
        elif section == '_WOE_melee_effects':
            string = remove_enclosing_quotes(string)
            if string == '.':
                continue
            elif string.startswith(' '):
                adverbs.append(string)
            else:
                if len(adverbs) < len(verbs):
                    adverbs.append("")
                verbs.append(string)
        elif section == '_ELEMENTAL_STAFF_melee_effects':
            result.extend(do_any_2_actors_message(string, ''))
        elif string.endswith('ing Sword'):
            # Singing Sword names
            result.append(article_the(string))
        else:
            result.append(string)

    return result

def post_process_directn_cc(strings):
    result = []
    section = None
    for string in strings:
        if string.startswith('# section:'):
            # new section starts
            section = string.replace('# section:', '').strip()
        elif string.startswith('#'):
            string = string
        elif section == "_base_feature_desc":
            string = article_the(string)
        elif section == 'feature_description_at' and string.endswith(' '):
            result.extend(separate_adjectives(string))
            continue
        result.append(string)

    return result

def post_process_feature_data_h(strings):
    output = []
    adjectives = []
    for string in strings:
        if string.startswith('#'):
            output.append(string)
        elif string.endswith(' door') or string.endswith(' gate'):
            # we handle door adjectives as separate strings
            words = separate_adjectives_and_noun(string)
            for i in range(len(words)):
                if i == len(words) - 1:
                    output.append(words[i]);
                else:
                    adjectives.append(words[i])
        elif string.endswith("golubria"):
            # the version with a small g is an internal id
            continue
        elif string in ['explore horizon', 'unseen']:
            output.append(string)
        elif string.startswith('some '):
            output.append(string)
        else:
            output.append(article_the(string))

    output.append("# section: door/gate adjectives")
    for string in adjectives:
        output.append(string)

    # do we need plurals?

    return output

def post_process_invent_cc(strings):
    result = []
    section = None
    for string in strings:
        if string.startswith('# section:'):
            # new section starts
            section = string.replace('# section:', '').strip()
        elif string.startswith('#'):
            string = string
        elif section == '_operation_verb':
            string = 'Really ' + string + ' %s?'
        elif section == 'check_warning_inscriptions':
            if string in ['Really ', ' %s?']:
                continue
        result.append(string)

    return result

# you'd think from the filename that everything in here would be a name, but you'd be wrong
def post_process_item_name_cc(strings):
    result = []
    extras1 = []
    extras2 = []
    section = ''

    for string in strings:
        if string.startswith('# section:'):
            # new section starts
            result.extend(extras1)
            result.extend(extras2)
            extras1 = []
            extras2 = []
            section = string.replace('# section:', '').strip()
        elif string.startswith('#'):
            string = string # null op
        elif section in ['_random_vowel', '_random_cons', '_random_consonant_set', 'make_name']:
            # random-name generation - ignore
            continue
        elif '_test' in section:
            # test stuff
            continue
        elif section.endswith('_secondary_string') or section == 'staff_primary_string':
            # extract adjective as separate word
            if not string.endswith(' '):
                string += ' '
        elif section.endswith('_primary_string'):
            # primary adjective (closest to the noun)
            noun = re.sub('_.*', '', section)
            if not string.endswith(' '):
                string += ' '
            string = article_the(string + noun)
        elif section == 'item_def::name':
            if string == ' (in ':
                result.append(' (in hand)')
                result.append(' (in claw)')
                result.append(' (in tentacle)')
                continue
            elif string in ['right', 'left']:
                result.append(' (' + string + ' hand)');
                result.append(' (' + string + ' claw)');
                result.append(' (' + string + ' paw)');
                result.append(' (' + string + ' tentacle)');
                result.append(' (' + string + ' branch)');
                result.append(' (' + string + ' front leg)');
                result.append(' (' + string + ' blade hand)');
                continue
        elif section == 'missile_brand_name':
            # these have a long form and a terse form
            # the long form is used either as an adjective or a suffix
            # the terse form is used as an annotation
            if string == 'poisoned' or string.endswith('-tipped'):
                # adjective used only on darts - expand all possibilities
                result.append(article_the(string + ' dart'))
                result.append('%d ' + string + ' darts')
            elif string == 'dispersal' or string.endswith('ing') or string.endswith('ion'):
                # used as suffix
                result.append(' of ' + string)
            else:
                # terse form (used as annotation)
                result.append(string)
                if string == 'silver':
                    # is also the long form - used as adjective
                    result.append(string + ' ')
                elif string == 'chaos':
                    # is also the long form - used as suffix
                    result.append(' of ' + string)
            continue
        elif section == 'weapon_brands_terse':
            if string == 'confuse':
                # not a real weapon brand - used on hands for confusing touch
                continue
            elif string == 'flame':
                # terse version also used after "of" (see _item_ego_name in religion.cc)
                result.append(' of ' + string)
        elif section == 'weapon_brands_verbose':
            if string == 'confusion':
                # not a real weapon brand - used on hands for confusing touch
                continue
            elif string in ['vampirism', 'antimagic', 'vorpality', 'spectralizing']:
                # verbose name is never used (see brand_prefers_adj)
                continue
            string = ' of ' + string
        elif section == 'weapon_brands_adj':
            # adjectives defined for all, but only used for some (see brand_prefers_adj)
            if string in ['vampiric', 'antimagic', 'vorpal', 'spectral']:
                string = string + ' '
            else:
                continue
        elif section == 'armour_ego_name':
            string = ' of ' + string
        elif section == 'armour_ego_name(terse)':
            if string == 'rC+ rF+':
                # handled as two separate strings
                continue
            # the plus is handled separately
            string = re.sub(r'\+.*', '', string)
        elif section == '_wand_type_name':
            string = 'wand of ' + string
            if not string.endswith('removedness'):
                # uncounted plural for known items menu
                extras1.append(pluralise(string))
            string = article_the(string)
        elif section == 'potion_type_name':
            string = 'potion of ' + string
            # counted plural for stacks
            extras2.append('%d ' + pluralise(string))
            string = article_the(string)
        elif section == 'scroll_type_name':
            string = 'scroll of ' + string
            # counted plural for stacks
            extras2.append('%d ' + pluralise(string))
            string = article_the(string)
        elif section == 'jewellery_effect_name':
            string = ' of ' + string
        elif section == 'jewellery_effect_name(terse)':
            # the plus is handled separately
            string = re.sub(r'\+.*', '', string)
        elif section == 'rune_type_name':
            if string in ['mossy', 'elven']:
                # obsolete
                continue
            else:
                extras1.append(article_the(string + ' rune'))
        elif section == 'misc_type_name':
            # uncounted plural for known items menu
            if string != 'horn of Geryon':
                extras1.append(pluralise(string))
            string = article_the(string)
        elif section == '_book_type_name':
            if string == 'Fixed Level' or string == 'Fixed Theme':
                continue
            string = 'a book of ' + string
        elif section == 'sub_type_string':
            if string == 'manual':
                result.append(string)
                string = pluralise(string)
            elif is_spellbook(string):
                string = add_spellbook_article(string)
        elif section == 'staff_type_name':
            extras1.append('staves of ' + string)
            string = 'the staff of ' + string
        elif section == 'ghost_brand_name':
            if string == '%s weapon':
                string = 'the weapon'
            elif string == 'weapon of %s':
                # suffixes handles separately
                continue
            elif string == '%s touch':
                # there's only one possibility
                string = 'confusing touch'
        elif section == 'potion_colours':
            if not string.endswith(' '):
                string += ' '
        elif section == 'display_runes':
            if string == "green":
                # text colour tag
                continue
        elif section == 'item_prefix':
            # undisplayed, but (supposedly) searchable prefixes
            # many of these don't even work in English
            continue

        if string in ['wand of ', 'potion of ', 'scroll of', 'ring of', 'amulet of', 'staff of ', 'book of ']:
            # all subtypes already covered above
            continue
        elif string in [' wand', ' potion', ' ring', ' amulet', ' rune']:
            # all subtypes already covered above
            continue
        elif string == "Orb of Zot":
            string = article_the(string)
        elif string in ['manual of ', '%s of %s', ' of ', 'of '] or (string.endswith(' of Zot') and string != "The Orb of Zot"):
            # other "of <foo>" suffixes are handled separately
            continue
        elif string == "gold piece":
            result.append(article_the(string))
            result.append('%d ' + pluralise(string))
            continue
        elif string == 'enchanted %s':
            # will be handled the other way round, with "enchanted" as added adjective
            string = 'enchanted '
        elif string == "damnation ":
            # there's only one possibility
            result.append("the damnation bolt")
            result.append('%d damnation bolts')
            continue
        elif string == "labelled ":
            result.append("the scroll labelled %s")
            string = "%d scrolls labelled %s"
        elif string == "x) ":
            # ignore - just used for size
            continue
        elif string == "pair of ":
            # handled in item-prop.cc
            continue
        elif string == "decaying skeleton":
            # dbname (just used as a lookup key, not displayed)
            continue
        elif "bug" in string or "bad item" in string or "bogus" in string:
            # case that should never happen - ignore
            continue

        result.append(string)

    result.extend(extras1)
    result.extend(extras2)

    return result

# this is where most of the item names are
def post_process_item_prop_cc(strings):
    output = []
    plurals = []

    for string in strings:
        if string.startswith('#'):
            # comment
            output.append(string)
            continue
        elif string in ['steam', 'acid', 'quicksilver', 'swamp', 'fire', 'ice', 'pearl', 'storm', 'shadow', 'gold']:
            string = string + ' dragon scales'
        elif string == ' dragon scales':
            # all possibilities covered above
            continue
        elif string in ['gloves', 'boots']:
            string = 'pair of ' + string
        elif string in ['javelin', 'boomerang']:
            output.append(article_the(string))
            string = 'silver ' + string

        # stackable items need a plural with count
        if is_missile(string):
            plurals.append('%d ' + pluralise(string))

        output.append(article_the(string))

    output.extend(plurals)
    return output

def post_process_job_data_h(strings):
    output = []
    for string in strings:
        if len(string) > 2 and not string.startswith('#'):
            output.append(article_the(string))
        else:
            output.append(string)
    return output;

def post_process_mon_data_h(strings):
    output = []
    names = []
    unique_names = []
    adjectives = []

    # separate unqiue from non-unique
    for string in strings:
        if string.startswith('#'):
            continue
        if string.endswith(' '):
            adjectives.append(string)
        elif is_unique_monster(string):
            unique_names.append(string)
        else:
            names.append(string)

    names.sort()
    unique_names.sort()

    # adjectives
    output.append("# section: adjectives")
    for string in adjectives:
        output.append(string)

    # singular non-unique
    output.append("# section: non-unique monsters, singular")
    for string in names:
        output.append(article_the(string))

    # singular unique
    output.append("# section: unique monsters")
    for string in unique_names:
        output.append(article_the(string))

    # possessive non-unique
    output.append("# section: non-unique monsters, singular possessive")
    for string in names:
        output.append(article_the(possessive(string)))

    # possessive unique
    output.append("# section: unique monsters, possessive")
    for string in unique_names:
        output.append(possessive(string))

    # plural non-unique
    output.append("# section: non-unique monsters, plural")
    for string in names:
        output.append('%d ' + pluralise(string))

    return output

def post_process_mutant_beast_h(strings):
    facets = []
    output = []
    section = None
    for string in strings:
        if string.startswith('# section:'):
            section = string.replace('# section:', '').strip()
        elif section == "mutant_beast_facet_names":
            facets.append(string)
        elif section == "mutant_beast_tier_names":
            string += " "
            output.append(string)

    facets.sort()
    for facet1 in facets:
        for facet2 in facets:
            if facet2 != facet1:
                output.append(article_the(facet1 + facet2 + " beast"))

    return output

def post_process_skills_cc(strings):
    # extract weight classes
    weight_classes = []
    section = None
    for string in strings:
        if string.startswith('# section:'):
            if section == '_stk_weight':
                break
            section = string.replace('# section:', '').strip()
        elif section == '_stk_weight':
            weight_classes.append(string)

    # expand weight classes
    result = []
    section = None
    for string in strings:
        if string.startswith('# section:'):
            section = string.replace('# section:', '').strip()
            result.append(string)
        elif section == '_stk_weight':
            continue
        elif '@Weight@' in string and string != '@Weight@':
            result.append('# note: expand "' + string + '"')
            for weight in weight_classes:
                result.append(string.replace('@Weight@', weight))
        else:
            result.append(string)

    return result

def post_process_tilereg_cc(strings):
    result = []
    section = None
    for string in strings:
        if string.startswith('# section:'):
            section = string.replace('# section:', '').strip()
            result.append(string)
        elif section == "DungeonRegion::update_tip_text" and string != "out of sight":
            # wizard mode debug stuff
            continue
        elif string.startswith("["):
            # split mouse click from description of what it does
            pos = string.find(']')
            if pos > 0:
                mouse_action = string[0:pos+1]
                rest = string[pos+1:]
                result.append(mouse_action)
                result.append(rest.strip())
        else:
            result.append(string)

    # remove hotkey placeholders
    temp = result
    result = []
    for string in temp:
        if "(%" in string:
            string = re.sub(r'\(%.*', '', string).strip()
        result.append(string)

    return result

def post_process_transform_cc(strings):
    result = []
    section = None
    for string in strings:
        if string.startswith('# section:'):
            section = re.sub(r'^# section:\s*', '', string)
        elif section == "FormAppendage":
            # remove space at end of sentence
            string = string.strip()
        elif string == "Stone %s":
            # statue form unarmed attack
            for fist in ["fist", "paw", "claw", "tentacle"]:
                stone_fist = string.replace("%s", fist)
                result.append(stone_fist)
                result.append(pluralise(stone_fist))
            continue
        elif string == "hover solemnly before":
            # flying prayer action
            string = PRAY_SENTENCE.replace("%s", string, 1)
        elif re.search('^(a|an|your) ', string):
            # transform description
            # remove punctuation
            string = re.sub(r'[\.!]$', '', string)
            if 'fearsome %s' in string:
                # dragon form
                # draconians get a specific dragon based on their colour
                result.append(string.replace("%s", "ice dragon"))
                result.append(string.replace("%s", "swamp dragon"))
                result.append(string.replace("%s", "golden dragon"))
                result.append(string.replace("%s", "iron dragon"))
                result.append(string.replace("%s", "storm dragon"))
                result.append(string.replace("%s", "quicksilver dragon"))
                result.append(string.replace("%s", "steam dragon"))
                result.append(string.replace("%s", "fire dragon"))
                continue
        result.append(string)
    return result

specific_post_processing_funcs = {
    'art-func.h': post_process_art_func_h,
    'directn.cc': post_process_directn_cc,
    'feature-data.h': post_process_feature_data_h,
    'invent.cc': post_process_invent_cc,
    'item-name.cc': post_process_item_name_cc,
    'item-prop.cc': post_process_item_prop_cc,
    'job-data.h': post_process_job_data_h,
    'mon-data.h': post_process_mon_data_h,
    'mutant-beast.h': post_process_mutant_beast_h,
    'skills.cc': post_process_skills_cc,
    'transform.cc': post_process_transform_cc,
}

def post_process(filename, strings):
    # the strings in some files need special handling
    if filename in specific_post_processing_funcs:
        func = specific_post_processing_funcs[filename]
        strings = func(strings)
    elif filename.startswith("tilereg-") and filename.endswith(".cc"):
        strings = post_process_tilereg_cc(strings)
    elif filename != 'art-data.txt':
        section = None
        old_strings = strings
        strings = []
        for string in old_strings:
            if string.startswith('# note:'):
                strings.append(string)
            elif string.startswith('# section:'):
                strings.append(string)
                section = string.replace('# section: ', '')
            elif filename == 'player.cc' and string == "%sway":
                strings.append("the doorway")
                strings.append("the gateway")
            elif string == "Walk":
                # species walk verb and associated noun
                strings.append(string + "ing")
                strings.append(string + "er")
            elif string == "runed door":
                # should be covered by feature-data.h, but just in case...
                words = separate_adjectives_and_noun(string)
                strings.extend(words)
            elif re.match('^ ?shaped ', string):
                # separate "shaped" out as an adjective
                strings.append("shaped ");
                strings.append("@monster@ shaped ");
                string = re.sub('^ ?shaped ', '', string)
                append_monster_permutations(strings, string)
            elif string in ["spectre", "wavering orb of destruction"]:
                # treat like monsters in mon-data.h
                append_monster_permutations(strings, string)
            elif string == " the pandemonium lord":
                strings.append(string.strip())
            elif string in ["Blork", "gate", "deep water"]:
                strings.append(article_the(string))
            elif string == "deck of " or string == "decks of ":
                if string == "deck of ":
                    string = article_the(string)
                for suffix in ["destruction", "escape", "summoning", "punishment"]:
                    strings.append(string + suffix);
            elif string == "your stacked deck":
                strings.append("the stacked deck")
            elif "@medium_attack@" in string and len(medium_attack_verbs) > 0:
                for verb in medium_attack_verbs:
                    strings.append(string.replace("@medium_attack@", verb))
            elif "@Medium_attack@" in string and len(medium_attack_verbs) > 0:
                for verb in medium_attack_verbs:
                    strings.append(string.replace("@Medium_attack@", verb.capitalize()))
            elif string == "kneel at":
                # default prayer action
                string = PRAY_SENTENCE.replace("%s", string, 1)
                strings.append(string)
            elif string == PRAY_SENTENCE:
                continue
            elif filename == "arena.cc":
                if " v " in string and not "Expected" in string:
                    continue
                elif string in ["default", " (A)", " (B)"]:
                    continue
                strings.append(string)
            elif filename == "religion.cc" and section == "_item_ego_name":
                strings.append(" of " + string);
            elif filename == "items.cc" and string == "{gold}":
                # annotation?
                strings.append(re.sub(r'[\{\}]' ,'', string))
            else:
                strings.append(string)


    # remove duplicates and strings that should be ignored
    old_strings = strings
    strings = []
    for string in old_strings:
        if string.startswith("# section") or string.startswith("# note"):
            strings.append(string)
        elif not ignore_string(string) and string not in strings:
            strings.append(string)
        elif len(strings) > 0 and strings[-1].startswith('# note:'):
            strings.pop()

    strings = remove_unnecessary_section_markers(strings)

    return strings

#################
# Main
#################

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

    yaml_files = glob.glob("dat/species/*.yaml")
    yaml_deprec = glob.glob("dat/species/*deprecated*.yaml")
    yaml_files = list(set(yaml_files) - set(yaml_deprec))
    yaml_files.sort()

    lua_files = glob.glob("dat/clua/*.lua")
    lua_files.append("dat/dlua/lm_timed.lua")
    lua_files.append("dat/dlua/lm_tmsg.lua")
    lua_files.append("dat/dlua/lm_trove.lua")
    lua_files.sort()
    source_files.extend(lua_files)

    des_files = glob.glob("dat/des/*/*.des")
    des_files.sort()
    source_files.extend(des_files)

    # put some important files first
    # (because if there are duplicate strings, we want them put under these files)
    files = SPECIAL_FILES.copy()
    files.extend(yaml_files)

    # add wanted source files to list to be processed
    for fname in source_files:
        if fname not in files and \
           fname not in SKIP_FILES and \
           not re.match('l-', fname) and \
           not re.match('dbg-', fname):
            files.append(fname)

output = []

for filename in files:

    strings = []
    if filename == 'art-data.txt':
        strings = process_art_data_txt()
    elif filename == 'form-data.h':
        strings = process_form_data_h(filename)
    elif filename == 'sacrifice-data.h':
        strings = process_sacrifice_data_h(filename)
    elif filename.endswith('.yaml'):
        strings = process_yaml_file(filename)
    elif filename.endswith('.lua') or filename.endswith('.des'):
        strings = process_des_or_lua_file(filename)
    else:
        strings = process_cplusplus_file(filename)

    strings = post_process(filename, strings)

    add_strings_to_output(filename, strings, output)

output = remove_derived_duplicates(output)

for line in output:
    print(line)
