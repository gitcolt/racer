#ifndef DIALOG_PARSER
#define DIALOG_PARSER

#include "../sxml.h"
#include "dialog.h"

// Node names
#define NODE_CONVERSATION "conversation"
#define NODE_CONVERSATION_ID "conversation_id"
#define NODE_DIALOGS "dialogs"
#define NODE_DIALOG "dialog"
#define NODE_DIALOG_ID "dialog_id"
#define NODE_CHOICE_ID "choice_id"
#define NODE_DIALOG_TEXT "dialog_text"
#define NODE_CHOICE_TEXT "choice_text"
#define NODE_CHOICES "choices"
#define NODE_CHOICE "choice"
#define NODE_DIALOG_NEXT "dialog_next"
#define NODE_CHOICE_NEXT "choice_next"
#define NODE_DIALOG_ACTION "dialog_action"
#define NODE_CHOICE_ACTION "choice_action"

void load_conversation_from_xml(Conversation *convo, const char *filename);
void unload_conversation(Conversation *convo);
void dialogs_parse_xml(Conversation *dialogs,
                       const char *buf,
                       sxmltok_t *toks,
                       size_t num_toks);
void dialog_validate(Dialog *dialog);
void conversation_print(Conversation *convo);
void dialog_print(Dialog *dialog);

#endif
