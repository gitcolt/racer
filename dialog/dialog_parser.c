#include "dialog_parser.h"
#include "../ansi_colors.h"

#include "../xml.h"

#include <stdlib.h>
#include <assert.h>

#define NODE_NAME_MATCH(node_str, name) \
    !strcmp(node_str, name)

char node_name[MAX_NODE_NAME_SZ] = {0};
char node_content[MAX_NODE_CONTENT_SZ] = {0};

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

static void parse_node_dialog_id(Dialog *dialog, XmlNode *node) {
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
            parse_node_dialog_id(dialog, n);
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

void load_conversation_from_xml(Conversation *convo, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Failed to open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t buf[size];
    fread(buf, *buf, size, f);
    fclose(f);

    struct xml_document *doc = xml_parse_document(buf, size);
    if (!doc) {
        printf("Could not parse xml\n");
        exit(EXIT_FAILURE);
    }
    dialogs_parse_xml(convo, doc);
    for (int i = 0; i < convo->num_dialogs; ++i) {
        dialog_validate(&convo->dialogs[i]);
        dialog_print(&convo->dialogs[i]);
    }
    xml_document_free(doc, false);
}

void unload_conversation(Conversation *convo) {
    memset(convo, 0, sizeof(*convo));
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

void parse_node_conversation_id(Conversation *convo, XmlNode *node) {
    get_node_content(convo->id, node);
}

void parse_node_conversation_dialogs(Conversation *convo, XmlNode *node) {
    size_t num_children = xml_node_children(node);
    for (int i = 0; i < num_children; ++i) {
        XmlNode *dialog_node = xml_node_child(node, i);
        Dialog dialog;
        dialog_init(&dialog);
        parse_node_dialog(&dialog, dialog_node);
        convo->dialogs[convo->num_dialogs++] = dialog;
    }
}

void dialogs_parse_xml(Conversation *convo, XmlDocument *doc) {
    XmlNode *root = xml_document_root(doc);
    int num_nodes = xml_node_children(root);
    for (int i = 0; i < num_nodes; ++i) {
        XmlNode *n = xml_node_child(root, i);
        get_node_name(node_name, n);
        assert(NODE_NAME_MATCH(node_name, NODE_CONVERSATION));

        size_t num_children = xml_node_children(n);
        for (int i = 0; i < num_children; ++i) {
            XmlNode *child = xml_node_child(n, i);
            get_node_name(node_name, child);
            if (NODE_NAME_MATCH(node_name, NODE_ID)) {
                parse_node_conversation_id(convo, child);
            } else if (NODE_NAME_MATCH(node_name, NODE_DIALOGS)) {
                parse_node_conversation_dialogs(convo, child);
            }
        }
    }
}
