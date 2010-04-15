/* 
 * FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
 * Copyright (C) 2010, Mathieu Parent <math.parent@gmail.com>
 *
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
 *
 * The Initial Developer of the Original Code is
 * Mathieu Parent <math.parent@gmail.com>
 * Portions created by the Initial Developer are Copyright (C)
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * 
 * Mathieu Parent <math.parent@gmail.com>
 *
 *
 * skinny_api.c -- Skinny Call Control Protocol (SCCP) Endpoint Module
 *
 */

#include <switch.h>
#include "mod_skinny.h"
#include "skinny_protocol.h"
#include "skinny_tables.h"

/*****************************************************************************/
/* skinny_api_list_* */
/*****************************************************************************/

static switch_status_t skinny_api_list_profiles(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_console_callback_match_t *my_matches = NULL;
    switch_status_t status = SWITCH_STATUS_FALSE;
    switch_hash_index_t *hi;
    void *val;
    skinny_profile_t *profile;

    /* walk profiles */
    switch_mutex_lock(globals.mutex);
    for (hi = switch_hash_first(NULL, globals.profile_hash); hi; hi = switch_hash_next(hi)) {
	    switch_hash_this(hi, NULL, NULL, &val);
	    profile = (skinny_profile_t *) val;

	    switch_console_push_match(&my_matches, profile->name);
    }
    switch_mutex_unlock(globals.mutex);

    if (my_matches) {
	    *matches = my_matches;
	    status = SWITCH_STATUS_SUCCESS;
    }

    return status;
}

struct match_helper {
    switch_console_callback_match_t *my_matches;
};

static int skinny_api_list_devices_callback(void *pArg, int argc, char **argv, char **columnNames)
{
    struct match_helper *h = (struct match_helper *) pArg;
    char *device_name = argv[0];

    switch_console_push_match(&h->my_matches, device_name);
    return 0;
}

static switch_status_t skinny_api_list_devices(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    struct match_helper h = { 0 };
    switch_status_t status = SWITCH_STATUS_FALSE;
    skinny_profile_t *profile = NULL;
    char *sql;

    char *myline;
    char *argv[1024] = { 0 };
    int argc = 0;

    if (!(myline = strdup(line))) {
	    status = SWITCH_STATUS_MEMERR;
	    return status;
    }
    if (!(argc = switch_separate_string(myline, ' ', argv, (sizeof(argv) / sizeof(argv[0])))) || argc != 5) {
	    return status;
    }

    if(!strcasecmp(argv[1], "profile")) {/* skinny profile <profile_name> ... */
	    profile = skinny_find_profile(argv[2]);
    } else if(!strcasecmp(argv[2], "profile")) {/* skinny status profile <profile_name> ... */
	    profile = skinny_find_profile(argv[3]);
    }

    if(profile) {
	    if ((sql = switch_mprintf("SELECT name FROM skinny_devices"))) {
		    skinny_execute_sql_callback(profile, profile->sql_mutex, sql, skinny_api_list_devices_callback, &h);
		    switch_safe_free(sql);
	    }
    }

    if (h.my_matches) {
	    *matches = h.my_matches;
	    status = SWITCH_STATUS_SUCCESS;
    }

    return status;
}

static switch_status_t skinny_api_list_reset_types(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    SKINNY_PUSH_DEVICE_RESET_TYPES
    return status;
}

static switch_status_t skinny_api_list_stimuli(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    SKINNY_PUSH_STIMULI
    return status;
}

static switch_status_t skinny_api_list_ring_types(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    SKINNY_PUSH_RING_TYPES
    return status;
}

static switch_status_t skinny_api_list_ring_modes(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    SKINNY_PUSH_RING_MODES
    return status;
}

static switch_status_t skinny_api_list_stimulus_instances(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    switch_console_callback_match_t *my_matches = NULL;

    switch_console_push_match(&my_matches, "<stimulus_instance>");
    switch_console_push_match(&my_matches, "0");

    if (my_matches) {
	    *matches = my_matches;
	    status = SWITCH_STATUS_SUCCESS;
    }
    return status;
}

static switch_status_t skinny_api_list_stimulus_modes(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    SKINNY_PUSH_LAMP_MODES
    return status;
}

static switch_status_t skinny_api_list_speaker_modes(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    SKINNY_PUSH_SPEAKER_MODES
    return status;
}

