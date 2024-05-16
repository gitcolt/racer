#include "dialog_parser.h"
#include "../ansi_colors.h"
#include "../shared.h"

#include "../xml.h"

#include <stdlib.h>
#include <assert.h>

#define NODE_NAME_MATCH(node_str, name) \
    !strcmp(node_str, name)

char node_name[MAX_NODE_NAME_SZ] = {0};
char node_content[MAX_NODE_CONTENT_SZ] = {0};

static void dialog_init(Dialog *dialog) {
    ZERO_MEM(dialog);
    dialog->font = GetFontDefault();
}

static void get_node_name(char *dst, XmlNode *node) {
    XmlString *xml_str = xml_node_name(node);
    memset(dst, 0, MAX_NODE_NAME_SZ);
    xml_string_copy(xml_str, (uint8_t *)dst, xml_string_length(xml_str));
}

static void get_node_content(char *dst, XmlNode *node) {
    XmlString *xml_str = xml_node_content(node);
    memset(dst, 0, MAX_NODE_NAME_SZ);
    xml_string_copy(xml_str, (uint8_t *)dst, xml_string_length(xml_str));
}

static void parse_node_choice_text(Dialog *dialog, XmlNode *node) {
    get_node_content(node_content, node);
    printf("node_content: %s\n", node_content);
    strcpy(dialog->choices[dialog->choices_sz].text, node_content);
}

static void parse_node_choice_next(Dialog *dialog, XmlNode *node) {
    get_node_content(node_content, node);
    strcpy(dialog->choices[dialog->choices_sz].next, node_content);
}

static void parse_node_choice(Dialog *dialog, XmlNode *node) {
    size_t num_children = xml_node_children(node);
    for (int i = 0; i < num_children; ++i) {
        XmlNode *child = xml_node_child(node, i);
        get_node_name(node_name, child);
        if (NODE_NAME_MATCH(node_name, NODE_TEXT))
            parse_node_choice_text(dialog, child);
        else if (NODE_NAME_MATCH(node_name, NODE_NEXT))
            parse_node_choice_next(dialog, child);
    }
    ++dialog->choices_sz;
}

static void parse_node_choices(Dialog *dialog, XmlNode *node) {
    size_t num_children = xml_node_children(node);
    for (int i = 0; i < num_children; ++i) {
        XmlNode *choice = xml_node_child(node, i);
        get_node_name(node_name, choice);
        assert(NODE_NAME_MATCH(node_name, NODE_CHOICE));
        parse_node_choice(dialog, choice);
    }
}

static void parse_node_id(Dialog *dialog, XmlNode *node) {
    get_node_content(dialog->id, node);
}

static void parse_node_text(Dialog *dialog, XmlNode *node) {
    get_node_content(dialog->text, node);
}

static void parse_node_next(Dialog *dialog, XmlNode *node) {
    get_node_content(dialog->next, node);
}

static void parse_node_action(Dialog *dialog, XmlNode *node) {
    get_node_content(dialog->action, node);
}

static void parse_node_dialog(Dialog *dialog, XmlNode *node) {

    size_t num_children = xml_node_children(node);
    for (int i = 0; i < num_children; ++i) {
        XmlNode *n = xml_node_child(node, i);
        get_node_name(node_name, n);
        if (NODE_NAME_MATCH(node_name, NODE_ID))
            parse_node_id(dialog, n);
        else if (NODE_NAME_MATCH(node_name, NODE_TEXT))
            parse_node_text(dialog, n);
        else if (NODE_NAME_MATCH(node_name, NODE_NEXT))
            parse_node_next(dialog, n);
        else if (NODE_NAME_MATCH(node_name, NODE_CHOICES))
            parse_node_choices(dialog, n);
        else if (NODE_NAME_MATCH(node_name, NODE_ACTION))
            parse_node_action(dialog, n);
        else {
            fprintf(stderr, "Bad XML node '%s'\n", node_name);
            exit(EXIT_FAILURE);
        }
    }
}

void dialog_print(Dialog *dialog) {
    printf("Dialog:\n");
    printf("\t%7s: %s\n", "id", *dialog->id ? dialog->id : "NULL");
    printf("\t%7s: %s\n", "text", *dialog->text ? dialog->text : "NULL");
    printf("\t%7s: %s\n", "next", *dialog->next ? dialog->next : "NULL");
    printf("\t%7s: %s\n", "action", *dialog->action ? dialog->action : "NULL");
    printf("\t%7s: ", "choices");
    if (!dialog->choices_sz)
        printf("None\n");
    else {
        printf("\n");
        for (int i = 0; i < dialog->choices_sz; ++i) {
            printf("\t\ttext: %s\n", dialog->choices[i].text);
            printf("\t\tnext: %s\n", dialog->choices[i].next);
        }
    }
}

void dialog_validate(Dialog *dialog) {
    printf(YEL_TXT "[dialog_validate]: UNIMPLEMENTED\n" RST_COL);
    if (!(strlen(dialog->text) > 0)) {
            printf(GRN_TXT "INVALID DIALOG: invalid text node\n" RST_COL);
            return;
    }
    // should have either a <choices> node or a <next> node exclusively
    if (dialog->choices_sz) { // validate <choices> node
        if (*dialog->next) {
            printf(GRN_TXT "INVALID DIALOG: Node should have either a <choices> node"
                    "or a <next> node, not both\n" RST_COL);
            return ;
        }
        for (int i = 0; i < dialog->choices_sz; ++i) {
            if (!(strlen(dialog->choices[i].text) > 0)) {
                printf(GRN_TXT "INVALID DIALOG: invalid <choice> node (%d)\n" RST_COL, i);
                return;
            }
        }
    } else { // validate <next> node
        if (!(strlen(dialog->next) > 0)) {
            printf(GRN_TXT "INVALID DIALOG: invalid <next> node\n" RST_COL);
            return;
        }
    }
    if (strlen(dialog->action) > 0) { // validate <action> node
        // TODO
    }
}

void dialogs_parse_xml(Conversation *convo, XmlDocument *doc) {
    XmlNode *root = xml_document_root(doc);
    int num_nodes = xml_node_children(root);
    for (int i = 0; i < num_nodes; ++i) {
        XmlNode *n = xml_node_child(root, i);
        get_node_name(node_name, n);
        assert(NODE_NAME_MATCH(node_name, NODE_DIALOG));
        Dialog dialog;
        dialog_init(&dialog);
        parse_node_dialog(&dialog, n);
        convo->dialogs[convo->num_dialogs++] = dialog;
    }
}
