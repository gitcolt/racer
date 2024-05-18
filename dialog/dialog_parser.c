#include "dialog_parser.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define MAX_TEXT_SZ 128

static void parse_node_dialog_text(Conversation *convo,
                                   const char *content_start,
                                   size_t content_len) {
    char dialog_text_str[MAX_TEXT_SZ];
    memcpy(dialog_text_str, content_start, content_len);
    dialog_text_str[content_len] = '\0';
    strcpy(convo->dialogs[convo->num_dialogs].text, dialog_text_str);
}

static void parse_node_dialog_next(Conversation *convo,
                                   const char *content_start,
                                   size_t content_len) {
    char dialog_next_str[MAX_TEXT_SZ];
    memcpy(dialog_next_str, content_start, content_len);
    dialog_next_str[content_len] = '\0';
    strcpy(convo->dialogs[convo->num_dialogs].next, dialog_next_str);
}

static void parse_node_dialog_action(Conversation *convo,
                                     const char *content_start,
                                     size_t content_len) {
    char dialog_action_str[MAX_TEXT_SZ];
    memcpy(dialog_action_str, content_start, content_len);
    dialog_action_str[content_len] = '\0';
    strcpy(convo->dialogs[convo->num_dialogs].action, dialog_action_str);
}

static void parse_node_dialog(Conversation *convo) {
    Dialog *dialog = &convo->dialogs[convo->num_dialogs];
    memset(dialog, 0, sizeof(*dialog));
    memset(dialog->text, 0, sizeof(*dialog->text));
    memset(dialog->next, 0, sizeof(*dialog->next));
}

static void parse_node_choice(Conversation *convo) {
    Dialog *curr_dialog = &convo->dialogs[convo->num_dialogs];
    Choice *choice = &curr_dialog->choices[curr_dialog->num_choices];
    memset(choice, 0, sizeof(*choice));
}

static void parse_node_choice_text(Conversation *convo,
                                   const char *content_start,
                                   size_t content_len) {
    char choice_text_str[MAX_TEXT_SZ];
    memcpy(choice_text_str, content_start, content_len);
    choice_text_str[content_len] = '\0';
    Dialog *dialog = &convo->dialogs[convo->num_dialogs];
    Choice *choice = &dialog->choices[dialog->num_choices];
    printf("choice_text_str: %s\n", choice_text_str);
    strcpy(choice->text, choice_text_str);
}

static void parse_node_choice_next(Conversation *convo,
                                   const char *content_start,
                                   size_t content_len) {
    char choice_next_str[MAX_TEXT_SZ];
    memcpy(choice_next_str, content_start, content_len);
    choice_next_str[content_len] = '\0';
    Dialog *dialog = &convo->dialogs[convo->num_dialogs];
    Choice *choice = &dialog->choices[dialog->num_choices];
    printf("choice_next_str: %s\n", choice_next_str);
    strcpy(choice->next, choice_next_str);
}

static void parse_node_dialog_id(Conversation *convo,
                                 const char *content_start,
                                 size_t content_len) {
    char dialog_id_str[MAX_TEXT_SZ];
    memcpy(dialog_id_str, content_start, content_len);
    dialog_id_str[content_len] = '\0';
    strcpy(convo->dialogs[convo->num_dialogs].id, dialog_id_str);
}

static void parse_node_conversation_id(Conversation *convo,
                                       const char *content_start,
                                       size_t content_len) {
    char conversation_id_str[MAX_TEXT_SZ];
    memcpy(conversation_id_str, content_start, content_len);
    conversation_id_str[content_len] = '\0';
    strcpy(convo->id, conversation_id_str);
}

