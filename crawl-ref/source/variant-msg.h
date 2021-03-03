/*
 * Variant messages
 * Messages that change based on the grammatical person of the subject and object
 * (e.g. "You hit <the monster>", "<The monster> hits you", "<The monster> hits <the monster>")
 */
#pragma once

#include <string>
using namespace std;

#include "actor.h"
#include "variant-msg-type.h"

const string& get_variant_template(variant_msg_type msg_id, msg_variant_type variant);

string get_variant_message(variant_msg_type msg_id,
                           const string& subject, const string& object = "");

void do_variant_message(variant_msg_type msg_id,
                        const string& subject, const string& object = "");

string get_variant_message(variant_msg_type msg_id,
                           const actor* subject, const actor* object = nullptr,
                           bool subject_visible = true, bool object_visible = true);

void do_variant_message(variant_msg_type msg_id,
                        const actor* subject, const actor* object = nullptr,
                        bool subject_visible = true, bool object_visible = true);