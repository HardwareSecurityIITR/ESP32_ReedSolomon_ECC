/* Reed Solomon Coding for glyphs */
#include <Arduino.h>

#define NPAR 10

typedef uint32_t rs_uint32_t;
typedef uint16_t rs_uint16_t;

#define MAXDEG (NPAR * 2)

/* Encoder parity bytes */
extern int pBytes[MAXDEG];

/* Decoder syndrome bytes */
extern int synBytes[MAXDEG];

/* print debugging info */
extern int DEBUG;

/* Reed Solomon encode/decode routines */
void initialize_ecc(void);
int check_syndrome(void);
void decode_data(unsigned char data[], int nbytes);
void encode_data(unsigned char msg[], int nbytes, unsigned char dst[]);

/* CRC-CCITT checksum generator */
rs_uint16_t crc_ccitt(unsigned char *msg, int len);

/* galois arithmetic tables */
extern int gexp[];
extern int glog[];

void init_galois_tables(void);
int ginv(int elt);
int gmult(int a, int b);

/* Error location routines */
int correct_errors_erasures(unsigned char codeword[], int csize,
                            int nerasures, int erasures[]);

/* polynomial arithmetic */
void add_polys(int dst[], int src[]);
void scale_poly(int k, int poly[]);
void mult_polys(int dst[], int p1[], int p2[]);
void copy_poly(int dst[], int src[]);
void zero_poly(int poly[]);
