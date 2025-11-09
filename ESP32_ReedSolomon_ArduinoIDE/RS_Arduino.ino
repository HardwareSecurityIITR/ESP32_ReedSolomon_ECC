/*
   Reed-Solomon demo for ESP32 (Arduino IDE)
   ------------------------------------------------
   • Uses the original RSCODE library (galois.c, rs.c, …)
   • Hard-coded message (change it if you want)
   • Introduces 5 random byte errors
   • Decodes / corrects / verifies
   ------------------------------------------------
*/

#include "ecc.h"

static const char *TAG = "RS";

unsigned char codeword[256];          // full codeword (msg + parity)
unsigned char original_msg[] = "Hello, ESP32! This is a Reed-Solomon test message.";
int   nbytes;                         // length of original_msg
int   ML;                             // nbytes + NPAR

/* ------------------------------------------------------------------ */
/* Helper: print a buffer in hex (uses Serial)                        */
void print_hex(const char *title, const uint8_t *data, int len)
{
  Serial.printf("%s (%d bytes): ", title, len);
  for (int i = 0; i < len; ++i) Serial.printf("%02X ", data[i]);
  Serial.println();
}

/* ------------------------------------------------------------------ */
void setup()
{
  Serial.begin(115200);
  while (!Serial) ;               // wait for serial port (optional)

  Serial.println("\n=== Reed-Solomon ESP32 Demo ===\n");

  /* ---- 1. Initialise the ECC library -------------------------------- */
  initialize_ecc();

  /* ---- 2. Prepare message ------------------------------------------- */
  nbytes = strlen((char *)original_msg);
  ML     = nbytes + NPAR;

  Serial.printf("Original message (%d bytes): \"%s\"\n", nbytes, original_msg);
  print_hex("Original (hex)", original_msg, nbytes);

  /* ---- 3. Encode ---------------------------------------------------- */
  encode_data(original_msg, nbytes, codeword);
  print_hex("Encoded codeword", codeword, ML);

  /* ---- 4. Corrupt with 5 random byte errors ----------------------- */
  randomSeed(esp_random());       // ESP32 hardware RNG
  Serial.println("Injecting 5 random errors:");
  for (int i = 0; i < 5; ++i) {
    int loc = random(ML);                 // 0 … ML-1
    uint8_t err = random(256);
    uint8_t old = codeword[loc];
    codeword[loc] = err;
    Serial.printf("  [%d] pos %3d  %02X → %02X\n", i + 1, loc, old, err);
  }
  print_hex("Corrupted codeword", codeword, ML);
  Serial.printf("Corrupted string: \"%.*s\"\n", nbytes, codeword);  // <-- NEW LINE

  /* ---- 5. Decode & correct ------------------------------------------ */
  decode_data(codeword, ML);

  int syndrome = check_syndrome();
  Serial.printf("\nSyndrome non-zero? %s\n", syndrome ? "YES" : "NO");

  int erasures[] = {0};                 // no known erasures
  int nerasures = 0;

  int ok = correct_errors_erasures(codeword, ML, nerasures, erasures);
  Serial.printf("Correction %s\n", ok ? "SUCCESS" : "FAILED");

  if (ok) {
    print_hex("Corrected codeword", codeword, ML);
    print_hex("Recovered message", codeword, nbytes);
    Serial.printf("Recovered string: \"%.*s\"\n", nbytes, codeword);

    /* ---- 6. Verify -------------------------------------------------- */
    bool match = (memcmp(original_msg, codeword, nbytes) == 0);
    Serial.printf("\nVerification: %s\n", match ? "PERFECT!" : "MISMATCH!");
  }

  Serial.println("\n=== Demo finished ===");
}

void loop()
{
  // nothing – everything runs once in setup()
}
