/**
 * @file localise.h
 * @brief String localisation (translation)
 **/

#pragma once

#include <string>

#include "format.h"

using std::string;

// A placeholder for strings that should not be localised.
// It's important that it ends with a full stop so that it's seen as a separate sentence.
const string NOLOC_PLACEHOLDER = "_NOLOC_PLACEHOLDER_.";

void init_localisation();

// not mandatory to call this, except in regression tests, where we want to switch languages
void shutdown_localisation();

void pause_localisation();

void unpause_localisation();

// Is localisation active?
bool localisation_active();

// localise string
string localise(const string &s);

// convenience function, equivalent to localise(make_stringf(format, ...))
string localisef(const char* format, ...);

// localise formatted string
formatted_string localise(const formatted_string& fs);

// localise with specific context
string localise_in_context(const string &context, const string &s);
