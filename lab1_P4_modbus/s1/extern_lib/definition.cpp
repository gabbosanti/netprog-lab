#include <bm/bm_sim/extern.h>
#include <bm/bm_sim/field_lists.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sstream>

#include <cstdint>
#include <cstring>
#include <array>
#include <vector>
#include <ios>
#include <iomanip>

#include <bits/stdc++.h>

using namespace std;

// ------------- start defines for sha
#define int int64_t // 64-bit integer
// Using 64-bit integers, to avoid overflow, during additions
// But, in the end, we have to take modulo of each number with 2^32, making all numbers having MSB<=31

// Global Variables
int modulo=pow(2,32);

std::string sha256_hash_1024_internal(bm::Data & b, bm::Data & c, bm::Data & d, std::string e);
std::string get_hash(std::string s);

// ------------- end defines for sha

const long max_size_content = 256; //in byte

// The number of columns comprising a state in AES. This is a parameter
// that could be 4, 6, or 8.  For this example we set it to 4.
#define Nb 4

// The number of rounds in AES Cipher. It is initialized to zero. 
// The actual value is computed from the input.
int Nr=0;

// The number of 32 bit words in the key. It is initialized to zero. 
// The actual value is computed from the input.
int Nk=0;

// in - the array that holds the plain text to be encrypted.
// out - the array that holds the cipher text.
// state - the array that holds the intermediate results during encryption.
unsigned char in[2048], out[2048], state[4][Nb];

// The array that stores the round keys.
unsigned char RoundKey[240];

// The Key input to the AES Program
unsigned char Key[32];

