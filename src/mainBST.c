#include "bst.h"

int main(void)
{

    BSTNode* root = NULL;
    
    int values[] = {1,5,8,2,6};

    FILE* f = fopen("bst_snapshots.json", "w");
    if(!f){
        perror("File unable to be created");
        return 1;
    }

    fprintf(f,"[\n");

    for(int i=0;i<5;i++){
        root = insert_BSTNode_BST(root,values[i]);
        exportBSTJSON(root,f);
        fprintf(f,",\n");
    }

    fprintf(f, "{}\n]");
    fclose(f);

    return 0;
}
