#pragma once

/**
 * @struct PPM_FileHeader
 * @brief PPM Information Struct
 * @var
 * <b>magic_number</b>-> represents magic number assigned<br> to every .ppm file(type)<br>
 * <b>width</b> -> image width<br>
 * <b>height</b> -> image height<br>
 * <b>max_color_val</b> -> maximum color value that can be assigned<br>
 * <b>image_data</b> -> the same as pixel data
 * @details
 * This structure is used to store information about .ppm files
 */

struct PPM_FileHeader{
    std::string magic_number;
    int width;
    int height;
    int max_color_val;
    std::vector<unsigned char> image_data;
};

/**
 * @SourceOfInformation
 * <h3>ChatGPT(OpenAI):</h3>
 * <h4>1) Information about .ppm file type</h4>
 * &emsp; Me: Tell me about .ppm file type and what main properties it has<br><br>
 * &emsp; ChatGPT: A .ppm file stands for "Portable Pixmap Format," which is a simple
 *                 format for storing image data in a plain text or binary format.
 *                 It is part of the PNM (Portable Any Map) family of formats and is
 *                 often used for storing and transferring images because of its simplicity
 *                 and ease of use. Here are the main properties and characteristics of the .ppm file format:<br>
 *                 &emsp;<b>Main Properties</b><br>
 *                 &emsp;File Extension: .ppm<br>
 *                 &emsp;<b>Format</b>: Can be either plain text (ASCII) or binary.<br>
 *                 <p> &emsp;&emsp;- Plain Text (ASCII): Easier to read and edit by humans, but larger in file size.<br>
 *                     &emsp;&emsp;- Binary: More compact, less readable by humans, but smaller in file size.<br></p>
 *                 &emsp;<b>Header</b>: The file starts with a header that provides essential information about the image. The header consists of:<br>
 *                 <p> &emsp;&emsp;- Magic Number: Identifies the file type. For PPM files, this can be P3 (plain text) or P6 (binary).<br>
 *                     &emsp;&emsp;- Width and Height: Specifies the dimensions of the image in pixels.<br>
 *                     &emsp;&emsp;- Maximum Color Value: Typically 255, indicating the maximum value for a color component.</p><br>
 *                 &emsp;<b>Pixel Data:</b><br>
 *                 <p> &emsp;&emsp;- Plain Text (P3): Pixel values are listed in ASCII format, with each pixel represented by its red, green, and blue components.<br>
 *                     &emsp;&emsp;- Binary (P6): Pixel values are stored as binary data, which is more compact.</p><br>
 *                 &emsp;<b>Key Features</b><br>
 *                 <p> &emsp;&emsp;- Simplicity: The format is straightforward and easy to understand, making it an excellent choice for teaching and learning about image processing.<br>
 *                     &emsp;&emsp;- No Compression: PPM files do not use any compression, so they can be quite large compared to other image formats.<br>
 *                     &emsp;&emsp;- No Metadata: PPM files do not store any metadata beyond the basic header information (dimensions and color depth).<br>
 *                     &emsp;&emsp;- Support: Widely supported by many graphics and image processing applications.</p><br>
*/