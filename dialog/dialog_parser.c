#include "dialog_parser.h"

#include <stdlib.h>
#include <assert.h>

#define MAX_NODE_NAME_SZ 99
#define MAX_NODE_CONTENT_SZ 999
#define MAX_CHOICES_SZ 99

// Node names
#define NODE_DIALOG "dialog"
#define NODE_ID "id"
#define NODE_TEXT "text"
#define NODE_CHOICES "choices"
#define NODE_CHOICE "choice"
#define NODE_NEXT "next"
#define NODE_ACTION "action"

#define NODE_NAME_MATCH(node_str, name) \
    !strcmp(node_str, name)


typedef struct xml_node XmlNode;
typedef struct xml_string XmlString;
typedef struct xml_document XmlDocument;

typedef struct D D;
typedef struct {
    char text[MAX_NODE_CONTENT_SZ];
    char next[MAX_NODE_CONTENT_SZ];
} Choice;

typedef struct D {
    char id[MAX_NODE_CONTENT_SZ];
    char text[MAX_NODE_CONTENT_SZ];
    Choice choices[MAX_CHOICES_SZ];
    size_t choices_sz;
    char next[MAX_NODE_CONTENT_SZ];
    char action[MAX_NODE_CONTENT_SZ];
} D;
typedef struct {
    size_t num_dialogs;
    D *dialogs;
} Dialogs;

char node_name[MAX_NODE_NAME_SZ] = {0};
char node_content[MAX_NODE_CONTENT_SZ] = {0};

static void print_d(D *d) {
    printf("D:\n");
    printf("\t%6s: %s\n", "id", *d->id ? d->id : "NULL");
    printf("\t%6s: %s\n", "text", *d->text ? d->text : "NULL");
    printf("\t%6s: %s\n", "next", *d->next ? d->next : "NULL");
    printf("\t%6s: %s\n", "action", *d->action ? d->action : "NULL");
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

static void parse_node_choice_text(D *d, XmlNode *node) {
    get_node_content(node_content, node);
    printf("node_content: %s\n", node_content);
    strcpy(d->choices[d->choices_sz].text, node_content);
}

static void parse_node_choice_next(D *d, XmlNode *node) {
    get_node_content(node_content, node);
    strcpy(d->choices[d->choices_sz].next, node_content);
}

static void parse_node_choice(D *d, XmlNode *node) {
    size_t num_children = xml_node_children(node);
    for (int i = 0; i < num_children; ++i) {
        XmlNode *child = xml_node_child(node, i);
        get_node_name(node_name, child);
        if (NODE_NAME_MATCH(node_name, NODE_TEXT))
            parse_node_choice_text(d, child);
        else if (NODE_NAME_MATCH(node_name, NODE_NEXT))
            parse_node_choice_next(d, child);
    }
    ++d->choices_sz;
}

static void parse_node_choices(D *d, XmlNode *node) {
    size_t num_children = xml_node_children(node);
    for (int i = 0; i < num_children; ++i) {
        XmlNode *choice = xml_node_child(node, i);
        get_node_name(node_name, choice);
        assert(NODE_NAME_MATCH(node_name, NODE_CHOICE));
        parse_node_choice(d, choice);
    }
}

static void parse_node_id(D *d, XmlNode *node) {
    get_node_content(d->id, node);
}

static void parse_node_text(D *d, XmlNode *node) {
    get_node_content(d->text, node);
}

static void parse_node_next(D *d, XmlNode *node) {
    get_node_content(d->next, node);
}

static void parse_node_action(D *d, XmlNode *node) {
    get_node_content(d->action, node);
}

static void parse_node_dialog(D *d, XmlNode *node) {

    size_t num_children = xml_node_children(node);
    for (int i = 0; i < num_children; ++i) {
        XmlNode *n = xml_node_child(node, i);
        get_node_name(node_name, n);
        if (NODE_NAME_MATCH(node_name, NODE_ID))
            parse_node_id(d, n);
        else if (NODE_NAME_MATCH(node_name, NODE_TEXT))
            parse_node_text(d, n);
        else if (NODE_NAME_MATCH(node_name, NODE_NEXT))
            parse_node_next(d, n);
        else if (NODE_NAME_MATCH(node_name, NODE_CHOICES))
            parse_node_choices(d, n);
        else if (NODE_NAME_MATCH(node_name, NODE_ACTION))
            parse_node_action(d, n);
        else {
            fprintf(stderr, "Bad XML node '%s'\n", node_name);
            exit(EXIT_FAILURE);
        }
    }
}

void parse_dialog_xml(XmlDocument *doc) {
    XmlNode *root = xml_document_root(doc);
    int num_nodes = xml_node_children(root);
    for (int i = 0; i < num_nodes; ++i) {
        XmlNode *n = xml_node_child(root, i);
        get_node_name(node_name, n);
        assert(!strcmp(node_name, NODE_DIALOG));
        D d = {0};
        parse_node_dialog(&d, n);
        print_d(&d);
    }
}
