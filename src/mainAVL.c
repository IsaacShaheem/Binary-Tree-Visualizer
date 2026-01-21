#include "avl.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    FILE* file = fopen("visualizer/avl_snapshots.json", "w");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    AVLNode* root = NULL;

    // Start JSON array
    fprintf(file, "[\n");

    // Insert ALL values passed from Python
    for (int i = 1; i < argc; i++) {
        int value = atoi(argv[i]);

        root = insert_AVLNode_AVL(root, value);

        export_AVL_JSON(root, file);
        fprintf(file, ",\n");
    }

    // End JSON array
    fprintf(file, "{}\n]");
    fclose(file);

    return 0;
}
