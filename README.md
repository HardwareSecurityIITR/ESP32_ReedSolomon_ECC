🛡️ RSCODE-EMBEDDED: Reed-Solomon ECC Library for Arduino/ESP32

This repository contains a highly portable C/C++ implementation of the Reed-Solomon (RS) error correction coding algorithm. It is derived from the robust and widely-used RSCODE library by Henry Minsky, with minor adaptations to ensure compatibility and efficiency in resource-constrained environments like Arduino and ESP32/ESP8266 microcontrollers.

This library is essential for applications requiring reliable data communication or storage in noisy environments, such as wireless links (LoRa), non-volatile memory (Flash/SD card), or industrial bus communication.

🌟 Features

Robust Correction: Capable of correcting a configurable number of random byte errors and/or known erasures within the codeword.

Optimized GF(256) Arithmetic: Utilizes pre-computed exponent (gexp) and logarithm (glog) lookup tables for extremely fast Galois Field multiplication and inversion.

Standard Algorithms: Implements the classic and proven Modified Berlekamp-Massey Algorithm for finding the error-locator polynomial and Chien's Search for root-finding (error locations).

Portability: The core C/C++ logic is highly modular and has minimal dependencies, making it easy to integrate into other embedded or host systems.

Auxiliary CRC-CCITT: Includes a dedicated, hardware-modeled CRC-CCITT function for basic, fast error detection (separate from the RS correction).

Embedded Focus: All memory is statically allocated, avoiding dynamic allocation (malloc/new) to ensure reliable operation on embedded platforms.

🚀 Getting Started: ESP32 Demo

The included RS_Arduino.ino sketch demonstrates a complete encode, corrupt, decode, and verify cycle on an ESP32 board.

1. Prerequisites

Arduino IDE or VSCode with PlatformIO.

ESP32 Board Package installed.

A basic understanding of serial communication.

2. Setup and Compilation

Clone the Repository or download the source files.

Create a new folder in your Arduino sketches directory (e.g., ~/Arduino/RS_Arduino_Demo).

Place all the C/C++ files (ecc.h, rs.cpp, galois.cpp, berlekamp.cpp, crcgen.cpp) and the demo sketch (RS_Arduino.ino) into this folder.

Open RS_Arduino.ino in the Arduino IDE.

Select your ESP32 board and upload the sketch.

3. Running the Demo

Open the Serial Monitor at 115200 baud. The demo will execute the following steps:

Initialization: Initializes the Galois Field lookup tables and computes the Reed-Solomon generator polynomial.

Encoding: Encodes a hard-coded message (default length is 50 bytes) with $\text{NPAR}=10$ parity bytes, creating a 60-byte codeword.

Corruption: Randomly injects 5 single-byte errors into the 60-byte codeword.

Decoding & Correction: Calculates the syndrome, runs the Berlekamp-Massey algorithm, and corrects all 5 errors.

Verification: Confirms that the recovered message perfectly matches the original data.

You should see output similar to:

=== Reed-Solomon ESP32 Demo ===
...
Injecting 5 random errors:
  [1] pos 11  32 -> D4
  [2] pos 59  68 -> 1A
...
Syndrome non-zero? YES
Correction SUCCESS
Recovered string: "Hello, ESP32! This is a Reed-Solomon test message."
Verification: PERFECT!


⚙️ Configuration and Capability

The core capability of the Reed-Solomon code is controlled by a single preprocessor constant defined in ecc.h:

Constant

Default Value

Description

NPAR

10

The number of parity bytes to append to the message.

Error Correction Capability

The number of errors ($t$) or erasures ($e$) the code can correct is determined by the formula:

$$\text{Number of Parity Bytes (NPAR)} \ge 2t + e$$

With the default $\text{NPAR}=10$:

Error Correction: You can correct up to $t = 5$ random byte errors (since $2 \times 5 = 10$).

Erasure Correction: If you know the locations of the errors (erasures), you could correct up to $e = 10$ erasures (e.g., if a data packet is entirely lost, but its position is known).

Combined Correction: You could correct, for example, 2 random errors and 6 erasures ($2 \times 2 + 6 = 10$).

