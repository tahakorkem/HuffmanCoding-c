#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

/**
 * Sıklık dizisi ve Huffman kod dizisinin toplam uzunluğu. Bir char uzunluğu olan 1 byte olarak belirlenmiştir.
 * */
#define N 256
/**
 * Girdi olarak verilebilecek maksimum metin uzunluğu.
 * */
#define MAX_TEXT 5000
/**
 * Oluşturulan Priority Queue'nun maksimum kapasitesi.
 * */
#define MAX_PRIORITY_QUEUE 1280
/**
 * Metnin şifrelenmiş halinin maksimum uzunluğu.
 * */
#define MAX_ENCODED_TEXT 20000

typedef struct Node Node;
typedef struct PriorityQueue PriorityQueue;

/**
 * Min Heap yapısı ile implement edilen Priority Queue veri yapısı.
 * İçerisinde toplam eleman sayısını ve Heap elemanlarının tutulduğu Node* dizisi barındırır.
 * */
struct PriorityQueue {
    int size;
    Node **arr;
};

/**
 * Heap ve Tree için ortak kullanılan düğüm veri yapısı.
 * Düğümde hangi harfin yer aldığı ve ne sıklığa sahip olduğu bilgisi yer almaktadır.
 * Heap içerisinde iken left ve right kullanılmamaktadır.
 * Tree içerisinde gezinti yapılabilmesi için left ve right kullanılır.
 * */
struct Node {
    char data;
    int freq;
    Node *left;
    Node *right;
};

Node *buildHuffmanTree(char *);

void decode(Node *, int *, char *, char *);

void encode(Node *, char *, char *[]);

void printCodes(char *[]);

int isLeaf(Node *);

int main() {
    /** Kullanıcıdan metin girdisi alınması */
    char text[MAX_TEXT];
    printf("Bir metin girin: ");
    gets(text);
    printf("\n");
    /** ** */

    int textLength = (int) strlen(text);

    /** Huffman ağacının oluşturulması */
    Node *tree = buildHuffmanTree(text);
    /** ** */


    /** Her bir harfe karşılık gelen Huffman kodlarının diziye kaydedilmesi */
    char *huffmanCodes[N] = {0};
    encode(tree, "", huffmanCodes);

    printf("Olusturulan Huffman kodlari su sekilde:\n");
    printCodes(huffmanCodes);
    /** ** */


    /** Metnin şifrelenmesi */
    char encoded[MAX_ENCODED_TEXT] = "";
    int i;
    for (i = 0; i < textLength; i++) {
        char c = text[i];
        strcat(encoded, huffmanCodes[c]);
    }

    printf("\nSifrelenen metin: %s", encoded);
    /** ** */

    int encodedSize = (int) strlen(encoded);

    /** Metnin şifrelenmiş halinden geriye çevrilmesi */
    char decoded[MAX_TEXT] = "";
    if (isLeaf(tree)) {
        // tamamen tek karakterden oluşan metinler için
        while (tree->freq > 0) {
            printf("%c", tree->data);
            tree->freq--;
        }
    } else {
        // harf teker teker ağaç üzerinden gezerek bulur
        int index = -1;
        while (index < encodedSize - 1) {
            decode(tree, &index, encoded, decoded);
        }
    }

    printf("\n\nSifresi cozulen metin: %s\n", decoded);
    /** ** */
    return 0;
}

/**
 * Priority Queue dizisi içerisindeki iki elemanı değiş tokuş eder.
 * */
void swap(PriorityQueue *pq, int i1, int i2) {
    Node *temp = pq->arr[i1];
    pq->arr[i1] = pq->arr[i2];
    pq->arr[i2] = temp;
}

/**
 * Belirli bir indise eklenen bir elemanı
 * alt elemanlarından büyük olmayana veya bir yaprak olana kadar
 * Heap içerisinde aşağıya taşır.
 * */
void siftUp(PriorityQueue *pq, int index) {
    // Kök düğümde olunduğundan ötürü
    // rekürsif işleme son verilir
    if (index == 0) {
        return;
    }
    int parentIndex = (index - 1) / 2;
    // Üstteki düğüm şu anki düğümden büyük olmadığı için
    // rekürsif işleme son verilir
    if (pq->arr[parentIndex]->freq <= pq->arr[index]->freq) {
        return;
    }
    // İki düğüm yer değiştirir
    swap(pq, index, parentIndex);
    // İşleme rekürsif olarak devam edilir
    siftUp(pq, parentIndex);
}

