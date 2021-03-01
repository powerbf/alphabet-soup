/*
 * Actor utility functions
 */

#include "AppHdr.h"
#include "actor-util.h"
#include "english.h"
#include "gender-type.h"
#include "localize.h"
#include "mpr.h"

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
 * Get localized string for an actor doing something
 *
 * subject = the actor doing the action
 * seen = is the subject seen?
 * you_msg = msg to be used if actor is player (expect no %s)
 * other_msg = msg to be used otherwise (expect one %s)
 */
string actor_action_string(const actor* subject, bool seen,
                           const string& you_msg,
                           const string& other_msg)
{
    if (subject && subject->is_player())
        return localize(you_msg);
    else
    {
        string subj = actor_name(subject, DESC_THE, seen);
        return localize(other_msg, subj);
    }
}

/*
 * Get localized string for an actor doing something to another actor
 * (Must be two different actors)
 *
 * subject = the actor doing the action
 * subject_seen = is the subject seen?
 * object = the other actor
 * subject_seen = is the object seen?
 * you_subj_msg = msg to be used if subject is player (expect one %s)
 * you_obj_msg = msg to be used if object is player (expect one %s)
 * other_msg = msg to be used otherwise (expect two %s)
 */
string actor_action_string(const actor* subject, bool subject_seen,
                           const actor* object, bool object_seen,
                           const string& you_subj_msg,
                           const string& you_obj_msg,
                           const string& other_msg)

{
    string msg;
    if (subject && subject->is_player())
    {
        string obj = actor_name(object, DESC_THE, object_seen);
        msg = localize(you_subj_msg, obj);
    }
    else if (object && object->is_player())
    {
        string subj = actor_name(subject, DESC_THE, subject_seen);
        msg = localize(you_obj_msg, subj);
    }
    else
    {
        string subj = actor_name(subject, DESC_THE, subject_seen);
        string obj = actor_name(object, DESC_THE, object_seen);
        msg = localize(other_msg, subj, obj);
    }

    if (subject && subject == object)
    {
        // reflexive (acting on self) - we didn't expect that here
        msg += " (bug: reflexive not expected here)"; // noextract
    }

    return msg;
}

/*
 * Get localized string for an actor doing something to actor
 * (Can be itself)
 *
 * subject = the actor doing the action
 * subject_seen = is the subject seen?
 * object = the other actor
 * subject_seen = is the object seen?
 * you_subj_msg = msg to be used if subject is player (expect one %s)
 * you_obj_msg = msg to be used if object is player (expect one %s)
 * other_msg = msg when subject and object are 2 different 3rd parties (expect two %s)
 * yourself_msg = msg to be used if player doing something to self (expect no %s)
 * itself_msg = msg to be used if 3rd party doing something to self (expect 1 or 2 %s)
 *              (if present, 2nd %s will be replaced by reflexive pronoun,
 *               but you should avoid this because it's hard to translate with the right gender.
 *               It's preferable to rephrase without reflexive pronoun (e.g. passive voice)))
 */
string actor_action_string(const actor* subject, bool subject_seen,
                           const actor* object, bool object_seen,
                           const string& you_subj_msg,
                           const string& you_obj_msg,
                           const string& other_msg,
                           const string& yourself_msg,
                           const string& itself_msg)
{
    if (subject && subject == object)
    {
        // reflexive (acting on self)
        if (subject->is_player())
        {
            return localize(yourself_msg);
        }
        else
        {
            string subj = actor_name(subject, DESC_THE, subject_seen);
            string obj = actor_pronoun(subject, PRONOUN_REFLEXIVE, subject_seen);
            return localize(itself_msg, subj, obj);
        }
    }
    else
    {
        return actor_action_string(subject, subject_seen,
                                   object, object_seen,
                                   you_subj_msg, you_obj_msg, other_msg);
    }
}


void actor_action_message(const actor* subject, bool seen,
                          const string& you_msg,
                          const string& other_msg)
{
    string msg = actor_action_string(subject, seen, you_msg, other_msg);
    mpr_nolocalize(msg);
}

void actor_action_message(const actor* subject, bool subject_seen,
                          const actor* object, bool object_seen,
                          const string& you_subj_msg,
                          const string& you_obj_msg,
                          const string& other_msg)
{
    string msg = actor_action_string(subject, subject_seen,
                                     object, object_seen,
                                     you_subj_msg, you_obj_msg, other_msg);
    mpr_nolocalize(msg);
}

void actor_action_message(const actor* subject, bool subject_seen,
                          const actor* object, bool object_seen,
                          const string& you_subj_msg,
                          const string& you_obj_msg,
                          const string& other_msg,
                          const string& yourself_msg,
                          const string& itself_msg)
{
    string msg = actor_action_string(subject, subject_seen,
                                     object, object_seen,
                                     you_subj_msg, you_obj_msg, other_msg,
                                     yourself_msg, itself_msg);
    mpr_nolocalize(msg);
}