static void parse(Conversation *convo, const char *buf, sxmltok_t *toks, size_t num_tokens) {
    for (int i = 0; i < num_tokens; ++i) {
        const sxmltok_t *tok = toks + i; // ..
        if (tok->type == SXML_STARTTAG) {

#define NODE_MATCH_START(node_name) \
     !memcmp(buf + tok->startpos, node_name ">", strlen(node_name) + 1) \
        ? ( \
                content_start = buf + tok->endpos + 1, \
                content_end = strstr(content_start, "</" node_name ), \
                content_len = content_end - content_start, \
                true \
                ) \
        : false

            const char *content_start;
            const char *content_end;
            size_t content_len;

            if (NODE_MATCH_START(NODE_CONVERSATION)) {
                // printf("content_len (convo): %zu\n", content_len);
            } else if (NODE_MATCH_START(NODE_CONVERSATION_ID)) {
                parse_node_conversation_id(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_DIALOG)) {
                parse_node_dialog(convo);
            } else if (NODE_MATCH_START(NODE_DIALOG_ID)) {
                parse_node_dialog_id(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_DIALOG_TEXT)) {
                parse_node_dialog_text(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_CHOICE_TEXT)) {
                parse_node_choice_text(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_CHOICE)) {
                parse_node_choice(convo);
            } else if (NODE_MATCH_START(NODE_DIALOG_NEXT)) {
                parse_node_dialog_next(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_CHOICE_NEXT)) {
                // 
            } else if (NODE_MATCH_START(NODE_DIALOG_ACTION)) {
                parse_node_dialog_action(convo, content_start, content_len);
            }
        } else if (tok->type == SXML_ENDTAG) {

#define NODE_MATCH_END(node_name) \
     !memcmp(buf + tok->startpos - 1, "/" node_name ">", strlen(node_name) + 2)

            if (NODE_MATCH_END(NODE_DIALOG))
                ++convo->num_dialogs;
            else if (NODE_MATCH_END(NODE_CHOICE))
                ++convo->dialogs[convo->num_dialogs].num_choices;
        } else {
            // ???
        }
    }
}

void load_conversation_from_xml(Conversation *convo, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Failed to open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char buf[size];
    memset(buf, 0, size);
    fread(buf, 1, size, f);
    fclose(f);

#define NUM_TOKS 128
    sxmltok_t toks[NUM_TOKS];
    sxml_t parser;
    sxml_init(&parser);

    sxmlerr_t err;
    while ((err = sxml_parse(&parser, &buf[0], size, toks, NUM_TOKS)) == SXML_SUCCESS) {
        // switch (err) {
        //     case SXML_SUCCESS:
        //         break;
        //     case SXML_ERROR_TOKENSFULL:
        //         fprintf(stderr, "SXML_ERROR_TOKENSFULL\n");
        //         break;
        //     case SXML_ERROR_BUFFERDRY:
        //         fprintf(stderr, "SXML_ERROR_BUFFERDRY\n");
        //         goto done_parsing;
        //     case SXML_ERROR_XMLINVALID:
        //         fprintf(stderr, "SXML_ERROR_XMLINVALID\n");
        //         break;
        //     default:
        //         fprintf(stderr, "Unknown error code '%d'\n", err);
        // }
        parse(convo, buf, toks, parser.ntokens);
    }
}

void unload_conversation(Conversation *convo) {
    memset(convo, 0, sizeof(*convo));
}

void dialog_validate(Dialog *dialog) {
    printf("dialog_validate()\n");
}

void choice_print(Choice *choice) {
    printf("\t\ttext: %s\n", choice->text);
    printf("\t\tnext: %s\n", choice->next);
}

void dialog_print(Dialog *dialog) {
    printf("DIALOG:\n");
    printf("\t%7s: %s\n", "id", dialog->id);
    printf("\t%7s: %s\n", "text", dialog->text);
    printf("\t%7s: %s\n", "next", strlen(dialog->next) ? dialog->next : "NULL");
    printf("\t%7s: %s\n", "action", strlen(dialog->action) ? dialog->action : "NULL");
    if (dialog->num_choices) {
        printf("\tCHOICES:\n");
        for (int i = 0; i < dialog->num_choices; ++i) {
            choice_print(&dialog->choices[i]);
        }
    }
}

void conversation_print(Conversation *convo) {
    printf("convo id: '%s'\n", convo->id);
    for (int i = 0; i < convo->num_dialogs; ++i) {
        Dialog *dialog = &convo->dialogs[i];
        dialog_print(dialog);
    }
}