static switch_status_t skinny_api_list_call_states(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    SKINNY_PUSH_CALL_STATES
    return status;
}

static switch_status_t skinny_api_list_line_instances(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    switch_console_callback_match_t *my_matches = NULL;

    /* TODO */
    switch_console_push_match(&my_matches, "1");
    switch_console_push_match(&my_matches, "<line_instance>");

    if (my_matches) {
	    *matches = my_matches;
	    status = SWITCH_STATUS_SUCCESS;
    }
    return status;
}

static switch_status_t skinny_api_list_call_ids(const char *line, const char *cursor, switch_console_callback_match_t **matches)
{
    switch_status_t status = SWITCH_STATUS_FALSE;
    switch_console_callback_match_t *my_matches = NULL;

    /* TODO */
    switch_console_push_match(&my_matches, "1345");
    switch_console_push_match(&my_matches, "<call_id>");

    if (my_matches) {
	    *matches = my_matches;
	    status = SWITCH_STATUS_SUCCESS;
    }
    return status;
}

/*****************************************************************************/
/* skinny_api_cmd_* */
/*****************************************************************************/
static switch_status_t skinny_api_cmd_status_profile(const char *profile_name, switch_stream_handle_t *stream)
{
    skinny_profile_t *profile;
    if ((profile = skinny_find_profile(profile_name))) {
	    skinny_profile_dump(profile, stream);
    } else {
	    stream->write_function(stream, "Profile not found!\n");
    }

    return SWITCH_STATUS_SUCCESS;
}

static switch_status_t skinny_api_cmd_status_profile_device(const char *profile_name, const char *device_name, switch_stream_handle_t *stream)
{
    skinny_profile_t *profile;
    if ((profile = skinny_find_profile(profile_name))) {
	    dump_device(profile, device_name, stream);
    } else {
	    stream->write_function(stream, "Profile not found!\n");
    }

    return SWITCH_STATUS_SUCCESS;
}

static switch_status_t skinny_api_cmd_profile_device_send_ringer_message(const char *profile_name, const char *device_name, const char *ring_type, const char *ring_mode, switch_stream_handle_t *stream)
{
    skinny_profile_t *profile;

    if ((profile = skinny_find_profile(profile_name))) {
	    listener_t *listener = NULL;
	    skinny_profile_find_listener_by_device_name(profile, device_name, &listener);
	    if(listener) {
		    send_set_ringer(listener, skinny_str2ring_type(ring_type), skinny_str2ring_mode(ring_mode), 0, 0);
	    } else {
		    stream->write_function(stream, "Listener not found!\n");
	    }
    } else {
	    stream->write_function(stream, "Profile not found!\n");
    }

    return SWITCH_STATUS_SUCCESS;
}

static switch_status_t skinny_api_cmd_profile_device_send_lamp_message(const char *profile_name, const char *device_name, const char *stimulus, const char *instance, const char *lamp_mode, switch_stream_handle_t *stream)
{
    skinny_profile_t *profile;

    if ((profile = skinny_find_profile(profile_name))) {
	    listener_t *listener = NULL;
	    skinny_profile_find_listener_by_device_name(profile, device_name, &listener);
	    if(listener) {
		    send_set_lamp(listener, skinny_str2button(stimulus), atoi(instance), skinny_str2lamp_mode(lamp_mode));
	    } else {
		    stream->write_function(stream, "Listener not found!\n");
	    }
    } else {
	    stream->write_function(stream, "Profile not found!\n");
    }

    return SWITCH_STATUS_SUCCESS;
}

static switch_status_t skinny_api_cmd_profile_device_send_speaker_mode_message(const char *profile_name, const char *device_name, const char *speaker_mode, switch_stream_handle_t *stream)
{
    skinny_profile_t *profile;

    if ((profile = skinny_find_profile(profile_name))) {
	    listener_t *listener = NULL;
	    skinny_profile_find_listener_by_device_name(profile, device_name, &listener);
	    if(listener) {
		    send_set_speaker_mode(listener, skinny_str2speaker_mode(speaker_mode));
	    } else {
		    stream->write_function(stream, "Listener not found!\n");
	    }
    } else {
	    stream->write_function(stream, "Profile not found!\n");
    }

    return SWITCH_STATUS_SUCCESS;
}

