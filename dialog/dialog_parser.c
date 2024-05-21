#include "dialog_parser.h"

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define MAX_TEXT_SZ 128

static void parse_node_hl(char parent_str[]) {
    // replace <hl> tag?
    char *hl_node_start = strstr(parent_str, "<" NODE_HL);
    // TODO
}

static void parse_node_var(Conversation *convo, char parent_str[]) {
    char *start = strstr(parent_str, "<" NODE_VAR);

    Var *var = &convo->vars[convo->num_vars];

    const char *attr = start + strlen("<" NODE_VAR);
    if (*attr == ' ') {
        static char attr_buf[99];
        static char attr_key_buf[99];
        static char attr_val_buf[99];
        int i = 0;
        char *end_attr = strchr(attr, '>');
        memcpy(attr_buf, attr + 1, end_attr - attr - 1);
        attr_buf[end_attr - attr - 1] = '\0';

        char *attr_key_end = strchr(attr_buf, '=');
        memcpy(attr_key_buf, attr_buf, attr_key_end - attr_buf);
        attr_key_buf[attr_key_end - attr_buf] = '\0';
        if (strcmp(attr_key_buf, "type")) {
            fprintf(stderr, "'type' is the only supported attribute for node "
                    "'" NODE_VAR "'\n");
            exit(EXIT_FAILURE);
        }

        char *attr_val_start = strchr(attr_buf, '"') + 1;
        char *attr_val_end = strchr(attr_val_start, '"');
        memcpy(attr_val_buf, attr_val_start, attr_val_end - attr_val_start);
        attr_val_buf[attr_val_end - attr_val_start] = '\0';

        if (!strcmp(attr_val_buf, "str")) {
            var->type = VAR_TYPE_STR;
        } else if (!strcmp(attr_val_buf, "num")) {
            var->type = VAR_TYPE_NUM;
        } else {
            fprintf(stderr, "Unknown var type '%s'\n", attr_val_buf);
            exit(EXIT_FAILURE);
        }
    } else {
        // fprintf(stderr, "'type' attribute is required for node 'var'\n");
        // exit(EXIT_FAILURE);
    }

    const char *var_id_start = strchr(start, '>') + 1;
    const char *var_id_end = strstr(var_id_start, "</" NODE_VAR ">");
    size_t var_id_len = var_id_end - var_id_start;

    memcpy(&convo->vars[convo->num_vars].id, var_id_start, var_id_len);
    convo->vars[convo->num_vars].id[var_id_len] = '\0';

    const char *end = strstr(parent_str, "</" NODE_VAR ">") + strlen("</" NODE_VAR ">");
    size_t nbytes = strlen(end) + 1;
    const char *str_fmt_specifier = "%s";
    strcpy(start, str_fmt_specifier);
    memmove(start + strlen(str_fmt_specifier), end, nbytes + 1);

    ++convo->num_vars;
}

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
    dialog_init(dialog);
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

#define PARENT_NODE_MATCH(node_name) \
    ( \
        parent_tok = toks + (i - 2), \
        !memcmp(buf + parent_tok->startpos, node_name ">", strlen(node_name) + 1) || \
        !memcmp(buf + parent_tok->startpos, node_name " ", strlen(node_name) + 1) \
    )

