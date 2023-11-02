#User's structure and functions
To work, you must specify a key and use the encryption or decryption function. A key is an arbitrary sequence of bytes of 16, 32, 64 or 256 bytes in size.

This library is multi-threaded. Different threads use different memory objects to manipulate data.
When a thread calls **MV2SetKey** function for the first time, an MV2 object is created to implement the main functions. The MV2Release function is used to free up the memory of an object.
When the library is loaded into the calling process's memory, an object is created to implement the basic functionality.

##Functions
###MV2SetKey 
This function creates an MV2 object if it does not exist and sets the encryption/decryption key defined as
```c
bool MV2SetKey( unsigned char * srcKey, int szKey ); 
```
**Parameters:**  
***srcKey*** - [in] Pointer to the buffer containing the key. To be leveled up to the unsigned int size.  
***szKey*** -  [in] Size of the key in bytes. Current options are 16(128bits), 32(256 bits), 64 (512 bits) 128 (1024 bits) or 256 (2048 bits).

**Return Values**  
   If the function succeeds, the return value is TRUE. 

**Remarks**  
    This function must be called before encrypting or decrypting data. 

###MV2Encrypt 
This function encrypts the data and is defined as
```c
bool MV2Encrypt( MV2_BUFFER &mv2Buf, long RestLength ); 
```
**Parameters**  
***mv2Buf*** - [in] \ [out] Structure containing address and size of buffers for input and output data        	      ***SrcBuf*** - [in] - pointer to the data block to be encrypted  
   ***szSrcBuf*** - [in] - size of data block to be encrypted in bytes; 
   ***CoreBuf*** - [in\out] - pointer to the memory area in which the core is to be placed; 
   ***szCoreBuf*** - [in] - max size of memory area in bytes; [out] - size of the output core in bytes 
   ***FlagBuf*** - [in\out] - pointer to the memory area in which flags are to be placed; 
   ***szFlagBuf*** - [in] - max size of memory area in bytes; [out] - size of the output flags in bytes 
   ***RetCode*** - [out] - termination code;  
***RestLength*** - [in] Encryption parameter. Values from 1 to 50 set the number of encryptions that have to be performed. Values more than **50** set max length for the core upon encryption. 

**Return Values**  
    If the function succeeds, the return value is TRUE. If RestLength is less than 0, the return value is FALSE. If an error occurs during encryption, the function returns FALSE. Termination code is in the field RetCode of the structure MV2_BUFFER. The encrypted data are located at the addresses from the fields CoreBuf and FalgBuf, the sizes of the encrypted data are located in the fields szCoreBuf and szFalgBuf of the structure MV2_BUFFER. 

**Remarks**  
    Before using the function set the encryption key by the function **MV2SetKey**. The calling process has to allocate memory before using MV2Encrypt.  
    Field ***SrcBuf*** of the input structure MV2_BUFFER points at the address of the data that have to be encrypted. Field ***szSrcBuf*** contains data size in bytes.   
    Fields ***CoreBuf*** and ***FlagBuf*** point at the addresses of memory blocks serving for allocation of the output data. 

###MV2Decrypt 
This function decrypts the data and is defined as
```c
bool MV2Decrypt(MV2_BUFFER& mv2Buf);
```
**Parameters:**  
***mv2Buf*** - [in] \ [out] Structure containing addresses and sizes of buffers for the input and output data  
      ***SrcBuf*** [out] - address of the buffer in which decrypted data have to be placed to  
      ***szSrcBuf*** [out] - max size of the buffer in bytes;  
      ***CoreBuf*** [in] - address of the memory area containing the core;  
      ***szCoreBuf*** [in] - size of the core in bytes;  
      ***FlagBuf*** [in] - address of the memory area containing flags;  
      ***szFlagBuf*** [in] - size of flags in bytes;  
      ***RetCode*** [out] - termination code;  
      
**Return Values**  
    If the function succeeds, the return value is TRUE. If there is an error during decryption, the function returns FALSE. Termination code is in the field RetCode of the structure MV2_BUFFER. Decrypted data are placed to the memory area with the address from the field SrcBuf, the size is set by the fields szSrcBuf and szFalgBuf of the structure MV2_BUFFER. 
    
**Remarks**  
    Before using the function set the decryption key by the function MV2SetKey. The calling process has to allocate memory before using MV2Decrypt. 

###MV2GetStatus 
This function returns the current status of the process and is defined as
```c
int MV2GetStatus();
```
**Parameters:**  

**Return Values**  

**Remarks**  

###MV2GetStep 
The function returns the number of performed rounds and is defined as 
```c
int MV2GetStep();
```
**Parameters:**  
    This function has no parameters. 

**Return Values**  
   Function returns current status of the encryption\decryption process or an error code. 
   
###MV2Release(); 
The function destroys the MV2 object for the current thread and is defined as
```c
void MV2Release();
```
**Parameters:**  
    This function has no parameters. 

**Return Values**  
    The function does not return any value.


###MV2CalcRecomendedSizes
Helper function for calculating buffer sizes for CORE and FLAGS components. It defines as
```c
bool MV2CalcRecomendedSizes(  
	unsigned int cbSource, unsigned int dwStep, 
	unsigned int& cbCore, unsigned int& cbFlags
);
```
**Parameters:**  
***cbSource*** - [in] plaintext size  
***dwStep*** - [in] Number of rounds (if less than 50) or maximum CORE size (otherwise)  
***cbCore*** - [in]  \ [out] recommended CORE buffer size 
***cbFlags*** - [in] \ [out] recommended maximum buffer size for FLAGS  

**Return Values**
   Returns TRUE if successful or FALSE otherwise.

The function calculates the maximum buffer sizes and fills in the corresponding fields.

##Structures and constants

###MV2_BUFFER 
This structure contains the addresses and sizes of the input and output data and is defined as follows:
```c
typedef struct tagMV2_BUFFER {
	int		szSrcBuf; // plaintext buffer size 
	unsigned char*  SrcBuf; // pointer to plaintext buffer
	int	 	szCoreBuf; // buffer size for CORE data (encryption) or CORE data (decryption)
	unsigned char*  CoreBuf; // pointer to the CORE buffer
	int	 	szFlagBuf; // buffer size for FLAGS data (encryption) or FLAGS data (decryption)
	unsigned char*  FlagBuf; // pointer to the FLAGS buffer
	int		RetCode; // operation result code
}MV2_BUFFER;
```

###Defined Constants
```c
enum KeySize {
	sizeZero = 0,
	size256bits = 32,
	size512bits = 64,
	size1024bits = 128,
	size2048bits = 256,
	sizeLast
};
enum MV2ERROR {
	MV2ERR_WORK = -1,
	MV2ERR_DWORK = -2,
	MV2ERR_OK = 0,
	MV2ERR_SRCNOTEXIST = 1,
	MV2ERR_OPEN = 2,
	MV2ERR_INPUTRND = 22,
	MV2ERR_MEMALLOC = 40,
	MV2ERR_DECRYPT = 65,
	MV2ERR_WRITE = 77,
	MV2ERR_WRITEFLAG = 79,
	MV2ERR_CREATEOUT = 80,
	MV2ERR_KEYNOTSET = 99,
	MV2ERR_KEYINT = 101,
	MV2ERR_ENDFLAGS = 103,
	MV2ERR_ENDCORE = 104,
	MV2ERR_KEY = 105
};
```