static switch_status_t skinny_api_cmd_profile_device_send_call_state_message(const char *profile_name, const char *device_name, const char *call_state, const char *line_instance, const char *call_id, switch_stream_handle_t *stream)
{
    skinny_profile_t *profile;

    if ((profile = skinny_find_profile(profile_name))) {
	    listener_t *listener = NULL;
	    skinny_profile_find_listener_by_device_name(profile, device_name, &listener);
	    if(listener) {
		    send_call_state(listener, skinny_str2call_state(call_state), atoi(line_instance), atoi(call_id));
	    } else {
		    stream->write_function(stream, "Listener not found!\n");
	    }
    } else {
	    stream->write_function(stream, "Profile not found!\n");
    }

    return SWITCH_STATUS_SUCCESS;
}

static switch_status_t skinny_api_cmd_profile_device_send_reset_message(const char *profile_name, const char *device_name, const char *reset_type, switch_stream_handle_t *stream)
{
    skinny_profile_t *profile;

    if ((profile = skinny_find_profile(profile_name))) {
	    listener_t *listener = NULL;
	    skinny_profile_find_listener_by_device_name(profile, device_name, &listener);
	    if(listener) {
		    send_reset(listener, skinny_str2device_reset_type(reset_type));
	    } else {
		    stream->write_function(stream, "Listener not found!\n");
	    }
    } else {
	    stream->write_function(stream, "Profile not found!\n");
    }

    return SWITCH_STATUS_SUCCESS;
}

/*****************************************************************************/
/* API */
/*****************************************************************************/
SWITCH_STANDARD_API(skinny_function)
{
    char *argv[1024] = { 0 };
    int argc = 0;
    char *mycmd = NULL;
    switch_status_t status = SWITCH_STATUS_SUCCESS;
    const char *usage_string = "USAGE:\n"
	    "--------------------------------------------------------------------------------\n"
	    "skinny help\n"
	    "skinny status profile <profile_name>\n"
	    "skinny status profile <profile_name> device <device_name>\n"
	    "skinny profile <profile_name> device <device_name> send ResetMessage [DeviceReset|DeviceRestart]\n"
	    "skinny profile <profile_name> device <device_name> send SetRingerMessage <ring_type> <ring_mode>\n"
	    "skinny profile <profile_name> device <device_name> send SetLampMessage <stimulus> <instance> <lamp_mode>\n"
	    "skinny profile <profile_name> device <device_name> send SetSpeakerModeMessage <speaker_mode>\n"
	    "skinny profile <profile_name> device <device_name> send CallStateMessage <call_state> <line_instance> <call_id>\n"
	    "--------------------------------------------------------------------------------\n";
    if (session) {
	    return SWITCH_STATUS_FALSE;
    }

    if (zstr(cmd)) {
	    stream->write_function(stream, "%s", usage_string);
	    goto done;
    }

    if (!(mycmd = strdup(cmd))) {
	    status = SWITCH_STATUS_MEMERR;
	    goto done;
    }

    if (!(argc = switch_separate_string(mycmd, ' ', argv, (sizeof(argv) / sizeof(argv[0])))) || !argv[0]) {
	    stream->write_function(stream, "%s", usage_string);
	    goto done;
    }

    if (!strcasecmp(argv[0], "help")) {/* skinny help */
	    stream->write_function(stream, "%s", usage_string);
	    goto done;
    } else if (argc == 3 && !strcasecmp(argv[0], "status") && !strcasecmp(argv[1], "profile")) {
	    /* skinny status profile <profile_name> */
	    status = skinny_api_cmd_status_profile(argv[2], stream);
    } else if (argc == 5 && !strcasecmp(argv[0], "status") && !strcasecmp(argv[1], "profile") && !strcasecmp(argv[3], "device")) {
	    /* skinny status profile <profile_name> device <device_name> */
	    status = skinny_api_cmd_status_profile_device(argv[2], argv[4], stream);
    } else if (argc >= 6 && !strcasecmp(argv[0], "profile") && !strcasecmp(argv[2], "device") && !strcasecmp(argv[4], "send")) {
	    /* skinny profile <profile_name> device <device_name> send ... */
	    switch(skinny_str2message_type(argv[5])) {
		    case SET_RINGER_MESSAGE:
			    if(argc == 8) {
				    /* SetRingerMessage <ring_type> <ring_mode> */
				    status = skinny_api_cmd_profile_device_send_ringer_message(argv[1], argv[3], argv[6], argv[7], stream);
			    }
			    break;
		    case SET_LAMP_MESSAGE:
			    if (argc == 9) {
				    /* SetLampMessage <stimulus> <instance> <lamp_mode> */
				    status = skinny_api_cmd_profile_device_send_lamp_message(argv[1], argv[3], argv[6], argv[7], argv[8], stream);
			    }
			    break;
		    case SET_SPEAKER_MODE_MESSAGE:
			    if (argc == 7) {
				    /* SetSpeakerModeMessage <speaker_mode> */
				    status = skinny_api_cmd_profile_device_send_speaker_mode_message(argv[1], argv[3], argv[6], stream);
			    }
			    break;
		    case CALL_STATE_MESSAGE:
			    if (argc == 9) {
				    /* CallStateMessage <call_state> <line_instance> <call_id> */
				    status = skinny_api_cmd_profile_device_send_call_state_message(argv[1], argv[3], argv[6], argv[7], argv[8], stream);
			    }
			    break;
		    case RESET_MESSAGE:
			    if (argc == 7) {
				    /* ResetMessage <reset_type> */
				    status = skinny_api_cmd_profile_device_send_reset_message(argv[1], argv[3], argv[6], stream);
			    }
			    break;
		    default:
			    stream->write_function(stream, "Unhandled message %s\n", argv[5]);
	    }
    } else {
	    stream->write_function(stream, "Unknown Command [%s]\n", argv[0]);
    }

done:
    switch_safe_free(mycmd);
    return status;
}

