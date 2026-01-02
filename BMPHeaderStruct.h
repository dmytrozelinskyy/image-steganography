#pragma once

/**
 * @struct BMP_FileHeader
 * @struct BMP_FileInfoHeader
 * @brief BMP Information Struct
 * @var
 * <h2>BMP_FileHeader struct</h2>
 * <b>fileType</b>-> specifies the file type(always "BM" || '0x4D42' in hexadecimal representation<br>
 * <b>fileSize</b> -> file size (in 'bytes') <br>
 * <b>reserved</b> -> reserved (always 0) <br>
 * <b>dataOffset</b> -> offset from the beginning of file to the bitmap data (in 'bytes')<br><br>
 * <h2>BMP_FileInfoHeader struct</h2>
 * <b>size</b>-> number of bytes required by the structure<br>
 * <b>width</b> -> width of the image (in 'pixels') <br>
 * <b>height</b> -> height of the image (in 'pixels') <br>
 * <b>planes</b> -> number of color planes (always 1) <br>
 * <b>bitCount</b> -> number of bits per pixel<br>
 * <b>compression</b> -> type of compression<br>
 * <b>imageSize</b> -> size of the image data (in 'bytes') <br>
 * <b>xPixelsPerMeter</b> -> horizontal resolution (in pixels per meter) <br>
 * <b>yPixelsPerMeter</b> -> vertical resolution (in pixels per meter) <br>
 * <b>colorsUsed</b> -> number of colors used by the bitmap <br>
 * <b>colorsImportant</b> -> number of important colors <br>
 * @details
 * This structure is used to store information about .bmp files
 * @attention
 *   <p>A file is the sequence of bytes, so that is why we should cast our struct to char pointer to read the data from the file<br>
 *      General definition of file reading:<br> std::istream& read(char* s, std::streamsize n);  </p><br>
 */

#pragma pack(2)

struct BMP_FileHeader {
    uint16_t fileType{0x4D42};
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
};

struct BMP_FileInfoHeader {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  xPixelsPerMeter;
    int32_t  yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};

#pragma pack() // Reset pragma packaging



