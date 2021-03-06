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
    {
        // We need to insert message into punctuation rather than merely append
        // because, for example, English "%s!" becomes "¡%s!" in Spanish.
        // Note: The message is already localized, so we don't localize again.
        if (contains(punctuation, "%s"))
            msg = localize(punctuation, LocalizationArg(msg, false));
        else
            msg = localize("%s" + punctuation, LocalizationArg(msg, false));
    }

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
