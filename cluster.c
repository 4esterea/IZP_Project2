/**
 * Zhdanovich Iaroslav
 * login: xzhdan00
 * 03.12.2022
 * 
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t -  objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    c->obj = malloc(cap * sizeof(struct obj_t));
    c->capacity = cap;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    init_cluster(c, 0);
    c->size = 0;

}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    resize_cluster(c, (c->capacity)+1); 
    c->obj[c->capacity-1] = obj;
    c->size += 1;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    for (int i = 0; i < c2->capacity; i++){
        append_cluster(c1, c2->obj[i]);
    }
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/

int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    for (int j = 0; j < narr-idx-1; j++){ 
        clear_cluster(carr+j); 
        for (int i = 0; i < (carr+j+1)->capacity; i++){
            append_cluster(carr+j, (carr+j+1)->obj[i]); 
        }
    }
    free((carr+narr-idx-1)->obj);
    return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    float vzdalenost = sqrtf(((o1->x)-(o2->x))*((o1->x)-(o2->x))+((o1->y)-(o2->y))*((o1->y)-(o2->y)));
    return vzdalenost;
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);
    float mindistance = 99999;
    for (int i = 0; i < c1->capacity; i++){
        for (int j = 0; j < c2->capacity; j++){
            if (obj_distance(&c1->obj[i], &c2->obj[j]) < mindistance){
                mindistance = obj_distance(&c1->obj[i], &c2->obj[j]);
            }
        }
    }
    return mindistance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    float mindistance = 99999;
    for (int i = 0; i < narr - 1; i++){
        for (int j = i + 1; j < narr; j++){
            if (cluster_distance(&carr[i], &carr[j]) < mindistance){
                mindistance = cluster_distance(&carr[i], &carr[j]);
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/

int load_clusters(char *filename, struct cluster_t **arr)
{
	assert(arr != NULL);
	strcat(filename, ".txt");
    FILE *soubor = fopen(filename, "r");
    if (!soubor){
        fprintf(stderr, "Error: Wrong file name\n");
        abort();
    }
	int count;
	if (!(fscanf(soubor, "count=%d", &count))){
        fprintf(stderr, "Error: Wrong file format\n");
        abort();
    }
    *arr = malloc(count * sizeof(struct cluster_t));  
    for (int i = 0; i < count; i++){
		init_cluster((*arr+i), 1);
        if (fscanf(soubor, "%d %f %f", &(*arr+i)->obj->id, &(*arr+i)->obj->x, &(*arr+i)->obj->y) != 3){
            fprintf(stderr, "Error: Wrong file format\n");
            abort();
        }
        (*arr+i)->size = 1;
		(*arr+i)->capacity = 1;
    }
	fclose(soubor);
	return count;
}
/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    int finalclustercount;
    if (argc == 2){
        finalclustercount = 1;
    } else if (argc == 3){
        finalclustercount = atoi(argv[2]);
        
    } else {
        fprintf(stderr, "Error: Unexpected number of arguments\n");
        abort();
    }
	char *filename = argv[1];
	struct cluster_t *clusters;
	int currentclustercount = load_clusters(filename, &clusters);
    if ((finalclustercount > currentclustercount) || (finalclustercount < 1)){
        fprintf(stderr, "Error: Unexpected number of clusters was inserted\n");
        for (int i = 0; i < currentclustercount; i++){
            free((clusters+i)->obj);
        }
        free(clusters);
        abort();
        }
    int cykluscount = currentclustercount - finalclustercount;
    for (int i = 0; i < cykluscount; i++){
        int neighbour1idx, neighbour2idx;
        find_neighbours(clusters, currentclustercount, &neighbour1idx, &neighbour2idx);
        merge_clusters(clusters+neighbour1idx, clusters+neighbour2idx);
        currentclustercount = remove_cluster(clusters+neighbour2idx, currentclustercount, neighbour2idx);
    }
    for (int i = 0; i < currentclustercount; i++){
        sort_cluster(clusters+i);
    }
    print_clusters(clusters, currentclustercount);
    for (int i = 0; i < currentclustercount; i++){
        free((clusters+i)->obj);
    }
    free(clusters);
    return 0;
}
