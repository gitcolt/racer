#ifndef DIALOG_PARSER_H
#define DIALOG_PARSER_H

#include "dialog.h"
#include "../shared.h"

// Node names
#define NODE_CONVERSATION "conversation"
#define NODE_DIALOGS "dialogs"
#define NODE_DIALOG "dialog"
#define NODE_ID "id"
#define NODE_TEXT "text"
#define NODE_CHOICES "choices"
#define NODE_CHOICE "choice"
#define NODE_NEXT "next"
#define NODE_ACTION "action"

void load_conversation_from_xml(Conversation *convo, const char *filename);
void unload_conversation(Conversation *convo);
void dialogs_parse_xml(Conversation *dialogs, XmlDocument *doc);
void dialog_validate(Dialog *dialog);
void dialog_print(Dialog *dialog);

#endif
