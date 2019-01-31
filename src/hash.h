struct ht_item_S;
struct ht_index_S;
typedef struct ht_item_S ht_item_T;
typedef struct ht_index_S ht_index_T;


ht_index_T* ht_new();
void ht_destroy(ht_index_T* t);

void  ht_insert(ht_index_T* t, const char* k, const char* v);
char* ht_search(ht_index_T* t, const char* k);
void  ht_delete(ht_index_T* t, const char* k);
