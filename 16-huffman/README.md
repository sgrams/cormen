# Huffman coding
# Usage  
Compile with command  
`gcc -o main main.c huff.c $(pkg-config --cflags glib-2.0 --libs glib-2.0)`  
or with command&nbsp;&nbsp;`make`  
# .heff File Structure
## Header
| info             | bytes |                              |
|------------------|-------|------------------------------|
| hello            | 4     | *0x46465548* or *0x46464548* |
| size             | 4     |                              |
| uniq_size        | 4     |                              |
| padding (0xFFFF) | 2     |                              |

## Dictionary entry
| info             | bytes |           |
|------------------|-------|-----------|
| code length      | 1     | *in bits* |
| unique byte      | 1     |           |
| encoded byte     |  4     |           |

## In between
| info             | bytes |           |
|------------------|-------|-----------|
| padding (0xFFFF) |       |           |

## Encoded stream
| info  | bytes |   |
|-------|-------|---|
| entry | ???   |   |


# To-Do
- Burrows-Wheelers Transformation and MoveToFront algorithm
- GTK+ GUI to select size of block (for BWT transformation)
- optimizing structure of huff file (enabling easy read&write to binary file)
- hashmap instead of list in the first structure (easier and faster operations like: minimal element extraction and insertion)
- *reminder* – if i want to take 3 last bits of guint16 I take 1<<2 + 1<<1 + 1
- *AES encryption*
# Info 
  This file is a part of 16-huffman task.  
  *Stanisław J. Grams*  