int getSBoxValue(int num) {
   int sbox[256] = {
      // 0     1     2     3     4     5     6     7
      // 8     9     A     B     C     D     E     F
      0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 
      0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
      0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 
      0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
      0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 
      0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
      0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 
      0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
      0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 
      0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
      0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 
      0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
      0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 
      0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
      0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 
      0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
      0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 
      0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
      0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 
      0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
      0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 
      0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
      0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 
      0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
      0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 
      0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
      0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 
      0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
      0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 
      0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
      0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 
      0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F
   return sbox[num];
}

// The round constant word array, Rcon[i], contains the values given by 
// x to the power (i-1) being powers of x (x is denoted as {02}) in the 
// field GF(28).  Note that i starts at 1, not 0).
int Rcon[255] = {
         0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
   0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 
   0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 
   0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 
   0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 
   0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 
   0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 
   0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 
   0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 
   0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 
   0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 
   0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 
   0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 
   0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 
   0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 
   0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 
   0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 
   0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 
   0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 
   0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 
   0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 
   0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 
   0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 
   0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 
   0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 
   0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 
   0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 
   0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 
   0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 
   0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 
   0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 
   0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };

int getSBoxInvert(int num) {
   int rsbox[256] = { 
      0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 
      0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
      0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 
      0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
      0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 
      0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
      0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 
      0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
      0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 
      0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
      0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 
      0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
      0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 
      0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
      0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 
      0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
      0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 
      0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
      0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 
      0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
      0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 
      0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
      0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 
      0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
      0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 
      0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
      0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 
      0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
      0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 
      0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
      0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 
      0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
   return rsbox[num];
}

// This function produces Nb(Nr+1) round keys. The round keys are used in 
// each round to encrypt the states. 
void KeyExpansion() {
   int i,j;
   unsigned char temp[4],k;
	
   // The first round key is the key itself.
   for (i=0 ; i < Nk ; i++) {
      RoundKey[i*4] = Key[i*4];
      RoundKey[i*4+1] = Key[i*4+1];
      RoundKey[i*4+2] = Key[i*4+2];
      RoundKey[i*4+3] = Key[i*4+3];
   }

   // All other round keys are found from the previous round keys.
   while (i < (Nb * (Nr+1))) {
      for (j=0 ; j < 4 ; j++) {
	 temp[j] = RoundKey[(i-1) * 4 + j];
      }
      if (i % Nk == 0) {
	 // This function rotates the 4 bytes in a word to the left once.
	 // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]
	 
	 // Function RotWord()
	 k = temp[0];
	 temp[0] = temp[1];
	 temp[1] = temp[2];
	 temp[2] = temp[3];
	 temp[3] = k;
	 
	 // SubWord() is a function that takes a four-byte input word and 
	 // applies the S-box to each of the four bytes to produce an output
         // word.
	 
	 // Function Subword()
	 temp[0] = getSBoxValue(temp[0]);
	 temp[1] = getSBoxValue(temp[1]);
	 temp[2] = getSBoxValue(temp[2]);
	 temp[3] = getSBoxValue(temp[3]);

	 temp[0] =  temp[0] ^ Rcon[i/Nk];
      } else if (Nk > 6 && i % Nk == 4) {
	 // Function Subword()
	 temp[0] = getSBoxValue(temp[0]);
	 temp[1] = getSBoxValue(temp[1]);
	 temp[2] = getSBoxValue(temp[2]);
	 temp[3] = getSBoxValue(temp[3]);
      }
      RoundKey[i*4+0] = RoundKey[(i-Nk)*4+0] ^ temp[0];
      RoundKey[i*4+1] = RoundKey[(i-Nk)*4+1] ^ temp[1];
      RoundKey[i*4+2] = RoundKey[(i-Nk)*4+2] ^ temp[2];
      RoundKey[i*4+3] = RoundKey[(i-Nk)*4+3] ^ temp[3];
      i++;
   }
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
void AddRoundKey(int round) {
   int i,j;
   for (i=0 ; i < Nb ; i++) {
      for (j=0 ; j < 4 ; j++) {
	 state[j][i] ^= RoundKey[round * Nb * 4 + i * Nb + j];
      }
   }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void SubBytes() {
   int i,j;
   for (i=0 ; i < 4 ; i++) {
      for (j=0 ; j < Nb ; j++) {
	 state[i][j] = getSBoxValue(state[i][j]);
      }
   }
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void ShiftRows() {
   unsigned char temp;

   // Rotate first row 1 columns to left	
   temp = state[1][0];
   state[1][0] = state[1][1];
   state[1][1] = state[1][2];
   state[1][2] = state[1][3];
   state[1][3] = temp;

   // Rotate second row 2 columns to left	
   temp = state[2][0];
   state[2][0] = state[2][2];
   state[2][2] = temp;

   temp = state[2][1];
   state[2][1] = state[2][3];
   state[2][3] = temp;

   // Rotate third row 3 columns to left
   temp = state[3][0];
   state[3][0] = state[3][3];
   state[3][3] = state[3][2];
   state[3][2] = state[3][1];
   state[3][1] = temp;
}

// xtime is a macro that finds the product of {02} and the argument to
// xtime modulo {1b}  
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

// MixColumns function mixes the columns of the state matrix
void MixColumns() {
   int i;
   unsigned char Tmp,Tm,t;
   for (i=0 ; i < Nb ; i++) {	
      t = state[0][i];
      Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i] ;
      Tm = state[0][i] ^ state[1][i] ; 
      Tm = xtime(Tm); 
      state[0][i] ^= Tm ^ Tmp ;
      
      Tm = state[1][i] ^ state[2][i] ; 
      Tm = xtime(Tm); 
      state[1][i] ^= Tm ^ Tmp ;

      Tm = state[2][i] ^ state[3][i] ; 
      Tm = xtime(Tm); 
      state[2][i] ^= Tm ^ Tmp ;

      Tm = state[3][i] ^ t ; 
      Tm = xtime(Tm); 
      state[3][i] ^= Tm ^ Tmp ;
   }
}

// Cipher is the main function that encrypts the PlainText.
void Cipher() {
   int i,j,round=0;

   //Copy the input PlainText to state array.
   for (i=0 ; i < Nb ; i++) {
      for (j=0 ; j < 4 ; j++) {
	 state[j][i] = in[i*4 + j];
      }
   }

   // Add the First round key to the state before starting the rounds.
   AddRoundKey(0); 
	
   // There will be Nr rounds.
   // The first Nr-1 rounds are identical.
   // These Nr-1 rounds are executed in the loop below.
   for (round=1 ; round < Nr ; round++) {
      SubBytes();
      ShiftRows();
      MixColumns();
      AddRoundKey(round);
      //DEBUG: print the round number
      //printf("Applying round number %lu\n", round);
   }
	
   // The last round is given below.
   // The MixColumns function is not here in the last round.
   SubBytes();
   ShiftRows();
   AddRoundKey(Nr);
   
   // The encryption process is over.
   // Copy the state array to output array.
   for (i=0 ; i < Nb ; i++) {
      for (j=0 ; j < 4 ; j++) {
	 out[i*4+j]=state[j][i];
      }
   }
}

int fillBlock (int sz, char *str, unsigned char *in, long inputLength) {
   int j=0;
   while (sz < inputLength) {
      if (j >= Nb*4) break;
      in[j++] = (unsigned char)str[sz];
      sz++;
   }
   // Pad the block with 0s, if necessary
   if (sz >= inputLength) for ( ; j < Nb*4 ; j++) in[j] = 0;
   return sz;   
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void InvSubBytes() {
   int i,j;
   for (i=0 ; i < 4 ; i++) {
      for (j=0 ; j < Nb ; j++) {
	 state[i][j] = getSBoxInvert(state[i][j]);
      }
   }
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void InvShiftRows() {
   unsigned char temp;

   // Rotate first row 1 columns to right	
   temp = state[1][3];
   state[1][3] = state[1][2];
   state[1][2] = state[1][1];
   state[1][1] = state[1][0];
   state[1][0] = temp;

   // Rotate second row 2 columns to right	
   temp = state[2][0];
   state[2][0] = state[2][2];
   state[2][2] = temp;
   
   temp = state[2][1];
   state[2][1] = state[2][3];
   state[2][3] = temp;

   // Rotate third row 3 columns to right
   temp = state[3][0];
   state[3][0] = state[3][1];
   state[3][1] = state[3][2];
   state[3][2] = state[3][3];
   state[3][3] = temp;
}

// xtime is a macro that finds the product of {02} and the argument to
// xtime modulo {1b}  
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

// Multiplty is a macro used to multiply numbers in the field GF(2^8)
#define Multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the
// inexperienced.  Please use the references to gain more information.
void InvMixColumns() {
   int i;
   unsigned char a,b,c,d;
   for (i=0 ; i < Nb ; i++) {	
	
      a = state[0][i];
      b = state[1][i];
      c = state[2][i];
      d = state[3][i];
		
      state[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ 
	 Multiply(c, 0x0d) ^ Multiply(d, 0x09);
      state[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ 
	 Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
      state[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ 
	 Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
      state[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ 
	 Multiply(c, 0x09) ^ Multiply(d, 0x0e);
   }
}

// InvCipher is the main function that decrypts the CipherText.
void InvCipher() {
   int i,j,round=0;

   //Copy the input CipherText to state array.
   for (i=0 ; i < Nb ; i++) {
      for (j=0 ; j < 4 ; j++) {
	 state[j][i] = in[i*4 + j];
      }
   }

   // Add the First round key to the state before starting the rounds.
   AddRoundKey(Nr); 

   // There will be Nr rounds.
   // The first Nr-1 rounds are identical.
   // These Nr-1 rounds are executed in the loop below.
   for (round=Nr-1 ; round > 0 ; round--) {
      InvShiftRows();
      InvSubBytes();
      AddRoundKey(round);
      InvMixColumns();
   }
	
   // The last round is given below.
   // The MixColumns function is not here in the last round.
   InvShiftRows();
   InvSubBytes();
   AddRoundKey(0);

   // The decryption process is over.
   // Copy the state array to output array.
   for(i=0 ; i < Nb ; i++) {
      for(j=0 ; j < 4 ; j++) {
	 out[i*4+j] = state[j][i];
      }
   }
}


long get_crypt_payload_length(long content_length) {
    return ((content_length / 16) + 1) * 16;
}
long get_shift_size(long crypt_payload_length) {
    return max_size_content - crypt_payload_length; //in byte
}

std::array<unsigned char, 4> data_to_u32_be_bytes(const bm::Data &value) {
    uint64_t raw = value.get_uint64();
    return {
        static_cast<unsigned char>((raw >> 24) & 0xff),
        static_cast<unsigned char>((raw >> 16) & 0xff),
        static_cast<unsigned char>((raw >> 8) & 0xff),
        static_cast<unsigned char>(raw & 0xff)
    };
}

void load_aes_key_material(
    bm::Data & k1, bm::Data & k2, bm::Data & k3, bm::Data & k4,
    bm::Data & k5, bm::Data & k6, bm::Data & k7, bm::Data & k8
) {
    bm::Data keys[] = { k1, k2, k3, k4, k5, k6, k7, k8 };

    Nk = 8;
    for (int i = 0; i <= 7; i++) {
        if (keys[i].get_uint64() == 0) {
            Nk = i;
            break;
        }
    }

    Nr = Nk + 6;

    Key[0] = (k1.get_uint64() & 0xff000000UL) >> 24;
    Key[1] = (k1.get_uint64() & 0x00ff0000UL) >> 16;
    Key[2] = (k1.get_uint64() & 0x0000ff00UL) >> 8;
    Key[3] = (k1.get_uint64() & 0x000000ffUL);

    Key[4] = (k2.get_uint64() & 0xff000000UL) >> 24;
    Key[5] = (k2.get_uint64() & 0x00ff0000UL) >> 16;
    Key[6] = (k2.get_uint64() & 0x0000ff00UL) >> 8;
    Key[7] = (k2.get_uint64() & 0x000000ffUL);

    Key[8] = (k3.get_uint64() & 0xff000000UL) >> 24;
    Key[9] = (k3.get_uint64() & 0x00ff0000UL) >> 16;
    Key[10] = (k3.get_uint64() & 0x0000ff00UL) >> 8;
    Key[11] = (k3.get_uint64() & 0x000000ffUL);

    Key[12] = (k4.get_uint64() & 0xff000000UL) >> 24;
    Key[13] = (k4.get_uint64() & 0x00ff0000UL) >> 16;
    Key[14] = (k4.get_uint64() & 0x0000ff00UL) >> 8;
    Key[15] = (k4.get_uint64() & 0x000000ffUL);

    if (Nk > 4) {
        Key[16] = (k5.get_uint64() & 0xff000000UL) >> 24;
        Key[17] = (k5.get_uint64() & 0x00ff0000UL) >> 16;
        Key[18] = (k5.get_uint64() & 0x0000ff00UL) >> 8;
        Key[19] = (k5.get_uint64() & 0x000000ffUL);
    }
    if (Nk > 5) {
        Key[20] = (k6.get_uint64() & 0xff000000UL) >> 24;
        Key[21] = (k6.get_uint64() & 0x00ff0000UL) >> 16;
        Key[22] = (k6.get_uint64() & 0x0000ff00UL) >> 8;
        Key[23] = (k6.get_uint64() & 0x000000ffUL);
    }
    if (Nk > 6) {
        Key[24] = (k7.get_uint64() & 0xff000000UL) >> 24;
        Key[25] = (k7.get_uint64() & 0x00ff0000UL) >> 16;
        Key[26] = (k7.get_uint64() & 0x0000ff00UL) >> 8;
        Key[27] = (k7.get_uint64() & 0x000000ffUL);
    }
    if (Nk > 7) {
        Key[28] = (k8.get_uint64() & 0xff000000UL) >> 24;
        Key[29] = (k8.get_uint64() & 0x00ff0000UL) >> 16;
        Key[30] = (k8.get_uint64() & 0x0000ff00UL) >> 8;
        Key[31] = (k8.get_uint64() & 0x000000ffUL);
    }
}

std::vector<unsigned char> normalize_payload_bytes(const std::string &input, long total_length) {
    if (total_length <= 0) return {};

    std::size_t target_size = static_cast<std::size_t>(total_length);
    std::vector<unsigned char> normalized(target_size, 0x00);

    if (input.size() >= target_size) {
        std::size_t source_start = input.size() - target_size;
        for (std::size_t i = 0; i < target_size; i++) {
            normalized[i] = static_cast<unsigned char>(input[source_start + i]);
        }
        return normalized;
    }

    std::size_t initial_padding = target_size - input.size();
    for (std::size_t i = 0; i < input.size(); i++) {
        normalized[initial_padding + i] = static_cast<unsigned char>(input[i]);
    }
    return normalized;
}

void append_hex_byte(std::string &output, unsigned char value) {
    static const char hex_chars[] = "0123456789abcdef";
    output.push_back(hex_chars[(value >> 4) & 0x0f]);
    output.push_back(hex_chars[value & 0x0f]);
}

std::string bytes_to_hex(const unsigned char *bytes, std::size_t len) {
    std::string hex;
    hex.reserve(len * 2);
    for (std::size_t i = 0; i < len; i++) {
        append_hex_byte(hex, bytes[i]);
    }
    return hex;
}

std::string bytes_to_hex(const std::vector<unsigned char> &bytes) {
    if (bytes.empty()) return "";
    return bytes_to_hex(bytes.data(), bytes.size());
}

int hex_nibble_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool is_hex_string(const std::string &value) {
    for (char c : value) {
        if (!std::isxdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

std::string lowercase_hex(const std::string &value) {
    std::string out = value;
    for (std::size_t i = 0; i < out.size(); i++) {
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    }
    return out;
}

std::vector<unsigned char> hex_to_bytes(const std::string &hex) {
    if (hex.size() % 2 != 0) return {};

    std::vector<unsigned char> out;
    out.reserve(hex.size() / 2);
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        int hi = hex_nibble_value(hex[i]);
        int lo = hex_nibble_value(hex[i + 1]);
        if (hi < 0 || lo < 0) return {};
        out.push_back(static_cast<unsigned char>((hi << 4) | lo));
    }
    return out;
}

std::string sha256_hex_bytes(const std::vector<unsigned char> &data) {
    std::string raw(data.begin(), data.end());
    return get_hash(raw);
}

std::vector<unsigned char> sha256_raw_bytes(const std::vector<unsigned char> &data) {
    std::string digest_hex = sha256_hex_bytes(data);
    std::vector<unsigned char> digest_raw = hex_to_bytes(digest_hex);
    if (digest_raw.empty()) digest_raw.assign(32, 0x00);
    return digest_raw;
}

std::vector<unsigned char> derive_hmac_key(const bm::Data &k1, const bm::Data &k2) {
    auto k1_bytes = data_to_u32_be_bytes(k1);
    auto k2_bytes = data_to_u32_be_bytes(k2);

    std::vector<unsigned char> base_key;
    base_key.reserve(8);
    base_key.insert(base_key.end(), k1_bytes.begin(), k1_bytes.end());
    base_key.insert(base_key.end(), k2_bytes.begin(), k2_bytes.end());

    const std::string label = "P4ICS-HMAC-KDF-v1";
    std::vector<unsigned char> kdf_message(label.begin(), label.end());
    std::vector<unsigned char> hmac_key = base_key;

    if (hmac_key.size() > 64) hmac_key = sha256_raw_bytes(hmac_key);
    hmac_key.resize(64, 0x00);

    std::vector<unsigned char> inner_pad(64), outer_pad(64);
    for (std::size_t i = 0; i < 64; i++) {
        inner_pad[i] = static_cast<unsigned char>(hmac_key[i] ^ 0x36);
        outer_pad[i] = static_cast<unsigned char>(hmac_key[i] ^ 0x5c);
    }

    std::vector<unsigned char> inner_data = inner_pad;
    inner_data.insert(inner_data.end(), kdf_message.begin(), kdf_message.end());
    std::vector<unsigned char> inner_hash = sha256_raw_bytes(inner_data);

    std::vector<unsigned char> outer_data = outer_pad;
    outer_data.insert(outer_data.end(), inner_hash.begin(), inner_hash.end());
    std::vector<unsigned char> derived = sha256_raw_bytes(outer_data);
    if (derived.empty()) derived.assign(32, 0x00);
    return derived;
}

std::string hmac_sha256_hex(const std::vector<unsigned char> &key_material, const std::vector<unsigned char> &message) {
    std::vector<unsigned char> hmac_key = key_material;
    if (hmac_key.size() > 64) hmac_key = sha256_raw_bytes(hmac_key);
    hmac_key.resize(64, 0x00);

    std::vector<unsigned char> inner_pad(64), outer_pad(64);
    for (std::size_t i = 0; i < 64; i++) {
        inner_pad[i] = static_cast<unsigned char>(hmac_key[i] ^ 0x36);
        outer_pad[i] = static_cast<unsigned char>(hmac_key[i] ^ 0x5c);
    }

    std::vector<unsigned char> inner_data = inner_pad;
    inner_data.insert(inner_data.end(), message.begin(), message.end());
    std::vector<unsigned char> inner_hash = sha256_raw_bytes(inner_data);

    std::vector<unsigned char> outer_data = outer_pad;
    outer_data.insert(outer_data.end(), inner_hash.begin(), inner_hash.end());
    return sha256_hex_bytes(outer_data);
}

std::string canonical_payload_hex_for_mac(const std::string &payload_hex_or_raw) {
    if ((payload_hex_or_raw.size() % 2 == 0) && is_hex_string(payload_hex_or_raw)) {
        return lowercase_hex(payload_hex_or_raw);
    }
    return bytes_to_hex(
        reinterpret_cast<const unsigned char *>(payload_hex_or_raw.data()),
        payload_hex_or_raw.size()
    );
}

std::string compute_payload_hmac_hex(const bm::Data &k1, const bm::Data &k2, const bm::Data &seqNo, const std::string &payload_hex) {
    std::vector<unsigned char> hmac_key = derive_hmac_key(k1, k2);
    std::vector<unsigned char> message;

    const std::string domain = "P4ICS-HMAC-PAYLOAD-v1";
    message.insert(message.end(), domain.begin(), domain.end());

    auto seq_bytes = data_to_u32_be_bytes(seqNo);
    message.insert(message.end(), seq_bytes.begin(), seq_bytes.end());

    std::string canonical_payload_hex = canonical_payload_hex_for_mac(payload_hex);
    message.insert(message.end(), canonical_payload_hex.begin(), canonical_payload_hex.end());

    return hmac_sha256_hex(hmac_key, message);
}

std::array<unsigned char, 16> derive_packet_iv(const bm::Data &k1, const bm::Data &k2, const bm::Data &seqNo) {
    std::vector<unsigned char> hmac_key = derive_hmac_key(k1, k2);
    std::vector<unsigned char> message;

    const std::string domain = "P4ICS-IV-v1";
    message.insert(message.end(), domain.begin(), domain.end());

    auto seq_bytes = data_to_u32_be_bytes(seqNo);
    message.insert(message.end(), seq_bytes.begin(), seq_bytes.end());

    std::string iv_hex = hmac_sha256_hex(hmac_key, message);
    std::vector<unsigned char> iv_bytes = hex_to_bytes(iv_hex);

    std::array<unsigned char, 16> iv{};
    for (std::size_t i = 0; i < iv.size() && i < iv_bytes.size(); i++) {
        iv[i] = iv_bytes[i];
    }
    return iv;
}

void append_zero_hex_bytes(std::string &output, long count) {
    if (count <= 0) return;
    output.append(static_cast<std::size_t>(count) * 2, '0');
}

std::vector<unsigned char> hash_data_to_32_bytes(const bm::Data &hash_data) {
    std::string hash_string = hash_data.get_string();
    std::vector<unsigned char> normalized;

    if (hash_string.size() == 64 && is_hex_string(hash_string)) {
        normalized = hex_to_bytes(hash_string);
    } else {
        normalized.assign(hash_string.begin(), hash_string.end());
    }

    if (normalized.size() > 32) {
        normalized.erase(normalized.begin(), normalized.begin() + (normalized.size() - 32));
    } else if (normalized.size() < 32) {
        normalized.insert(normalized.begin(), 32 - normalized.size(), 0x00);
    }
    return normalized;
}


void Decrypt(bm::Data & a, bm::Data & b, bm::Data & k1, bm::Data & k2, bm::Data & k3, bm::Data & k4, bm::Data & k5, bm::Data & k6, bm::Data & k7, bm::Data & k8, bm::Data & len, bm::Data & sha, bm::Data & seqNo, bm::Data & shaCalculated) {
    (void)sha;

    load_aes_key_material(k1, k2, k3, k4, k5, k6, k7, k8);
    KeyExpansion();

    long totalLength = len.get_uint64();
    long crypt_payload_length = get_crypt_payload_length(totalLength);
    long shift_size = get_shift_size(crypt_payload_length);

    std::vector<unsigned char> ciphertext = normalize_payload_bytes(a.get_string(), crypt_payload_length);
    std::array<unsigned char, 16> prev_cipher_block = derive_packet_iv(k1, k2, seqNo);

    std::string plaintext_hex;
    if (totalLength > 0) plaintext_hex.reserve(static_cast<std::size_t>(totalLength) * 2);

    int nBlocks = crypt_payload_length / (Nb * 4);
    int bytesNotInserted = 0;

    for (int block = 0; block < nBlocks; block++) {
        std::array<unsigned char, 16> current_cipher_block{};
        std::size_t block_offset = static_cast<std::size_t>(block) * (Nb * 4);

        for (int j = 0; j < Nb * 4; j++) {
            unsigned char cipher_byte = ciphertext[block_offset + static_cast<std::size_t>(j)];
            in[j] = cipher_byte;
            current_cipher_block[static_cast<std::size_t>(j)] = cipher_byte;
        }

        InvCipher();

        bool isLastBlock = block == (nBlocks - 1);
        int bytesInBlock = isLastBlock ? (totalLength % (Nb * 4)) : (Nb * 4);
        bytesNotInserted = (Nb * 4) - bytesInBlock;

        for (int i = 0; i < bytesInBlock; i++) {
            unsigned char plain_byte = static_cast<unsigned char>(out[i] ^ prev_cipher_block[static_cast<std::size_t>(i)]);
            append_hex_byte(plaintext_hex, plain_byte);
        }

        prev_cipher_block = current_cipher_block;
    }

    std::string shaCalculatedString = sha256_hash_1024_internal(k1, k2, seqNo, plaintext_hex);
    std::string padded_plaintext_hex = plaintext_hex;
    append_zero_hex_bytes(padded_plaintext_hex, shift_size + bytesNotInserted);

    shaCalculated.set(shaCalculatedString);
    b.set(padded_plaintext_hex);
}

void verify_hash_equals(bm::Data & equals, bm::Data & hash1, bm::Data & hash2) {
    std::vector<unsigned char> tag1 = hash_data_to_32_bytes(hash1);
    std::vector<unsigned char> tag2 = hash_data_to_32_bytes(hash2);

    unsigned char diff = 0x00;
    for (std::size_t i = 0; i < 32; i++) {
        diff |= static_cast<unsigned char>(tag1[i] ^ tag2[i]);
    }
    equals.set(diff == 0x00);
}

void Encrypt(bm::Data & a, bm::Data & b, bm::Data & k1, bm::Data & k2, bm::Data & k3, bm::Data & k4, bm::Data & k5, bm::Data & k6, bm::Data & k7, bm::Data & k8, bm::Data & len, bm::Data & seqNo) {
    load_aes_key_material(k1, k2, k3, k4, k5, k6, k7, k8);
    KeyExpansion();

    long totalLength = len.get_uint64();
    long crypt_payload_length = get_crypt_payload_length(totalLength);
    long shift_size = get_shift_size(crypt_payload_length);

    std::vector<unsigned char> plaintext = normalize_payload_bytes(a.get_string(), totalLength);
    plaintext.resize(static_cast<std::size_t>(crypt_payload_length), 0x00);

    std::array<unsigned char, 16> prev_cipher_block = derive_packet_iv(k1, k2, seqNo);

    std::string result;
    if (crypt_payload_length + shift_size > 0) {
        result.reserve(static_cast<std::size_t>(crypt_payload_length + shift_size) * 2);
    }

    int nBlocks = crypt_payload_length / (Nb * 4);
    for (int block = 0; block < nBlocks; block++) {
        std::size_t block_offset = static_cast<std::size_t>(block) * (Nb * 4);
        for (int i = 0; i < Nb * 4; i++) {
            unsigned char plain_byte = plaintext[block_offset + static_cast<std::size_t>(i)];
            in[i] = static_cast<unsigned char>(plain_byte ^ prev_cipher_block[static_cast<std::size_t>(i)]);
        }

        Cipher();

        for (int i = 0; i < Nb * 4; i++) {
            unsigned char cipher_byte = out[i];
            append_hex_byte(result, cipher_byte);
            prev_cipher_block[static_cast<std::size_t>(i)] = cipher_byte;
        }
    }

    append_zero_hex_bytes(result, shift_size);
    b.set(result);
}


// -------------------------------------------------
// START SHA FUNCTIONS
// -------------------------------------------------

// Function to Rotate Right a 32-bit integer (word), by 'x' bits
int rot_right(int word, int x_bits)
{
    return ((word>>(x_bits)) | (word<<(32-x_bits)));
}

// Function to Compute 'sigma_1(x)'
int sigma1(int x)
{
    return (rot_right(x,17)^(rot_right(x,19))^((x)>>10));
}

// Function to Compute 'sigma_0(x)'
int sigma0(int x)
{
    return (rot_right(x,7)^(rot_right(x,18))^((x)>>3));
}

// Function to Compute 'S0(x)'
int S0(int x)
{
    return (rot_right(x,2)^(rot_right(x,13))^(rot_right(x,22)));
}

// Function to Compute 'S1(x)'
int S1(int x)
{
    return (rot_right(x,6)^(rot_right(x,11))^(rot_right(x,25)));
}

// Function to Compute 'Ch(a,b,c)'
int Ch(int a, int b, int c)
{
    return (a&b)^((~a)&c);
}

// Function to Compute 'Maj(a,b,c)'
int Maj(int a, int b, int c)
{
    return (a&b)^(a&c)^(b&c);
}

// Function to Convert a String, into a String of bits, corresponding to the ASCII Value of Characters in the String
string convert_string_to_bits(string &s)
{
    string ret="";
    for(auto x:s) // Iterate through the whole string 's'
    {
        int ascii_of_x=int(x);
        bitset<8> b(ascii_of_x); // Convert 'ascii_of_x' to a 8-bit binary string, using Bitset
        ret+=b.to_string();
    }
    return ret;
}

// Function to Convert a 32-bit Integer to 'hexadecimal' String
string int_to_hex(int integer)
{
    stringstream ss;
    ss<<hex<<setw(8)<<setfill('0')<<integer;
    string ret;
    ss>>ret;
    return ret;
}

// Function to perform Pre-Processing
void pre_process(string &input_str_in_bits) // Pre-Processing Step of the Algorithm
{
    int l=int(input_str_in_bits.size());
    input_str_in_bits+="1"; // Step '1'
    int k=0;
    while(true) // Finding 'k'
    {
        int curr_length_of_string=int(input_str_in_bits.size());
        int length_of_string_after_appending=k+curr_length_of_string+64;
        if(length_of_string_after_appending%512==0)
        {
            break;
        }
        k++;
    }
    for(int zeroes=0; zeroes<k; zeroes++) // Step '2'
    {
        input_str_in_bits+="0";
    }

    // Step '3'
    bitset<64> b(l);
    input_str_in_bits+=b.to_string(); // Appending 64-bit String (= 'l' in Integer) to the end of Current String
}

// Function to break the string into chunks (blocks) of 512 bits
vector<string> break_into_chunks(string &input_str_in_bits)
{
    vector<string> ret;
    for(int i=0; i<int(input_str_in_bits.size()); i+=512)
    {
        ret.push_back(input_str_in_bits.substr(i,512)); // '1' Chunk Added to the List
    }
    return ret;
}

// Function to Resize/Convert the 512-bit Blocks to '16' 32-bit Integers
vector<int> convert_512bits_to_16integers(string &s)
{
    vector<int> ret;
    for(int i=0; i<int(s.size()); i+=32)
    {
        bitset<32> b(s.substr(i,32)); // Using Bitset to Convert String of Bits, to Integer
        ret.push_back(b.to_ulong());
    }
    for(auto &x:ret) // Take Modulo with 2^32, for every Integer
    {
        x%=modulo;
    }
    return ret;
}

// Functin to Process the Hash Function, for i'th Message Block
void process_hash_function(int i, vector<int> &curr_block, vector<array<int,8>> &H, vector<int> &k)
{
    // Here, i = Current 'Message Block' Number

    // Initialize the 8 Working Variables, using Last Hash Values
    int a=H[i-1][0];
    int b=H[i-1][1];
    int c=H[i-1][2];
    int d=H[i-1][3];
    int e=H[i-1][4];
    int f=H[i-1][5];
    int g=H[i-1][6];
    int h=H[i-1][7];

    // Create a 64-entry Message Schedule Array w[0..63] of 32-bit Integers
    int w[64];

    // Copy the '16' 32-bit Integers of the Current Message Block, to w[0..15]
    for(int j=0; j<16; j++)
    {
        w[j]=curr_block[j];
    }

    // Extend the first 16 words (32-bit Integers) into Remaining 48 [16..63] words, using Sigma Functions
    for(int j=16; j<64; j++)
    {
        w[j]=w[j-16]+sigma0(w[j-15])+sigma1(w[j-2])+w[j-7];
        w[j]%=modulo; // Take Modulo, to avoid overflow
    }

    // Main Hashing Loop
    for(int j=0; j<64; j++)
    {
        int temp1=h+S1(e)+Ch(e,f,g)+k[j]+w[j];
        int temp2=S0(a)+Maj(a,b,c);
        h=g;
        g=f;
        f=e;
        e=d+temp1;
        d=c;
        c=b;
        b=a;
        a=temp1+temp2;

        // Taking Modulo with 2^32
        e%=modulo;
        a%=modulo;
    }

    // Update Current Hash Values
    H[i][0]=H[i-1][0]+a;
    H[i][1]=H[i-1][1]+b;
    H[i][2]=H[i-1][2]+c;
    H[i][3]=H[i-1][3]+d;
    H[i][4]=H[i-1][4]+e;
    H[i][5]=H[i-1][5]+f;
    H[i][6]=H[i-1][6]+g;
    H[i][7]=H[i-1][7]+h;

    // Take Modulo with 2^32, for All Current New Hash Values
    for(int j=0; j<8; j++)
    {
        H[i][j]%=modulo;
    }
}


// Function to Process the Hash Function for all Message Blocks of 512-bit and find the Final Hash Value of the Input Message
string process_hash(vector<vector<int>> &M, vector<array<int,8>> &H, vector<int> &k)
{
    for(int i=1; i<=int(M.size()); i++) // For Each 512-bit Message Block, Process the Hash Function
    {
        process_hash_function(i,M[i-1],H,k);
    }
    string ret="";
    for(int i=0; i<8; i++)
    {
        ret+=int_to_hex(H[int(M.size())][i]);
    }
    return ret;
}



std::string get_hash(std::string s){
    // Convert the Input String to Bits
    string input_str_in_bits=convert_string_to_bits(s);

    // Do Pre-Processing on (input_str_in_bits), in the following manner:
    // 1. Append one '1' bit, to (input_str_in_bits)
    // 2. Append 'k'(>=0) '0' bits to (input_str_in_bits), such that length(input_str_in_bits) becomes
       // exactly divisible by 512 (after completion of each pre-processing sub-step)
    // 3. Append l(length of original message, in terms of bits), as a 64-bit String
    pre_process(input_str_in_bits);

    // Break the Message(input_str_in_bits) into Chunks of 512 Bits
    vector<string> chunks_of_512_bits=break_into_chunks(input_str_in_bits);

    // Convert Each 512-Bits Message Chunk into '16' 32-bit integers
    vector<vector<int>> M;
    for(auto x:chunks_of_512_bits)
    {
        M.push_back(convert_512bits_to_16integers(x));
    }

    int number_of_512bit_chunks=int(M.size());

    // Vector to Store 8 Hash Values after each iteration of every 512-bit Block
    vector<array<int,8>> H(number_of_512bit_chunks+1);

    // Assigning Initial Hash Values
    // Actually, these are:
    // First 32 bits of the fractional parts of the square roots of the first 8 primes (from 2 to 19).
    H[0][0]=0x6a09e667;
    H[0][1]=0xbb67ae85;
    H[0][2]=0x3c6ef372;
    H[0][3]=0xa54ff53a;
    H[0][4]=0x510e527f;
    H[0][5]=0x9b05688c;
    H[0][6]=0x1f83d9ab;
    H[0][7]=0x5be0cd19;

    // 'Array of Round' Constants
    // Actually, these are:
    // First 32 bits of the fractional parts of the cube roots of the first 64 primes (from 2 to 311).
    vector<int> k=
    {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Process the Hash Function of SHA-256 Algorithm, on each 512-bit block of the message successively
    string res=process_hash(M,H,k);

    return res;
}


void sha256_hash_256(bm::Data & a, bm::Data & b) {
	std::string str;
	std::string result = "";

	str = b.get_string();

	result = get_hash(str);

	a.set(result);
}

void sha256_hash_512(bm::Data & a, bm::Data & b, bm::Data & c) {
    std::string str;
    std::string result = "";

	str = b.get_string();
	str = str + c.get_string();

	result = get_hash(str);

    a.set(result);
}

void sha256_hash_1024(bm::Data & a, bm::Data & b, bm::Data & c, bm::Data & d, bm::Data & e, bm::Data & len) {
    long totalLength = len.get_uint64();
    std::vector<unsigned char> payload_bytes = normalize_payload_bytes(e.get_string(), totalLength);
    std::string payload_hex = bytes_to_hex(payload_bytes);
    std::string result = compute_payload_hmac_hex(b, c, d, payload_hex);
    a.set(result);
}

std::string sha256_hash_1024_internal(bm::Data & b, bm::Data & c, bm::Data & d, std::string e) {
	return compute_payload_hmac_hex(b, c, d, e);
}


BM_REGISTER_EXTERN_FUNCTION(sha256_hash_256, bm::Data &, bm::Data &);
BM_REGISTER_EXTERN_FUNCTION(sha256_hash_512, bm::Data &, bm::Data &, bm::Data &);
BM_REGISTER_EXTERN_FUNCTION(sha256_hash_1024, bm::Data &, bm::Data &, bm::Data &, bm::Data &, bm::Data &, bm::Data &);
BM_REGISTER_EXTERN_FUNCTION(verify_hash_equals, bm::Data &, bm::Data &, bm::Data &);



BM_REGISTER_EXTERN_FUNCTION(Encrypt, bm::Data &, bm::Data &,
    bm::Data &, bm::Data &, bm::Data &, bm::Data &,
    bm::Data &, bm::Data &, bm::Data &, bm::Data &,
    bm::Data &, bm::Data &);
BM_REGISTER_EXTERN_FUNCTION(Decrypt, bm::Data &, bm::Data &,
    bm::Data &, bm::Data &, bm::Data &, bm::Data &,
    bm::Data &, bm::Data &, bm::Data &, bm::Data &,
    bm::Data &,
    bm::Data &, bm::Data &, bm::Data &);
