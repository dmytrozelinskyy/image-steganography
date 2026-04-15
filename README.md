# Image Steganography
A C++ command-line tool for hiding and extracting secret messages inside image files using the <b>LSB (Least Significant Bit)</b> technique.

## Overview
This project was developed during my C++ coursework at university. I was fascinated by the idea of hiding information inside an image file without visibly changing its appearance - the image before and after encryption should look identical to the human eye. I started by exploring different image formats and chose BMP and PPM as they have simple, well-documented structures that are straightforward to work with at a binary level.

## How it works
Each pixel storws 3 color channels (RGB), each 8 bits wide. By modifying the 2 least significant bits of each channel, we can store 6 bits per pixel. The change in color value is at most +-3 out of 255 - imperceptible to the human eye. 

The message is prefixed with 32-bit length header embedded directly into the first pixels of the image, so decryption knows exactly how manyy bits to extract without relying on any external files.

## Supported Formats
| Format | Extension | Restriction |
| ------ | --------- | ----------- |
| BMP (Bitmap) | '.bmp' | 24-bit uncompressed only |
| PPM (Portable Pixmap) | 'ppm' | P6 binary only |

## Build
```bash
git clone https://github.com//dmytrozelinskyy/image-steganography
cd image-steganography
cmake -B build
cmake --build build
```

## Usage
| Flag | Arguments | Description |
| ------ | --------- | ----------- |
| `-e` / `-encrypt` | `<path> <message>` | Encrypt message into image |
| `-d` / `-decrypt` | `<path>` | Decrypt message from image |
| `-i` / `-info` | `<path>` | Display image information |
| `-c` / `-check` | `<path> <message>` | Check if message fits in image |
| `-h` / `-help` | | Show help |

## Examples
```bash
# Encrypt a message
./imgsteg -e ./assets/sample3.bmp "Hello World"

# Decrypt a message
./imsteg -d ./assets/sample3_encrypted.bmp

# Check if message fits
./imsteg -c ./assets/sample3.bmp "Hello World"

# Display image info
./imsteg -i ./assets/sample3.bmp
```

## Limitations
- BMP: 24-bit uncompressed only. Compressed or paletted BMP files are not supported. Standard BITMAPINFOHEADER (40-byte info header) only - files with extended V4/V5 headers are partially read but pixel data is still accessed correctly via dataOffset.
- PPM: P6 binary only. P3 plain text formati is not supported. 
- Message size is limited by image dimensions. Capacity is calculated as: <br>
     `capacity (bytes) = (width x height x 6) / 8` <br>
  For example, a 100x100 image can store up to 7,500 bytes. Use '-c' to check before encrypting.

## License
MIT