switch_status_t skinny_api_register(switch_loadable_module_interface_t **module_interface)
{
    switch_api_interface_t *api_interface;

    SWITCH_ADD_API(api_interface, "skinny", "Skinny Controls", skinny_function, "<cmd> <args>");
    switch_console_set_complete("add skinny help");

    switch_console_set_complete("add skinny status profile ::skinny::list_profiles");
    switch_console_set_complete("add skinny status profile ::skinny::list_profiles device ::skinny::list_devices");

    switch_console_set_complete("add skinny profile ::skinny::list_profiles device ::skinny::list_devices send ResetMessage ::skinny::list_reset_types");
    switch_console_set_complete("add skinny profile ::skinny::list_profiles device ::skinny::list_devices send SetRingerMessage ::skinny::list_ring_types ::skinny::list_ring_modes");
    switch_console_set_complete("add skinny profile ::skinny::list_profiles device ::skinny::list_devices send SetLampMessage ::skinny::list_stimuli ::skinny::list_stimulus_instances ::skinny::list_stimulus_modes");
    switch_console_set_complete("add skinny profile ::skinny::list_profiles device ::skinny::list_devices send SetSpeakerModeMessage ::skinny::list_speaker_modes");
    switch_console_set_complete("add skinny profile ::skinny::list_profiles device ::skinny::list_devices send CallStateMessage ::skinny::list_call_states ::skinny::list_line_instances ::skinny::list_call_ids");

    switch_console_add_complete_func("::skinny::list_profiles", skinny_api_list_profiles);
    switch_console_add_complete_func("::skinny::list_devices", skinny_api_list_devices);
    switch_console_add_complete_func("::skinny::list_reset_types", skinny_api_list_reset_types);
    switch_console_add_complete_func("::skinny::list_ring_types", skinny_api_list_ring_types);
    switch_console_add_complete_func("::skinny::list_ring_modes", skinny_api_list_ring_modes);
    switch_console_add_complete_func("::skinny::list_stimuli", skinny_api_list_stimuli);
    switch_console_add_complete_func("::skinny::list_stimulus_instances", skinny_api_list_stimulus_instances);
    switch_console_add_complete_func("::skinny::list_stimulus_modes", skinny_api_list_stimulus_modes);
    switch_console_add_complete_func("::skinny::list_speaker_modes", skinny_api_list_speaker_modes);
    switch_console_add_complete_func("::skinny::list_call_states", skinny_api_list_call_states);
    switch_console_add_complete_func("::skinny::list_line_instances", skinny_api_list_line_instances);
    switch_console_add_complete_func("::skinny::list_call_ids", skinny_api_list_call_ids);
    return SWITCH_STATUS_SUCCESS;
}

/* For Emacs:
 * Local Variables:
 * mode:c
 * indent-tabs-mode:t
 * tab-width:4
 * c-basic-offset:4
 * End:
 * For VIM:
 * vim:set softtabstop=4 shiftwidth=4 tabstop=4:
 */
