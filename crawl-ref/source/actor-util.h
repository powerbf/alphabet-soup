#pragma once
/*
 * Actor utility functions
 */

#include "actor.h"

string anon_name(description_level_type desc);

string anon_pronoun(pronoun_type ptyp);

string actor_name(const actor *a, description_level_type desc,
                  bool actor_visible);

string actor_pronoun(const actor *a, pronoun_type ptyp, bool actor_visible);


string get_actor_message(const actor* subject, bool seen,
                         const string& you_msg,
                         const string& other_msg);

string get_actor_message(const actor* subject, bool subject_seen,
                         const actor* object, bool object_seen,
                         const string& you_subj_msg,
                         const string& you_obj_msg,
                         const string& other_msg);

string get_actor_message(const actor* subject, bool subject_seen,
                         const actor* object, bool object_seen,
                         const string& you_subj_msg,
                         const string& you_obj_msg,
                         const string& other_msg,
                         const string& yourself_msg,
                         const string& itself_msg);


void do_actor_message(const actor* subject, bool seen,
                          const string& you_msg,
                          const string& other_msg);

void do_actor_message(const actor* subject, bool subject_seen,
                      const actor* object, bool object_seen,
                      const string& you_subj_msg,
                      const string& you_obj_msg,
                      const string& other_msg);

void do_actor_message(const actor* subject, bool subject_seen,
                      const actor* object, bool object_seen,
                      const string& you_subj_msg,
                      const string& you_obj_msg,
                      const string& other_msg,
                      const string& yourself_msg,
                      const string& itself_msg);