static void parse(Conversation *convo, const char *buf, sxmltok_t *toks, size_t num_tokens) {
    for (int i = 0; i < num_tokens; ++i) {
        const sxmltok_t *tok = toks + i; // ..
        if (tok->type == SXML_STARTTAG) {

#define NODE_MATCH_START(node_name) \
     !memcmp(buf + tok->startpos, node_name ">", strlen(node_name) + 1) || \
     !memcmp(buf + tok->startpos, node_name " ", strlen(node_name) + 1)

#define SET_START_AND_LEN(node_name) \
            do { \
                content_start = buf + tok->endpos + 1; \
                content_end = strstr(content_start, "</" node_name ); \
                content_len = content_end - content_start; \
            } while (0)

            const char *content_start;
            const char *content_end;
            size_t content_len;
            sxmltok_t *parent_tok;

            if (NODE_MATCH_START(NODE_CONVERSATION)) {
                // printf("content_len (convo): %zu\n", content_len);
            } else if (NODE_MATCH_START(NODE_CONVERSATION_ID)) {
                SET_START_AND_LEN(NODE_CONVERSATION_ID);
                parse_node_conversation_id(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_DIALOG)) {
                SET_START_AND_LEN(NODE_DIALOG);
                parse_node_dialog(convo);
            } else if (NODE_MATCH_START(NODE_DIALOG_ID)) {
                SET_START_AND_LEN(NODE_DIALOG_ID);
                parse_node_dialog_id(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_DIALOG_TEXT)) {
                SET_START_AND_LEN(NODE_DIALOG_TEXT);
                parse_node_dialog_text(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_CHOICE_TEXT)) {
                SET_START_AND_LEN(NODE_CHOICE_TEXT);
                parse_node_choice_text(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_CHOICE)) {
                SET_START_AND_LEN(NODE_CHOICE);
                parse_node_choice(convo);
            } else if (NODE_MATCH_START(NODE_DIALOG_NEXT)) {
                SET_START_AND_LEN(NODE_DIALOG_NEXT);
                parse_node_dialog_next(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_CHOICE_NEXT)) {
                // 
            } else if (NODE_MATCH_START(NODE_DIALOG_ACTION)) {
                SET_START_AND_LEN(NODE_DIALOG_ACTION);
                parse_node_dialog_action(convo, content_start, content_len);
            } else if (NODE_MATCH_START(NODE_VAR)) {
                Dialog *dialog = &convo->dialogs[convo->num_dialogs];
                if (PARENT_NODE_MATCH(NODE_DIALOG_TEXT)) {
                    parse_node_var(convo, dialog->text);
                } else if (PARENT_NODE_MATCH(NODE_CHOICE_TEXT)) {
                    parse_node_var(convo, dialog->choices[dialog->num_choices].text);
                } else
                    printf("??\n");
            } else if (NODE_MATCH_START(NODE_HL)) {
                Dialog *dialog = &convo->dialogs[convo->num_dialogs];
                if (PARENT_NODE_MATCH(NODE_DIALOG_TEXT)) {
                    parse_node_hl(dialog->text);
                } else if (PARENT_NODE_MATCH(NODE_CHOICE_TEXT)) {
                    parse_node_hl(dialog->choices[dialog->num_choices].text);
                } else
                    printf("??\n");
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
    printf("\t\t\ttext: %s\n", choice->text);
    printf("\t\t\tnext: \"%s\"\n", choice->next);
}

void dialog_print(Dialog *dialog) {
    printf("\tDIALOG:\n");
    printf("\t\t%7s: %s\n", "id", dialog->id);
    printf("\t\t%7s: \"%s\"\n", "text", dialog->text);
    printf("\t\t%7s: %s\n", "next", strlen(dialog->next) ? dialog->next : "NULL");
    printf("\t\t%7s: %s\n", "action", strlen(dialog->action) ? dialog->action : "NULL");
    if (dialog->num_choices) {
        printf("\t\tCHOICES:\n");
        for (int i = 0; i < dialog->num_choices; ++i) {
            choice_print(&dialog->choices[i]);
        }
    }
}

void conversation_print(Conversation *convo) {
    // var_set_int(&convo->vars[0], 123);
    printf("CONVERSATION:\n");
    printf("id: %s\n", convo->id);
    for (int i = 0; i < convo->num_dialogs; ++i) {
        Dialog *dialog = &convo->dialogs[i];
        dialog_print(dialog);
    }
    printf("VARS:\n");
    for (int i = 0; i < convo->num_vars; ++i) {
        printf("id: %s\n", convo->vars[i].id);
        // printf("type: %s\n",
        //         convo->vars[i].type == VAR_TYPE_STR ? "STR"
        //         : convo->vars[i].type == VAR_TYPE_NUM ? "NUM"
        //         : "???");
        printf("val: \"%s\"\n", convo->vars[i].val_str);
    }
}