Maximum Codeword Length

This implementation uses GF(256), which is based on $2^8$ field elements (bytes). The maximum codeword length $N$ is limited to $2^8 - 1 = 255$ bytes, including both the message and the parity bytes.

🔬 Core Module Breakdown

The library is logically separated into distinct modules handling the different stages of ECC processing.

galois.cpp (Galois Field Arithmetic)

This is the mathematical foundation of the entire library.

Field: $\text{GF}(2^8)$ (Galois Field with $2^8 = 256$ elements).

Primitive Polynomial (PPOLY): The irreducible polynomial used to define the field is $x^8 + x^4 + x^3 + x^2 + 1$ (represented as 0x1D).

Key Functions:

init_galois_tables(): Pre-computes the gexp (exponent) and glog (logarithm) tables.

gmult(a, b): Performs field multiplication using table lookups, achieving significant speedup over standard bitwise operations.

ginv(elt): Calculates the multiplicative inverse.

rs.cpp (Encoder & Syndrome Calculator)

This module handles the core polynomial operations for encoding and checking for errors.

initialize_ecc(): Calls the Galois initializer and then computes the Generator Polynomial $g(x)$. The generator polynomial is the product of $(\text{x} - \alpha^1)(\text{x} - \alpha^2)\dots(\text{x} - \alpha^{\text{NPAR}})$, where $\alpha$ is a primitive element of $\text{GF}(256)$.

encode_data(): Generates the parity check bytes $p(x)$ by performing polynomial division of the message $m(x)$ by the generator polynomial $g(x)$ using an LFSR simulation.

decode_data(): Calculates the Syndrome Bytes ($S_j$), which are the core indicators of errors. $S_j$ is found by evaluating the received codeword $r(x)$ at the roots of $g(x)$.

check_syndrome(): Returns true if any syndrome byte is non-zero, indicating that an uncorrectable or correctable error occurred.

berlekamp.cpp (Error/Erasure Correction)

This module contains the sophisticated logic to correct the detected errors.

correct_errors_erasures(): The main entry point for correction.

Modified_Berlekamp_Massey(): Takes the syndrome bytes and iteratively finds the coefficients of the Error Locator Polynomial $\Lambda(x)$. This polynomial has roots corresponding to the location of the errors.

Find_Roots(): Performs Chien's Search, which is a process of evaluating $\Lambda(x)$ at every power of $\alpha$ (i.e., $\alpha^0, \alpha^1, \dots, \alpha^{254}$) to find the error locations.

Error Correction: Once the locations are known, the error magnitude is calculated using Forney's formula (based on the Error Evaluator Polynomial $\Omega(x)$) and the corrupted byte is fixed.

crcgen.cpp (CRC-CCITT Utility)

This is an auxiliary function for quick error detection (used in conjunction with RS for added security, or standalone).

crc_ccitt(unsigned char *msg, int len): Calculates the 16-bit CRC-CCITT checksum.

Polynomial: Uses the standard $\text{CRC-CCITT}$ polynomial: $x^{16} + x^{12} + x^5 + 1$ (represented as 0x1021).

The implementation simulates a hardware register shift-register process for calculating the CRC.

✍️ Usage Notes and Best Practices

Initialization is Mandatory: Always call initialize_ecc() once in your setup() function before any encoding or decoding operations.

Codeword Buffer: Ensure your array (codeword) is large enough to hold the message data plus the NPAR parity bytes.

Encoding: The encode_data function generates the parity and places it immediately following the message data in the destination buffer.

Decoding: The decode_data function calculates the syndrome based on the received (potentially corrupted) codeword. correct_errors_erasures then modifies the codeword buffer in place to restore the original data.

📜 License and Attribution

This library is a port and adaptation of the RSCODE library originally created by Henry Minsky (hqm@alum.mit.edu).

The original work is licensed under the GNU GENERAL PUBLIC LICENSE (GPL). The use, modification, and distribution of this derived work are governed by the terms of the GPL, ensuring it remains free software.

For any commercial applications or alternative licensing inquiries regarding the original RSCODE library, please refer to the author's official source code page.