/**
 * Belirli bir indise eklenen bir elemanı
 * üst elemanlarından büyük olana veya kök düğüm olana kadar
 * Heap içerisinde yukarıya taşır.
 * */
void siftDown(PriorityQueue *pq, int index) {
    int lastParentIndex = (pq->size) / 2 - 1;
    // Şu anki düğüm bir ebeveyn değilse
    // rekürsif işleme son verilir
    if (index > lastParentIndex) {
        return;
    }

    int leftChildIndex = index * 2 + 1;
    int rightChildIndex = index * 2 + 2;

    int smallestIndex = index;
    if (leftChildIndex < pq->size && pq->arr[leftChildIndex]->freq < pq->arr[index]->freq) {
        smallestIndex = leftChildIndex;
    }
    if (rightChildIndex < pq->size && pq->arr[rightChildIndex]->freq < pq->arr[smallestIndex]->freq) {
        smallestIndex = rightChildIndex;
    }
    if (smallestIndex == index) {
        return;
    }
    swap(pq, index, smallestIndex);
    siftDown(pq, smallestIndex);
}

/**
 * Priority Queue'daki bulunan ilk elemanı -yani en küçük elemanı- siler.
 * Bu eleman silindikten sonra PQ, Min Heap yapısını sağlayacak şekilde gerekli düzenlemeler yapılır.
 * En başta PQ'dan çıkarılan elemanı geriye döndürür.
 * */
Node *poll(PriorityQueue *pq) {
    if (pq->size == 0) {
        // Boş Priority Queue üzerinde poll yapılamaz
        return NULL;
    }
    // Dizinin ilk elemanı geçici bir değişkende saklanır
    Node *temp = pq->arr[0];

    // Dizinin boyutu bir azaltılır
    // Dizideki son eleman ilk elemanın yerine getirilir
    int i = --(pq->size);
    pq->arr[0] = pq->arr[i];
    pq->arr[i] = NULL;

    // En üste eklenen yeni eleman için
    // gerekli kaydırmalar yapılır
    siftDown(pq, 0);

    // Geçici değişkendeki dizinin en baştaki ilk elemanı geri döndürülür
    return temp;
}

/**
 * Priority Queue'da bulunan ilk elemanı -yani en küçük elemanı- döndürür.
 * */
Node *peek(PriorityQueue *pq) {
    return pq->size == 0 ? NULL : pq->arr[0];
}

/**
 * Priority Queue içerisine eleman ekler.
 * */
void add(PriorityQueue *pq, Node *n) {
    // NULL olan eleman eklenemez
    if (n == NULL) {
        return;
    }
    int i = pq->size;
    // Kapasitenin üstünde elaman eklenemez
    if (i + 1 >= MAX_PRIORITY_QUEUE) {
        return;
    }

    // Dizinin boyutu bir artırılır
    pq->size = i + 1;
    // Eleman dizinin en sonuna eklenir
    pq->arr[i] = n;
    // En alta eklenen yeni eleman için
    // gerekli kaydırmalar yapılır
    siftUp(pq, i);
}

/**
 * Verilen metin için Huffman ağacı oluşturup ilk düğümü geriye döndürür.
 * */
