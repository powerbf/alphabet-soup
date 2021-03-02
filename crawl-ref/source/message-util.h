#pragma once
/*
 * Message utility functions
 */

#include "actor.h"

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


string get_monster_message(const actor* subject, bool subject_seen,
                           const actor* object, bool object_seen,
                           const string& you_obj_msg,
                           const string& other_msg);

void do_monster_message(const actor* subject, bool subject_seen,
                        const actor* object, bool object_seen,
                        const string& you_obj_msg,
                        const string& other_msg);

string get_simple_message(const string& subject, const string& object,
                          const string& you_subj_msg,
                          const string& you_obj_msg,
                          const string& other_msg);

void do_simple_message(const string& subject, const string& object,
                       const string& you_subj_msg,
                       const string& you_obj_msg,
                       const string& other_msg);

