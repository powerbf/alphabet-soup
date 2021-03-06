/*
 * Message utility functions
 */

#include "AppHdr.h"
#include "english.h"
#include "gender-type.h"
#include "localize.h"
#include "message-util.h"
#include "mpr.h"
#include "stringutil.h"
#include "variant-msg.h"

/*
 * Add punctuation to a message
 */
string add_punctuation(const string& msg, const string& punctuation, bool localize_msg)
{
    // We need to insert message into punctuation rather than merely append
    // because, for example, English "%s!" becomes "¡%s!" in Spanish.
    // Note: If message is already localized, we don't localize again.
    if (contains(punctuation, "%s"))
        return localize(punctuation, LocalizationArg(msg, localize_msg));
    else
        return localize("%s" + punctuation, LocalizationArg(msg, localize_msg));
}

/* Returns an actor's name
 *
 * Takes into account actor visibility/invisibility and the type of description
 * to be used (definite, indefinite, possessive, etc.)
 */
string actor_name(const actor *a, description_level_type desc,
                  bool actor_visible)
{
    if (!a)
        return "null"; // bug, noextract
    return actor_visible ? a->name(desc) : anon_name(desc);
}

/* Returns an actor's pronoun
 *
 * Takes into account actor visibility
 */
string actor_pronoun(const actor *a, pronoun_type pron,
                     bool actor_visible)
{
    if (!a)
        return "null"; // bug, noextract
    return actor_visible ? a->pronoun(pron) : anon_pronoun(pron);
}

/*
 * Returns an anonymous actor's name
 *
 * Given invisibility (whether out of LOS or just invisible),
 * returns the appropriate noun.
 */
string anon_name(description_level_type desc)
{
    switch (desc)
    {
    case DESC_NONE:
        return "";
    case DESC_YOUR:
    case DESC_ITS:
        return "something's";
    case DESC_THE:
    case DESC_A:
    case DESC_PLAIN:
    default:
        return "something";
    }
}

/*
 * Returns an anonymous actor's pronoun
 *
 * Given invisibility (whether out of LOS or just invisible),
 * returns the appropriate pronoun.
 */
string anon_pronoun(pronoun_type pron)
{
    return decline_pronoun(GENDER_NEUTER, pron);
}

/*
 * Get message where subject and object can be any combination of 2nd or 3rd person
 * (1st person doesn't exist in this game)
 */
string get_any_person_message(variant_msg_type msg_id,
                              const string& subject, const string& object,
                              const string& punctuation)
{
    string subj = lowercase_string(subject);
    string obj = lowercase_string(object);
    msg_variant_type variant;

    if (subj == "you")
    {
        if (obj == "you" || obj == "yourself")
        {
            variant = MV_YOURSELF;
        }
        else
        {
            variant = MV_YOU_SUBJECT;
        }
    }
    else if (obj == "you")
    {
        variant = MV_YOU_OBJECT;
    }
    else if (obj == "itself" || obj == "himself" || obj == "herself")
    {
        variant = MV_ITSELF;
    }
    else
    {
        variant = MV_THIRD_PARTIES;
    }

    const string& temp = get_variant_template(msg_id, variant);

    string msg;
    if (starts_with(temp, "ERROR"))
    {
        msg = temp + ", subj=\"" + subj + "\", obj=\"" + obj + "\""; // noextract
        return msg;
    }

    if (variant == MV_YOU_SUBJECT)
        msg = localize(temp, obj);
    else if (variant == MV_YOU_OBJECT)
        msg = localize(temp, subj);
    else if (variant == MV_THIRD_PARTIES)
        msg = localize(temp, subj, obj);
    else if (variant == MV_YOURSELF)
        msg = localize(temp);
    else if (variant == MV_ITSELF)
        msg = localize(temp, subj, obj);

    if (!punctuation.empty())
        msg = add_punctuation(msg, punctuation, false);

    return msg;
}

/*
 * Get message where subject and object can be any combination of 2nd or 3rd person
 * (1st person doesn't exist in this game)
 */
string get_any_person_message(variant_msg_type msg_id,
                              const actor* subject, const actor* object,
                              const string& punctuation)
{
    return get_any_person_message(msg_id, subject, object, true, true, punctuation);
}

/*
 * Get message where subject and object can be any combination of 2nd or 3rd person
 * (1st person doesn't exist in this game)
 */
string get_any_person_message(variant_msg_type msg_id,
                              const actor* subject, const actor* object,
                              bool subject_seen, bool object_seen,
                              const string& punctuation)
{
    string subj = actor_name(subject, DESC_THE, subject_seen);
    string obj;
    if (object && object == subject)
        obj = actor_pronoun(object, PRONOUN_REFLEXIVE, object_seen);
    else
        obj = actor_name(object, DESC_THE, object_seen);
    return get_any_person_message(msg_id, subj, obj, punctuation);
}

/*
 * Output message where subject and object can be any combination of 2nd or 3rd person
 * (1st person doesn't exist in this game)
 */
void do_any_person_message(variant_msg_type msg_id,
                           const string& subject, const string& object,
                           const string& punctuation)
{
    mpr_nolocalize(get_any_person_message(msg_id, subject, object, punctuation));
}

/*
 * Output message where subject and object can be any combination of 2nd or 3rd person
 * (1st person doesn't exist in this game)
 */
void do_any_person_message(variant_msg_type msg_id,
                           const actor* subject, const actor* object,
                           const string& punctuation)
{
    mpr_nolocalize(get_any_person_message(msg_id, subject, object, punctuation));
}

/*
 * Output message where subject and object can be any combination of 2nd or 3rd person
 * (1st person doesn't exist in this game)
 */
void do_any_person_message(variant_msg_type msg_id,
                           const actor* subject, const actor* object,
                           bool subject_seen, bool object_seen,
                           const string& punctuation)
{
    mpr_nolocalize(get_any_person_message(msg_id, subject, object,
                                       subject_seen, object_seen,
                                       punctuation));
}

/*
 * Get message where subject is guaranteed to be 3rd person
 * (Object can be 2nd or 3rd person)
 */
string get_3rd_person_message(const actor* subject, bool subject_seen,
                              const actor* object, bool object_seen,
                              const string& you_obj_msg,
                              const string& other_msg,
                              const string& punctuation)
{
    string subj = actor_name(subject, DESC_THE, subject_seen);

    string msg;
    if (object && object->is_player())
    {
        msg = localize(you_obj_msg, subj);
    }
    else
    {
        string obj = actor_name(object, DESC_THE, object_seen);
        msg = localize(other_msg, subj, obj);
    }

    if (!punctuation.empty())
        msg = add_punctuation(msg, punctuation, false);

    if (subject && subject->is_player())
    {
        msg += " (bug: 2nd person subject unexpected here)"; // noextract
    }

    if (subject && subject == object)
    {
        // reflexive (acting on self) - we didn't expect that here
        msg += " (bug: reflexive unexpected here)"; // noextract
    }

    return msg;
}

/*
 * Output message where subject is guaranteed to be 3rd person
 * (Object can be 2nd or 3rd person)
 */
void do_3rd_person_message(const actor* subject, bool subject_seen,
                           const actor* object, bool object_seen,
                           const string& you_obj_msg,
                           const string& other_msg,
                           const string& punctuation)
{
    string msg = get_3rd_person_message(subject, subject_seen,
                                        object, object_seen,
                                        you_obj_msg, other_msg, punctuation);

    if (!msg.empty())
        mpr_nolocalize(msg);
}
