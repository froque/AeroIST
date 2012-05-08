#define ISAADDRESS_BETA 0x230
#define ISAADDRESS_ALPHA 0x240

void init_bta(int isaaddress);
int read_bta(int isaaddress);
void write_bta(int isaaddress,int value);