/**
 * @SourceOfInformation
 * <h3>ChatGPT(OpenAI):</h3>
 * <h4>1) Information about .bmp file type</h4>
 * &emsp; Me: Tell me about .bmp file type and what main properties it has<br><br>
 * &emsp; ChatGPT: The BMP file format, also known as Bitmap Image File or Device Independent Bitmap (DIB)
 *                 file format, is a raster graphics image file format used to store bitmap digital images,
 *                 independently of the display device. Here are the main properties and characteristics of
 *                 the BMP file format:<br>
 *                 &emsp;<b>Main Properties</b><br>
 *                 &emsp;File Extension and MIME Type: .ppm<br>
 *                 <p> &emsp;&emsp;- Extension: .bmp or .dib <br>
 *                     &emsp;&emsp;- MIME Type: image/bmp or image/x-ms-bmp <br></p>
 *                 &emsp;<b>File Structure</b>:<br>
 *                 <p> &emsp;&emsp;<b>- Header:</b> Contains information about the file itself, such as the file type, size, and layout.<br>
 *                 <p> &emsp;&emsp;&emsp;- File Type: Always starts with 'BM' for Bitmap.<br>
 *                     &emsp;&emsp;&emsp;- File Size: Size of the BMP file in bytes.<br>
 *                     &emsp;&emsp;&emsp;- Reserved: Reserved fields, typically set to zero.<br>
 *                     &emsp;&emsp;&emsp;- Offset: The starting address, in bytes, of the bitmap data.<br></p><br></p>
 *                 <p> &emsp;&emsp;<b>- DIB Header (Bitmap Information Header): Contains detailed information about the image.</b><br>
 *                 <p> &emsp;&emsp;&emsp;- Header Size: Size of this header.<br>
 *                     &emsp;&emsp;&emsp;- Image Width and Height: Dimensions of the image in pixels.<br>
 *                     &emsp;&emsp;&emsp;- Planes: Number of color planes (must be set to 1).<br>
 *                     &emsp;&emsp;&emsp;- Bits per Pixel: Color depth, typically 1, 4, 8, 16, 24, or 32 bits per pixel.<br>
 *                     &emsp;&emsp;&emsp;- Compression: Compression method used (if any).<br>
 *                     &emsp;&emsp;&emsp;- Image Size: Size of the raw bitmap data.<br>
 *                     &emsp;&emsp;&emsp;- Resolution: Horizontal and vertical resolution in pixels per meter.<br>
 *                     &emsp;&emsp;&emsp;- Colors Used: Number of colors in the color palette.<br>
 *                     &emsp;&emsp;&emsp;- Important Colors: Number of important colors; generally ignored.<br></p><br></p>
 *                 <p> &emsp;&emsp;<b>- Color Depth:</b><br>
 *                 <p> &emsp;&emsp;&emsp;- Can vary from monochrome (1-bit per pixel) to 24-bit true color images.<br>
 *                     &emsp;&emsp;&emsp;- Higher bits per pixel allow more colors and higher image quality.<br></p><br></p>
 *                 <p> &emsp;&emsp;<b>- Compression Methods:</b><br>
 *                 <p> &emsp;&emsp;&emsp;- BI_RGB: No compression (most common).<br>
 *                     &emsp;&emsp;&emsp;- BI_RLE8: Run-length encoding for 8-bit/pixel bitmaps.<br>
 *                     &emsp;&emsp;&emsp;- BI_RLE4: Run-length encoding for 4-bit/pixel bitmaps.<br>
 *                     &emsp;&emsp;&emsp;- BI_BITFIELDS: Specifies that the bitmap uses three color masks to define the actual colors.<br></p><br></p>
 *                 <p> &emsp;&emsp;<b>- Color Palette:</b><br>
 *                 <p> &emsp;&emsp;&emsp;- For images with 1, 4, or 8 bits per pixel, the BMP file includes a color palette.<br>
 *                     &emsp;&emsp;&emsp;- Each entry in the color palette is typically 4 bytes, representing a color in RGBA (Red, Green, Blue, Alpha) format.<br></p><br></p>
 *                 <p> &emsp;&emsp;<b>- Bitmap Data:</b><br>
 *                 <p> &emsp;&emsp;&emsp;- Pixel data stored in a row-wise manner.<br>
 *                     &emsp;&emsp;&emsp;- Each row is padded to a multiple of 4 bytes (to maintain alignment).<br></p><br></p>
 *                 <p> &emsp;&emsp;<b>- Image Origin:</b><br>
 *                 <p> &emsp;&emsp;&emsp;- The coordinate origin is at the bottom-left corner of the image.<br>
 *                     &emsp;&emsp;&emsp;- Pixels are stored in bottom-to-top, left-to-right order.<br></p><br></p>
 * <h4>2) Information about .bmp struct in C++</h4>
 * &emsp; Me: What data types we can use to create a struct that will represent BMP Header and BMP Information Header in C++?<br><br>
 * &emsp; ChatGPT: In C++, you can represent the BMP file header and BMP information header using struct data types.
 *                 Below are examples of how you can define these structures according to the
 *                 BMP file format specifications.<br>
 *   <p> &emsp;&emsp;<b>- Explanation: </b><br>
 *                  <p> &emsp;&emsp;&emsp;- #pragma pack(2) and #pragma pack():
 *                                          These directives ensure that the compiler aligns the struct members on 2-byte boundaries,
 *                                          which can be necessary for some BMP files or specific use cases where 2-byte alignment
 *                                          is required.<br>
 *                     &emsp;&emsp;&emsp;- Reading the headers: The file.read function reads the appropriate number of bytes
 *                                         from the file into the header structures. With 2-byte alignment, the structure may
 *                                         have different padding and alignment requirements compared to the default
 *                                         alignment.<br></p><br></p>
 * @code <code>#include <cstdint>  // For fixed-width integer types
 *
 *   #pragma pack(2)  // Ensure 2-byte alignment
 *
 *   struct BMPFileHeader {
 *       uint16_t bfType;      // Specifies the file type, must be 'BM'
 *       uint32_t bfSize;      // Specifies the size of the file in bytes
 *       uint16_t bfReserved1; // Reserved, must be zero
 *       uint16_t bfReserved2; // Reserved, must be zero
 *       uint32_t bfOffBits;   // Specifies the offset, in bytes, from the beginning of the file to the bitmap data
 *  };
 *
 *   struct BMPInfoHeader {
 *      uint32_t biSize;          // Specifies the number of bytes required by the structure
 *       int32_t biWidth;          // Specifies the width of the image, in pixels
 *       int32_t biHeight;         // Specifies the height of the image, in pixels
 *       uint16_t biPlanes;        // Specifies the number of color planes, must be 1
 *       uint16_t biBitCount;      // Specifies the number of bits per pixel (1, 4, 8, 16, 24, or 32)
 *       uint32_t biCompression;   // Specifies the type of compression (0 = BI_RGB, uncompressed)
 *       uint32_t biSizeImage;     // Specifies the size of the image data, in bytes (can be 0 for uncompressed)
 *       int32_t biXPelsPerMeter;  // Specifies the horizontal resolution, in pixels per meter
 *       int32_t biYPelsPerMeter;  // Specifies the vertical resolution, in pixels per meter
 *       uint32_t biClrUsed;       // Specifies the number of colors used by the bitmap
 *       uint32_t biClrImportant;  // Specifies the number of important colors, generally ignored
 *   };
 *
 *   #pragma pack()  // Restore the default alignment</code>
*/