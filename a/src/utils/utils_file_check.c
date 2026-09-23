#include "utils.h"
#include "config.h"
#include "taskmaster.h"

#include <signal.h>

int check_file_extension(const char *file, const char *file_ext)
{
    char *dot = strrchr(file, '.');

    if (!dot || dot == file || *(dot + 1) == '\0')
    {
        fprintf(stderr, "Error: '%s' has a wrong file extension\n", file);
        return (EXIT_FAILURE);
    }

    if (strcmp(dot + 1, file_ext) == 0)
    {
        return (EXIT_SUCCESS);
    }
    
    fprintf(stderr, "Error: can't find file extension\n");
    return (EXIT_FAILURE);
}

void print_yaml_node(yaml_document_t *document, yaml_node_t *node, int indent)
{
    if (!node)
        return ;
    for (int i = 0; i < indent; i++)
        printf(" ");

    switch (node->type)
    {
    case YAML_NO_NODE:
        printf("<Empty Node>\n");
        break;

    case YAML_SCALAR_NODE:
        printf("%s\n", (char *)node->data.scalar.value);
        break;

    case YAML_SEQUENCE_NODE:
        printf("[\n");
        for (yaml_node_item_t *item = node->data.sequence.items.start; item < node->data.sequence.items.top; item++)
        {
            yaml_node_t *child = yaml_document_get_node(document, *item);
            print_yaml_node(document, child, indent + 1);
        }

    for(int i = 0; i < indent; i++) printf(" ");
    printf("]\n");
    break;
    case YAML_MAPPING_NODE:
            printf("{\n");
            for (yaml_node_pair_t *pair = node->data.mapping.pairs.start;
                 pair < node->data.mapping.pairs.top; pair++) {

                yaml_node_t *key = yaml_document_get_node(document, pair->key);
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("Key: ");
                print_yaml_node(document, key, 0);

                yaml_node_t *value = yaml_document_get_node(document, pair->value);
                print_yaml_node(document, value, indent + 1);
            }
            for (int i = 0; i < indent; i++) printf("  ");
            printf("}\n");
            break;

    default:
    printf("Unknown Node type\n");
    break;
    }

}

int check_file_empty_v2(char *file)
{
    struct stat f;

    if (stat(file, &f) == 0)
    {
        if (f.st_size == 0)
        {
            fprintf(stderr, "Error: File is empty, his size %ld\n", f.st_size);
            return (EXIT_FAILURE);
        }
        return (EXIT_SUCCESS);
    }

    return (-1);
}