Node *buildHuffmanTree(char *text) {
    int textLength = (int) strlen(text);
    if (textLength == 0) {
        // Boş string üzerinden işlem yapılamaz
        return NULL;
    }

    /** Sıklık dizisinin oluşturulması */
    int freq[N] = {0};
    int i;
    for (i = 0; i < textLength; i++) {
        char c = text[i];
        freq[c]++;
    }
    /** ** */

    /** Priority Queue veri yapısının oluşturulması */
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    pq->size = 0;
    pq->arr = (Node **) malloc(MAX_PRIORITY_QUEUE * sizeof(Node *));
    /** ** */

    /** Sıklık dizisindeki tüm elemanların Priority Queue içerisine eklenmesi */
    for (i = 0; i < N; i++) {
        if (freq[i] > 0) {
            Node *node = (Node *) malloc(sizeof(Node));
            node->data = (char) i;
            node->freq = freq[i];
            node->left = NULL;
            node->right = NULL;
            add(pq, node);
        }
    }
    /** ** */

    /** Priority Queue kullanılarak Huffman Tree oluşturulması */
    // Priority Queue içerisinde tek eleman kalana kadar
    // devam edecek bir döngü başlatılır
    while (pq->size != 1) {
        // Priority Queue içinden sırayla iki eleman çekilir
        // Bu iki eleman en düşük sıklığa sahip iki eleman olmak durumundadır
        Node *left = poll(pq);
        Node *right = poll(pq);

        // En küçük iki düğüm kullanılarak bunların ebeveyni olacak
        // yeni bir düğüm oluşturulur
        // Bu düğümün sıklık değeri iki çocuğunun sıklık toplamı olacaktır
        // Oluşturulan bu yeni düğüm Priority Queue içerisine eklenir
        int sum = left->freq + right->freq;
        Node *node = (Node *) malloc(sizeof(Node));
        node->data = '\0'; // Ebeveyn düğüm veri taşımadığı için NULL karakteri atanır
        node->freq = sum;
        node->left = left;
        node->right = right;
        add(pq, node);
    }
    /** ** */

    // Priority Queue'da kalan son eleman geriye döndürülür.
    // Bu düğüm en yüksek sıklığa sahip olan kök düğümdür.
    return peek(pq);
}

/**
 * Verilen ağaç düğümünün bir yaprak olup olmadığını döndürür.
 * */
int isLeaf(Node *tree) {
    return tree->left == NULL && tree->right == NULL;
}

/**
 * İki string ifadeyi birleştirip yeni bir string döndürür.
 * */
char *strAppend(char *source, char *appendix) {
    char *copy = (char *) malloc((strlen(source) + strlen(appendix) + 1) * sizeof(char));
    strcpy(copy, source);
    strcat(copy, appendix);
    return copy;
}

/**
 * Huffman ağacını kullanarak tüm Huffman kodlarını çıkartıp
 * verilen kod dizisine ekler.
 * */
void encode(Node *root, char *str, char *codes[]) {
    // Düğüm olmayan bir yere gelindiği için
    // rekürsif işleme son verilir
    if (root == NULL) {
        return;
    }
    // Gelinen düğüm bir yapraksa
    // kodları yer aldığı diziye kod değeri eklenir
    if (isLeaf(root)) {
        codes[root->data] = str;
        return;
    }
    // Var olan kodun sonuna 0 eklenerek sola dallanılır
    encode(root->left, strAppend(str, "0"), codes);
    // Var olan kodun sonuna 1 eklenerek sağa dallanılır
    encode(root->right, strAppend(str, "1"), codes);
}

/**
 * Verilen kod dizisindeki tüm kodları yazdırır.
 */
void printCodes(char *codes[]) {
    int i;
    for (i = 0; i < N; i++) {
        if (codes[i] != NULL) {
            printf("%c -> %s\n", i, codes[i]);
        }
    }
}

/**
 * Verilen char'ı string'e dönüştürüp geri döndürür.
 * */
char *charToStr(char c) {
    char *cToStr = malloc(2 * sizeof(char));
    cToStr[0] = c;
    cToStr[1] = '\0';
    return cToStr;
}

/**
 * Huffman ağacını kullanarak
 * verilen metnin belirli bir indisinden itibaren bir harfin şifresini çözer
 * ve bunu verilen şifrelenmiş metin string'ine ekler.
 * */
void decode(Node *root, int *index, char *str, char *decodedText) {
    // Düğüm olmayan bir yere gelindiği için
    // rekürsif işleme son verilir
    if (root == NULL) {
        return;
    }

    // Gelinen düğüm bir yapraksa
    // şifrelenmiş metin string'ine bu düğümün değerini ekler
    if (isLeaf(root)) {
        strcat(decodedText, charToStr(root->data));
        return;
    }
    // İndis bir artırılır
    (*index)++;
    // Eğer şu an bakılan karakter 0 ise sola dallan
    // değilse -yani 1 ise- sağa dallan
    if (str[*index] == '0') {
        root = root->left;
    } else {
        root = root->right;
    }
    // İşleme rekürsif olarak devam et
    decode(root, index, str, decodedText);
}