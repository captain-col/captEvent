#ifndef _TSHA1_HXX_
#define _TSHA1_HXX_

namespace CP {
    class TSHA1;
}

// Implement the Secure Hashing Standard.
//
// Copyright (C) 1998, 2009
// Paul E. Jones <paulej@packetizer.com>
// All Rights Reserved.
//
// Freeware Public License (FPL)
//
// This software is licensed as "freeware."  Permission to distribute
// this software in source and binary forms, including incorporation 
// into other products, is hereby granted without a fee.  THIS SOFTWARE 
// IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESSED OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
// AND FITNESS FOR A PARTICULAR PURPOSE.  THE AUTHOR SHALL NOT BE HELD 
// LIABLE FOR ANY DAMAGES RESULTING FROM THE USE OF THIS SOFTWARE, EITHER 
// DIRECTLY OR INDIRECTLY, INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA 
// OR DATA BEING RENDERED INACCURATE.
//
// Modified by C. McGrew for use in T2K ND280 software.
// 
// Description:
//     This class implements the Secure Hashing Standard as defined
//     in FIPS PUB 180-1 published April 17, 1995.
// 
//     Many of the variable names in this class, especially the single
//     character names, were used because those were the names used
//     in the publication.
class CP::TSHA1 {

public:

    TSHA1();
    virtual ~TSHA1();

    /**
     *  Re-initialize the class
     */
    void Reset();

    /**
     *  Returns the message digest
     */
    bool Result(unsigned *message_digest_array);

    /**
     *  Provide input to SHA1
     */
    void Input( const unsigned char *message_array,
                unsigned            length);
    void Input( const char  *message_array,
                unsigned    length);
    void Input(unsigned char message_element);
    void Input(char message_element);
    TSHA1& operator<<(const char *message_array);
    TSHA1& operator<<(const unsigned char *message_array);
    TSHA1& operator<<(const char message_element);
    TSHA1& operator<<(const unsigned char message_element);

    /// Add an integer to the sha1 hash (added by CDM)
    void Input(int integer_element);

    /// Add an integer to the sha1 hash (added by CDM)
    void Input(unsigned int integer_element);

    /// Add a double to the sha1 hash (added by CDM)
    void Input(double float_element);

private:

    /**
     *  Process the next 512 bits of the message
     */
    void ProcessMessageBlock();

    /**
     *  Pads the current message block to 512 bits
     */
    void PadMessage();

    /**
     *  Performs a circular left shift operation
     */
    inline unsigned CircularShift(int bits, unsigned word);

    unsigned H[5];                      // Message digest buffers

    unsigned Length_Low;                // Message length in bits
    unsigned Length_High;               // Message length in bits

    unsigned char Message_Block[64];    // 512-bit message blocks
    int Message_Block_Index;            // Index into message block array

    bool Computed;                      // Is the digest computed?
    bool Corrupted;                     // Is the message digest corruped?
    
};
#endif
