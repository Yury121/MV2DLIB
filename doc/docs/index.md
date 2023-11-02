Welcome to MV2 dynamic library
========================

**MV2** is a symmetric cryptographic system designed for cryptographic transformation (encryption) of an arbitrary length plaintext (open message) into a ciphertext consisting of two parts. By setting parameters of the cipher one may control lengths of the parts. This cryptographic system doesn't belong to neither block class ciphers, nor to the stream class ciphers. The first component of the cascade MV2 is a stream cipher. 
   MV2 is an iterative probabilistic cipher, where each iteration resembles a round of a substitution-permutation network when not a single block as in block ciphers, but rather the whole message is being processed. The Substitution-Permutation Network (SPN) is a fundamental architecture of block ciphers, based on concepts of confusion and diffusion. 
The output data consist of two binary sequences we call core and string of flags or flags. The core is a remainder obtained at the last round. A String of flags is a concatenation of output flags obtained at all transformation rounds. 


MV2 presents several specific and advantageous characteristics: 

1. MV2 can complement and co-exist with well-known and proven encryption technologies; most of the commercially
valuable benefits can be achieved without changing existing corporate encryption technologies and policies.

2. MV2 "one-time" encryption produces unique, tamperproof ciphertext even when encrypting the same plaintext
repeatedly using the same key.

3. MV2  ability to require the presence of 3 or more files to read encrypted messages; this provides substantial
flexibility and savings when moving or securing data.

To understand where this algorithm can be used, you can look at the book [Harmed texts](Harmed texts.pdf).

#[Release notes](release-notes.md)
#[User's structure and functions](procedures.md)
#[Examples](examples.md)
#[Project content](components.md)
#[License](license.md)
