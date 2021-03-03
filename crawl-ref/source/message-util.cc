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
static string get_actor_message(const actor* subject, bool subject_seen,
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
 * Like get_actor_message, except subject must be a monster
 */
string get_monster_message(const actor* subject, bool subject_seen,
                           const actor* object, bool object_seen,
                           const string& you_obj_msg,
                           const string& other_msg)
{
    if (subject && subject->is_player())
        return "";

    return get_actor_message(subject, subject_seen,
                             object, object_seen,
                             "", you_obj_msg, other_msg);
}

/*
 * Like do_actor_message, except subject must be a monster
 */
void do_monster_message(const actor* subject, bool subject_seen,
                        const actor* object, bool object_seen,
                        const string& you_obj_msg,
                        const string& other_msg)
{
    string msg = get_monster_message(subject, subject_seen,
                                     object, object_seen,
                                     you_obj_msg, other_msg);

    if (!msg.empty())
        mpr_nolocalize(msg);
}

/*
 * Get localized string for a message containing subject and object
 *
 * subject = the subject of the sentence
 * object = the object of the sentence
 * you_subj_msg = msg to be used if subject is "you" (expect one %s)
 * you_obj_msg = msg to be used if object is "you" (expect one %s)
 * other_msg = msg to be used otherwise (expect two %s)
 */
string get_simple_message(const string& subject, const string& object,
                          const string& you_subj_msg,
                          const string& you_obj_msg,
                          const string& other_msg)
{
    string msg;
    if (lowercase_string(subject) == "you")
        return localize(you_subj_msg, object);
    else if (lowercase_string(object) == "you")
        return localize(you_obj_msg, subject);
    else
        return localize(other_msg, subject, object);
}

/*
 * Show localized message containing subject and object
 *
 * subject = the subject of the sentence
 * object = the object of the sentence
 * you_subj_msg = msg to be used if subject is "you" (expect one %s)
 * you_obj_msg = msg to be used if object is "you" (expect one %s)
 * other_msg = msg to be used otherwise (expect two %s)
 */
void do_simple_message(const string& subject, const string& object,
                       const string& you_subj_msg,
                       const string& you_obj_msg,
                       const string& other_msg)
{
    mpr_nolocalize(get_simple_message(subject, object, you_subj_msg,
                                      you_obj_msg, other_msg));
}
