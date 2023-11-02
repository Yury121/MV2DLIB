release-notes.md
================

MV2 is a symmetric cryptographic system designed for cryptographic transformation (encryption) of an arbitrary length plaintext (open message) into a ciphertext consisting of two parts. By setting parameters of the cipher one may control lengths of the parts. This cryptographic system doesn't belong to neither block class ciphers, nor to the stream class ciphers. The first component of the cascade MV2 is a stream cipher. 
   MV2 is an iterative probabilistic cipher, where each iteration resembles a round of a substitution-permutation network when not a single block as in block ciphers, but rather the whole message is being processed. The Substitution-Permutation Network (SPN) is a fundamental architecture of block ciphers, based on concepts of confusion and diffusion. 
The output data consist of two binary sequences we call core and string of flags or flags. The core is a remainder obtained at the last round. A String of flags is a concatenation of output flags obtained at all transformation rounds. 

To understand where this algorithm can be used, you can look at the book [Harmed texts](Harmed texts.pdf).

This project contains source files for implementing dynamic libraries for Linux and Windows(C) for x86/x64 processors.
It also contains documentation and test modules for testing and demonstrating the use of the libraries.

The documentation is written as regular [Markdown files](https://daringfireball.net/projects/markdown/) and compiled using the [MkDocs](https://www.mkdocs.org/) static site generator.

All trademarks are the property of their respective owners.

