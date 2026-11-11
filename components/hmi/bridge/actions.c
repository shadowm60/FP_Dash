/*
 * EEZ Studio native actions (declared in the generated src/ui/actions.h).
 * Every action declared in the EEZ project needs a function here, or the
 * link fails. Keep them thin.
 */
#include "actions.h"
#include "views.h"

/* Any widget event, e.g. a "Settings" button on a dash page. */
void action_open_settings(lv_event_t *e)
{
    nav_open_settings();
}

/* Settings screen > Back */
void action_close_settings(lv_event_t *e)
{
    nav_close_settings();
}
